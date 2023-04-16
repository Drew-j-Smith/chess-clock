
#include <ArduinoBLE.h>
#include <LedControl.h>
#include <ArduinoJson.h>

constexpr int bleStringLength = 80;
BLEService bleService("00000000-0000-1000-8000-00805f9b34fb");
BLEStringCharacteristic
    bleStringCharacteristic("741c12b9-e13c-4992-8a5e-fce46dec0bff",
                            BLERead | BLENotify | BLEWrite, bleStringLength);
BLEDevice centralBleDevice;

enum State {
  STOPPED,
  LEFT_TIME_RUNNING,
  RIGHT_TIME_RUNNING,
  LEFT_FLAG,
  RIGHT_FLAG,
};

constexpr int dataInPin = 12;
constexpr int clockPin = 10;
constexpr int loadPin = 11;
LedControl lc = LedControl(dataInPin, clockPin, loadPin, 1);

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
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  // button setup
  pinMode(leftPin, INPUT_PULLUP);
  pinMode(rightPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(leftPin), leftButtonPress, RISING);
  attachInterrupt(digitalPinToInterrupt(rightPin), rightButtonPress, RISING);

  // bluetooth setup
  BLE.begin();
  BLE.setLocalName("Nano ChessClock");
  BLE.setAdvertisedService(bleService);
  bleService.addCharacteristic(bleStringCharacteristic);
  BLE.addService(bleService);
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
  int actualLeftTime = leftTime;
  int actualRightTime = rightTime;
  int now = millis();
  switch (state) {
  case State::LEFT_TIME_RUNNING: {
    actualLeftTime = leftTime - now + lastButtonPressTime;
    setTime(actualLeftTime, true);
    setTime(rightTime, false);
    if (actualLeftTime <= 0) {
      state = State::LEFT_FLAG;
      leftTime = 0;
      changed = true;
    }
    break;
  }
  case State::RIGHT_TIME_RUNNING: {
    actualRightTime = rightTime - now + lastButtonPressTime;
    setTime(leftTime, true);
    setTime(actualRightTime, false);
    if (actualRightTime <= 0) {
      state = State::RIGHT_FLAG;
      rightTime = 0;
      changed = true;
    }
    break;
  }
  case State::LEFT_FLAG: {
    while (true) {
      flashLed(true);
    }
  }
  case State::RIGHT_FLAG: {
    while (true) {
      flashLed(false);
    }
  }
  default: {
    setTime(leftTime, true);
    setTime(rightTime, false);
    break;
  }
  }
  delay(100);
  if (centralBleDevice && changed) {
    handleBluetoothUpdate(actualLeftTime, actualRightTime);
    changed = false;
  } else {
    centralBleDevice = BLE.central();
  }
}

void handleBluetoothUpdate(int actualLeftTime, int actualRightTime) {
  StaticJsonDocument<bleStringLength> doc;
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
  bleStringCharacteristic.writeValue(bleMessage);
  changed = false;
}

void leftButtonPress() {
  int now = millis();
  switch (state) {
  case State::RIGHT_FLAG:
  case State::LEFT_FLAG:
  case State::RIGHT_TIME_RUNNING:
    return;
  case State::STOPPED: {
    lastButtonPressTime = now;
    leftIsWhite = false;
    break;
  }
  default:
    break;
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
  case State::LEFT_TIME_RUNNING:
    return;
  case State::STOPPED: {
    lastButtonPressTime = now;
    leftIsWhite = true;
    break;
  }
  default:
    break;
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
