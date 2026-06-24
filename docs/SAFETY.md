# Safety

## DANGER: non-isolated electronics

Do not connect the Arduino USB cable to a computer while the espresso machine is powered from mains.

The low-voltage electronics inside the tested HiBREW H10A Plus must not be assumed to be safely isolated from mains. A USB connection can create unwanted ground paths through the Arduino, the computer, the oscilloscope, the espresso machine, or the building earth system.

```text
SAFE upload/debug state:
Espresso machine unplugged from mains + Arduino connected by USB

UNSAFE state:
Espresso machine powered from mains + Arduino connected by USB to a PC
```

## Practical consequences

This project is harder to debug than a normal Arduino project.

Normal live `Serial` debugging is not appropriate while the machine is powered from mains, because the Arduino USB cable would connect the machine electronics to a computer. The firmware therefore avoids relying on `Serial` during real operation.

Use safer workflows:

- upload firmware only when the espresso machine is unplugged;
- disconnect USB before powering the machine from mains;
- perform live machine tests without a PC connected to the Arduino;
- use EEPROM logging or other offline methods when runtime data is needed;
- power down and unplug the machine before inspecting, rewiring, or reconnecting USB.

## Mains warning

This modification involves a mains-powered appliance, a TRIAC pump power stage, water, metal parts, and low-voltage control wiring. That combination is dangerous if handled casually.

Do not work on the machine while it is plugged in. Do not touch internal wiring while the machine is powered. Do not assume that a connector is safe just because it measures around 5 V in normal operation.

## Hydraulic warning

The coffee circuit can be hot and pressurized. Let the machine cool down and depressurize before opening hydraulic fittings or removing the pressure sensor branch.

## Responsibility

This repository documents an experimental personal modification. It is not a certified design, not a safety-approved kit, and not a recommendation for inexperienced users to modify a mains appliance.
