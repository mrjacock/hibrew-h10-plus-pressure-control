# Testing checklist

## Before USB upload

- Machine unplugged from mains.
- Arduino connected to USB only after the machine is unplugged.
- No exposed conductors touching the machine frame or each other.
- Firmware compiled for Arduino Nano / ATmega328P.

## Before powering the machine

- USB cable disconnected from the Arduino.
- Machine reassembled enough to prevent accidental contact with internal wiring.
- Pressure sensor connected and mechanically secured.
- Hydraulic fittings checked.
- Original pump path understood.

## First live tests

- Keep the test short.
- Watch the analog pressure gauge.
- Be ready to cut mains power.
- Do not connect USB while the machine is powered.
- After the test, unplug the machine before reconnecting USB or inspecting the Arduino.

## Debugging

Because live USB serial debugging is unsafe in this setup, use offline debugging methods such as EEPROM logging or controlled test firmware.
