# Firmware

This folder contains the working Arduino Nano firmware for the HiBREW H10A Plus pressure-control mod.

Main sketch:

```text
hibrew_h10_pressure_control.ino
```

## Hardware target

- Arduino Nano
- ATmega328P
- 5 V logic
- Direct AVR port access on Port D

## Pinout

| Signal | Arduino pin |
|---|---:|
| Original MOTO input | `D2` |
| Modified MOTO output | `D5` |
| ZERO input, currently unused | `D3` |
| Pressure sensor analog output | `A4` |
| 5 V | `5V` |
| Ground | `GND` |

## Upload warning

Upload the firmware only when the espresso machine is unplugged from mains.

Never connect the Arduino USB cable to a computer while the machine is powered from mains.

## Notes

The firmware intentionally avoids Serial debugging and disables Timer0 for timing stability. Do not add `delay()`, `millis()`, or Serial debugging unless you understand the timing impact and the safety implications.
