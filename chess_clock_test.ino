
#include <LedControl.h>
#include <ArduinoBLE.h>
#include <ArduinoJson.h>

constexpr int characteristicStateLength = 40;
BLEService clockBLEService("180A");
BLEStringCharacteristic bleCharacteristicState("2A57", BLERead | BLENotify | BLEWrite, characteristicStateLength);
BLEDevice centralBleDevice;

enum State {
  STOPPED,
  LEFT_TIME_RUNNING,
  RIGHT_TIME_RUNNING,
  LEFT_FLAG,
  RIGHT_FLAG,
};

constexpr int dataInPin = 12;
constexpr int clkPin = 10;
constexpr int loadPin = 11;
LedControl lc = LedControl(dataInPin, clkPin, loadPin, 1);

constexpr int leftPin = 2;
constexpr int rightPin = 3;

volatile int leftTime = 10000;
volatile int rightTime = 10000;
volatile State state = State::STOPPED;
volatile int lastButtonPressTime = 0;
volatile bool changed = true;
volatile bool leftIsWhite = true;

void setup() {
  // led setup
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);

  // button setup
  pinMode(leftPin, INPUT_PULLUP);
  pinMode(rightPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(leftPin), leftButtonPress, RISING);
  attachInterrupt(digitalPinToInterrupt(rightPin), rightButtonPress, RISING);

  // bluetooth setup
  BLE.begin();
  BLE.setLocalName("Nano ChessClock");
  BLE.setAdvertisedService(clockBLEService);
  clockBLEService.addCharacteristic(bleCharacteristicState);
  BLE.addService(clockBLEService);
  BLE.advertise();
  centralBleDevice = BLE.central();
}

void setTime(int timeMilli, bool left) {
  int digits[4];
  digits[0] = timeMilli / 1000 % 10;
  digits[1] = timeMilli / 10000 % 6;
  digits[2] = timeMilli / 60000 % 10;
  digits[3] = timeMilli / 600000 % 6;
  int offset = left ? 4 : 0;
  for (int i = 0; i < 4; i++) {
    if (digits[i] == 0 && i == 3) {
      lc.setRow(0, i + offset, 0);
    } else {
      lc.setDigit(0, i + offset, digits[i], i == 2); 
    }
  }
}

void flashLed(bool left) {
  constexpr int flashTime = 250;
  int offset = left ? 4 : 0;
  for (int i = 0; i < 4; i++) {
    lc.setRow(0, i + offset, 0);
  }
  delay(flashTime);
  for (int i = 0; i < 4; i++) {
    lc.setDigit(0, i + offset, 0, i == 2);
  }
  delay(flashTime);
}


void loop() {
  int now = millis();
  int actualLeftTime = state == State::LEFT_TIME_RUNNING ? leftTime - now + lastButtonPressTime : leftTime;
  int actualRightTime = state == State::RIGHT_TIME_RUNNING ? rightTime - now + lastButtonPressTime : rightTime;  
  if (actualLeftTime <= 0) {
    state = State::LEFT_FLAG;
    leftTime = 0;
    actualLeftTime = 0;
    changed = true;
  }
  if (actualRightTime <= 0) {
    state = State::RIGHT_FLAG;
    rightTime = 0;
    actualRightTime = 0;
    changed = true;
  }
  if(centralBleDevice){
      // if (bleCharacteristicState.written()) {
      //   handleBluetoothWritten();
      // } else 
      if (changed) {
        handleBluetoothUpdate(actualLeftTime, actualRightTime);
      }
  } else {
    centralBleDevice = BLE.central();
  }
  if (state == State::LEFT_FLAG) {
    setTime(actualRightTime, false);
    flashLed(true);
  } else if (state == State::RIGHT_FLAG) {
    setTime(actualLeftTime, true);
    flashLed(false);
  } else {
    setTime(actualLeftTime, true);
    setTime(actualRightTime, false);
    delay(100);
  }
}

