#pragma once
#include "state.h"
#include <ArduinoBLE.h>

BLEService bleService("00000000-0000-1000-8000-00805f9b34fb");

// clang-format off
BLEIntCharacteristic stateCharacteristic         ("15ee2281-96c3-47b9-8041-e4c3723d053d", BLERead | BLENotify);
BLEIntCharacteristic leftTimeCharacteristic      ("3c5b2478-a783-4eeb-acfa-0a9c48171d30", BLERead | BLEWrite);
BLEIntCharacteristic rightTimeCharacteristic     ("e46c0d94-a1bc-4e29-9ce6-c2b8899d7622", BLERead | BLEWrite);
BLEIntCharacteristic leftIncrementCharacteristic ("d9a4242e-2f41-471b-8bca-00b7650a053c", BLERead | BLEWrite);
BLEIntCharacteristic rightIncrementCharacteristic("a98cfae5-3fe4-452c-8c1f-7a15820b456e", BLERead | BLEWrite);
BLEIntCharacteristic leftDelayCharacteristic     ("8dcd2bd0-e4d8-416e-b2e9-cf8231cb28de", BLERead | BLEWrite);
BLEIntCharacteristic rightDelayCharacteristic    ("084ed1a2-da8e-48d2-9dcc-c328753639c2", BLERead | BLEWrite);
// clang-format on

void intializeBluetooth() {
  BLE.begin();
  BLE.setLocalName("Nano ChessClock");
  BLE.setAdvertisedService(bleService);

  bleService.addCharacteristic(stateCharacteristic);
  bleService.addCharacteristic(leftTimeCharacteristic);
  bleService.addCharacteristic(rightTimeCharacteristic);
  bleService.addCharacteristic(leftIncrementCharacteristic);
  bleService.addCharacteristic(rightIncrementCharacteristic);
  bleService.addCharacteristic(leftDelayCharacteristic);
  bleService.addCharacteristic(rightDelayCharacteristic);

  leftTimeCharacteristic.setEventHandler(
      BLEWritten, [](BLEDevice, BLECharacteristic) {
        leftTime = leftTimeCharacteristic.value();
      });
  rightTimeCharacteristic.setEventHandler(
      BLEWritten, [](BLEDevice, BLECharacteristic) {
        rightTime = rightTimeCharacteristic.value();
      });
  leftIncrementCharacteristic.setEventHandler(
      BLEWritten, [](BLEDevice, BLECharacteristic) {
        leftIncrement = leftIncrementCharacteristic.value();
      });
  rightIncrementCharacteristic.setEventHandler(
      BLEWritten, [](BLEDevice, BLECharacteristic) {
        rightIncrement = rightIncrementCharacteristic.value();
      });
  leftDelayCharacteristic.setEventHandler(
      BLEWritten, [](BLEDevice, BLECharacteristic) {
        leftDelay = leftDelayCharacteristic.value();
      });
  rightDelayCharacteristic.setEventHandler(
      BLEWritten, [](BLEDevice, BLECharacteristic) {
        rightDelay = rightDelayCharacteristic.value();
      });

  BLE.addService(bleService);
  BLE.advertise();
}

void updateBluetooth(int actualLeftTime, int actualRightTime) {
  leftTimeCharacteristic.writeValue(actualLeftTime);
  rightTimeCharacteristic.writeValue(actualRightTime);
  // write state last to notify
  stateCharacteristic.writeValue((int)state + (leftIsWhite ? 8 : 0));
}