#pragma once
#include "state.h"
#include <ArduinoBLE.h>

BLEService bleService("00000000-0000-1000-8000-00805f9b34fb");

// clang-format off
BLEIntCharacteristic stateCharacteristic         ("15ee2281-96c3-47b9-8041-e4c3723d053d", BLERead | BLENotify);
BLEIntCharacteristic leftTimeCharacteristic      ("3c5b2478-a783-4eeb-acfa-0a9c48171d30", BLERead | BLEWrite);
BLEIntCharacteristic rightTimeCharacteristic     ("e46c0d94-a1bc-4e29-9ce6-c2b8899d7622", BLERead | BLEWrite);
BLEIntCharacteristic leftResetTimeCharacteristic ("ec8c5792-e5c7-485c-a1c5-8f43449ba0b5", BLERead | BLEWrite);
BLEIntCharacteristic rightResetTimeCharacteristic("ade8080a-8d5e-48c4-b302-932b08c21bc1", BLERead | BLEWrite);
BLEIntCharacteristic leftIncrementCharacteristic ("d9a4242e-2f41-471b-8bca-00b7650a053c", BLERead | BLEWrite);
BLEIntCharacteristic rightIncrementCharacteristic("a98cfae5-3fe4-452c-8c1f-7a15820b456e", BLERead | BLEWrite);
BLEIntCharacteristic leftDelayCharacteristic     ("8dcd2bd0-e4d8-416e-b2e9-cf8231cb28de", BLERead | BLEWrite);
BLEIntCharacteristic rightDelayCharacteristic    ("084ed1a2-da8e-48d2-9dcc-c328753639c2", BLERead | BLEWrite);

BLEDescriptor stateDescriptor         ("2901", "state");
BLEDescriptor leftTimeDescriptor      ("2901", "leftTime");
BLEDescriptor rightTimeDescriptor     ("2901", "rightTime");
BLEDescriptor leftResetTimeDescriptor ("2901", "leftResetTime");
BLEDescriptor rightResetTimeDescriptor("2901", "rightResetTime");
BLEDescriptor leftIncrementDescriptor ("2901", "leftIncrement");
BLEDescriptor rightIncrementDescriptor("2901", "rightIncrement");
BLEDescriptor leftDelayDescriptor     ("2901", "leftDelay");
BLEDescriptor rightDelayDescriptor    ("2901", "rightDelay");
// clang-format on

void intializeBluetooth() {
  BLE.begin();
  BLE.setLocalName("Nano ChessClock");
  BLE.setAdvertisedService(bleService);

  bleService.addCharacteristic(stateCharacteristic);
  bleService.addCharacteristic(leftTimeCharacteristic);
  bleService.addCharacteristic(rightTimeCharacteristic);
  bleService.addCharacteristic(leftResetTimeCharacteristic);
  bleService.addCharacteristic(rightResetTimeCharacteristic);
  bleService.addCharacteristic(leftIncrementCharacteristic);
  bleService.addCharacteristic(rightIncrementCharacteristic);
  bleService.addCharacteristic(leftDelayCharacteristic);
  bleService.addCharacteristic(rightDelayCharacteristic);

  stateCharacteristic.addDescriptor(stateDescriptor);
  leftTimeCharacteristic.addDescriptor(leftTimeDescriptor);
  rightTimeCharacteristic.addDescriptor(rightTimeDescriptor);
  leftResetTimeCharacteristic.addDescriptor(leftResetTimeDescriptor);
  rightResetTimeCharacteristic.addDescriptor(rightResetTimeDescriptor);
  leftIncrementCharacteristic.addDescriptor(leftIncrementDescriptor);
  rightIncrementCharacteristic.addDescriptor(rightIncrementDescriptor);
  leftDelayCharacteristic.addDescriptor(leftDelayDescriptor);
  rightDelayCharacteristic.addDescriptor(rightDelayDescriptor);

  leftTimeCharacteristic.setEventHandler(
      BLEWritten, [](BLEDevice, BLECharacteristic) {
        leftTime = leftTimeCharacteristic.value();
      });
  rightTimeCharacteristic.setEventHandler(
      BLEWritten, [](BLEDevice, BLECharacteristic) {
        rightTime = rightTimeCharacteristic.value();
      });
  leftResetTimeCharacteristic.setEventHandler(
      BLEWritten, [](BLEDevice, BLECharacteristic) {
        leftResetTime = leftResetTimeCharacteristic.value();
      });
  rightResetTimeCharacteristic.setEventHandler(
      BLEWritten, [](BLEDevice, BLECharacteristic) {
        rightResetTime = rightResetTimeCharacteristic.value();
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