void handleBluetoothWritten() {
  StaticJsonDocument<characteristicStateLength> doc;
  DeserializationError error = deserializeJson(doc, bleCharacteristicState.value());
  if (error) {
    return; // no real error handling available
  }

  if (doc.containsKey("leftIsWhite")) {
    leftIsWhite = doc["leftIsWhite"];
  }
  changed = true;

  if (leftIsWhite) {
    leftTime = doc["white"];
    rightTime = doc["black"];
    String doc_state = doc["state"];
    if (doc_state == "white_time_running") {
      state = State::LEFT_TIME_RUNNING;
    } else if (doc_state == "black_time_running") {
      state = State::RIGHT_TIME_RUNNING;
    } else if (doc_state == "white_flag") {
      state = State::RIGHT_FLAG;
    } else if (doc_state == "black_flag") {
      state = State::LEFT_FLAG;
    } else {
      state = State::STOPPED;
    }
  } else {
    leftTime = doc["black"];
    rightTime = doc["white"];
    String doc_state = doc["state"];
    if (doc_state == "black_time_running") {
      state = State::LEFT_TIME_RUNNING;
    } else if (doc_state == "white_time_running") {
      state = State::RIGHT_TIME_RUNNING;
    } else if (doc_state == "black_flag") {
      state = State::RIGHT_FLAG;
    } else if (doc_state == "white_flag") {
      state = State::LEFT_FLAG;
    } else {
      state = State::STOPPED;
    }
  }
  
}

void handleBluetoothUpdate(int actualLeftTime, int actualRightTime) {
  StaticJsonDocument<characteristicStateLength> doc;
  if (leftIsWhite) {
    doc["white"] = actualLeftTime;
    doc["black"] = actualRightTime;
    switch (state) {
      case State::STOPPED: doc["state"] = "stopped"; break;
      case State::LEFT_TIME_RUNNING: doc["state"] = "white_time_running"; break;
      case State::RIGHT_TIME_RUNNING: doc["state"] = "black_time_running"; break;
      case State::RIGHT_FLAG: doc["state"] = "white_flag"; break;
      case State::LEFT_FLAG: doc["state"] = "black_flag"; break;
    }
  } else {
    doc["white"] = actualRightTime;
    doc["black"] = actualLeftTime;
    switch (state) {
      case State::STOPPED: doc["state"] = "stopped"; break;
      case State::LEFT_TIME_RUNNING: doc["state"] = "black_time_running"; break;
      case State::RIGHT_TIME_RUNNING: doc["state"] = "white_time_running"; break;
      case State::RIGHT_FLAG: doc["state"] = "black_flag"; break;
      case State::LEFT_FLAG: doc["state"] = "white_flag"; break;
    }
  }

  String bleMessage = "";
  serializeJson(doc, bleMessage);
  bleCharacteristicState.writeValue(bleMessage);
  changed = false;
}

void leftButtonPress() {
  int now = millis();
  switch (state) {
    case State::RIGHT_FLAG:
    case State::LEFT_FLAG:
    case State::RIGHT_TIME_RUNNING: return;
    case State::STOPPED: {
      lastButtonPressTime = now;
      leftIsWhite = false;
    }
    default: break;
  }
  state = State::RIGHT_TIME_RUNNING;
  leftTime = leftTime - now + lastButtonPressTime;
  if (leftTime <= 0) {
    state = State::LEFT_FLAG;
    leftTime = 0;
  }
  lastButtonPressTime = now;
  changed = true;
}

void rightButtonPress() {
  int now = millis();
  switch (state) {
    case State::RIGHT_FLAG:
    case State::LEFT_FLAG:
    case State::LEFT_TIME_RUNNING: return;
    case State::STOPPED: {
      lastButtonPressTime = now;
      leftIsWhite = true;
    }
    default: break;
  }
  state = State::LEFT_TIME_RUNNING;
  rightTime = rightTime - now + lastButtonPressTime;
  if (rightTime <= 0) {
    state = State::RIGHT_FLAG;
    rightTime = 0;
  }
  lastButtonPressTime = now;
  changed = true;
}
