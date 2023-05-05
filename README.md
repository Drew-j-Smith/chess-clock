# chess-clock

The goal of this project is to create a chess clock which can easily comunicate with a computer. The clock that comes with the DGT board has too much lag and not enough information. This chess clock aims to fix those issues.

## Parts

This project use the Arduino Nano 33 IOT for its small form factor and its bluetooth capabilities.Additionally (in the physical) prototype keyboard switches were used to give easy actuation to the keys.A 8 unit MAX7219 display was used to display the times with a 4 AA batteries to power it.

## Wiring

Here is a diagram of how it was wired. (The diagram shows a micro but it should be a Nano 33 IOT
but all of the pins should be the same). The pins numbers can be modified in `chess_clock/buttons.h` and `chess_clock/display.h`

![chess-clock-diagram](https://raw.githubusercontent.com/Drew-j-Smith/chess-clock/main/circuit.png)

## Libraries/Requirements

### Arduino

- ArduinoBLE
- LedControl
- InputDebounce

### Web

- Support for the Web Bluetooth API
- TypeScript
