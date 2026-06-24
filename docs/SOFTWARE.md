# Software

## Firmware style

The firmware targets an Arduino Nano with an ATmega328P.

It is still Arduino/AVR C++, but it is not written in the usual beginner Arduino style. The timing-sensitive part avoids slow convenience calls such as `digitalRead()` and `digitalWrite()` and instead uses direct AVR port access.

The running control loop also avoids `delay()`, `millis()`, and `Serial` because predictable timing matters and because live USB serial debugging is unsafe while the machine is powered from mains.

## AI-assisted / vibe-coded note

The firmware was AI-assisted / vibe-coded during development.

That does not mean the code was accepted blindly. The final logic was reviewed line by line, timing-critical behavior was kept deliberately small, and AI-generated changes were treated as suggestions rather than authority. The AI was intentionally given as little freedom as possible around safety-critical and mains-adjacent behavior.

## Main control idea

The original machine controller still decides when pump activity is requested. The Arduino reads that original `MOTO` signal and then decides whether to pass or block it depending on pressure.

```text
Pressure below target window  -> pass pump pulses
Pressure above target window  -> block pump pulses
Pressure falls again          -> resume pump pulses
```

The result is burst / half-cycle skipping control, not a full phase-angle dimmer.

## Why not normal Serial debugging?

Because the Arduino USB port must not be connected to a computer while the espresso machine is powered from mains.

That restriction changes the entire debug workflow. Runtime behavior should be tested with planned, short test runs and then inspected offline after the machine has been unplugged and the Arduino is safe to reconnect by USB.

## Firmware location

Place the working sketch here:

```text
firmware/hibrew_h10_pressure_control/hibrew_h10_pressure_control.ino
```

Arduino IDE expects the `.ino` file to be inside a folder with the same name.
