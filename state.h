#pragma once

#include <cstdint>

enum State {
  STOPPED,
  PAUSED,
  LEFT_TIME_RUNNING,
  RIGHT_TIME_RUNNING,
  LEFT_FLAG,
  RIGHT_FLAG,
};

volatile int leftTime = 10000;
volatile int rightTime = 10000;
volatile int leftIncrement = 0;
volatile int rightIncrement = 0;
volatile int leftDelay = 0;
volatile int rightDelay = 0;
volatile State state = State::STOPPED;
volatile int lastButtonPressTime = 0;
volatile bool changed = true;
volatile bool leftIsWhite = true;