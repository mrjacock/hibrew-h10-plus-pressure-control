# Safety

This project involves a mains-powered espresso machine, low-voltage control electronics, water, heat, and mechanical pressure.

Do not build this mod unless you understand the risks and have the skills and tools required to work safely around mains voltage and non-isolated circuits.

## Hard rules

- Unplug the espresso machine before opening it.
- Do not work on wiring while the machine is connected to mains.
- Never connect the Arduino USB cable to a PC while the machine is powered from mains.
- Keep all low-voltage wiring away from mains wiring, hot parts, sharp edges, and moving parts.
- Insulate every solder joint and exposed conductor.
- Mount the Arduino so it cannot move, touch metal parts, touch hot parts, or get wet.
- Leak-test the hydraulic circuit before closing the machine.
- Do not use grounded bench instruments unless you fully understand the isolation situation.

## USB warning

This is the failure mode to avoid:

```text
Espresso machine powered from mains + Arduino USB connected to a computer
```

On the tested machine, the low-voltage electronics may not be safely isolated from mains. Connecting USB at the same time can create unwanted ground paths through the computer, USB cable, Arduino, and machine electronics.

Safe firmware upload state:

```text
Espresso machine unplugged from mains + Arduino connected to USB
```

## Measurement tools

Recommended:

- battery-powered oscilloscope;
- properly isolated differential probes;
- reliable multimeter;
- insulated tools;
- heat shrink tubing and strain relief;
- blind basket or pressurized basket for pressure testing.

Avoid:

- grounded oscilloscope probes on unknown non-isolated nodes;
- loose USB connections during powered machine tests;
- temporary exposed wiring inside the machine;
- relying on tape alone near heat and humidity.

## Hydraulic pressure

Even if the electrical side is correct, a leak inside an espresso machine can damage electronics, create shock hazards, or spray hot water.

Before normal use:

1. run a cold leak test;
2. run a hot leak test;
3. test with a blind basket or pressurized basket;
4. inspect every fitting again after the machine cools down.

## Responsibility

This documentation is experimental and provided without warranty. You are responsible for your own machine, tools, wiring, measurements, and safety.
