#pragma once
#include <LedControl.h>

constexpr int dataInPin = 12;
constexpr int clockPin = 10;
constexpr int loadPin = 11;
LedControl lc = LedControl(dataInPin, clockPin, loadPin, 1);

void initializeDisplay() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
}

void setTime(int timeMilli, bool left) {
  int offset = left ? 4 : 0;
  lc.setDigit(0, 0 + offset, timeMilli / 1000 % 10, 0);
  lc.setDigit(0, 1 + offset, timeMilli / 10000 % 6, 0);
  lc.setDigit(0, 2 + offset, timeMilli / 60000 % 10, 1);
  if (timeMilli < 600000) {
    lc.setRow(0, 3 + offset, 0);
  } else {
    lc.setDigit(0, 3 + offset, timeMilli / 600000 % 6, 0);
  }
}

int regularLedUpdateTime = 0;
void setTime(int leftTimeMilli, int rightTimeMilli) {
  constexpr int minUpdateTime = 100;
  int now = millis();
  if (now - regularLedUpdateTime < minUpdateTime) {
    return;
  }
  regularLedUpdateTime = now;
  setTime(leftTimeMilli, true);
  setTime(rightTimeMilli, false);
}

int flashLedUpdateTime = 0;
int flashLedActive = false;
void flashLed(bool left) {
  constexpr int flashTime = 250;
  int now = millis();
  if (now - flashLedUpdateTime < flashTime) {
    return;
  }
  flashLedUpdateTime = now;
  flashLedActive = !flashLedActive;
  int offset = left ? 4 : 0;
  if (flashLedActive) {
    for (int i = 0; i < 4; i++) {
      lc.setRow(0, i + offset, 0);
    }
  } else {
    for (int i = 0; i < 4; i++) {
      lc.setDigit(0, i + offset, 0, i == 2);
    }
  }
}