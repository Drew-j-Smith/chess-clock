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

int leftTime = 10000;
int rightTime = 10000;
int leftResetTime = 10000;
int rightResetTime = 10000;
int leftIncrement = 0;
int rightIncrement = 0;
int leftDelay = 0;
int rightDelay = 0;
State state = State::STOPPED;
bool changed = true;
bool leftIsWhite = true;