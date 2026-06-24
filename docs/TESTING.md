# Testing checklist

Use this checklist before closing the machine or pulling real shots.

## Before powering the machine

- [ ] Espresso machine is unplugged from mains.
- [ ] Arduino firmware has been uploaded while the machine was unplugged.
- [ ] USB cable is disconnected from the Arduino.
- [ ] All solder joints are insulated.
- [ ] Arduino is mounted securely and insulated.
- [ ] Wires are away from the thermoblock and mains wiring.
- [ ] Pressure sensor wiring has been checked.
- [ ] `MOTO_IN` and `MOTO_OUT` are not swapped.
- [ ] Ground is common between Arduino and the machine low-voltage side.
- [ ] Hydraulic fittings are tight and correctly sealed.

## First electrical test

Start with `ENABLE_PRESSURE_CONTROL = false` if you want to verify clean pass-through behavior first.

Expected behavior:

- machine buttons still work;
- pump starts and stops normally;
- preinfusion is not broken;
- steam mode does not trigger unexpected behavior;
- no error appears from the original machine controller.

## First pressure-control test

After pass-through works:

- [ ] Set `ENABLE_PRESSURE_CONTROL = true`.
- [ ] Use a blind basket or pressurized basket.
- [ ] Watch the original gauge.
- [ ] Check for leaks while pressure rises.
- [ ] Stop immediately if pressure behaves unexpectedly.

## Normal extraction test

- [ ] Start with a normal grind, not an intentionally choked shot.
- [ ] Watch pressure rise.
- [ ] Confirm pressure does not overshoot excessively.
- [ ] Check that output flow remains reasonable.
- [ ] Inspect for leaks after the shot.

## After testing

- [ ] Unplug the machine.
- [ ] Let it cool down.
- [ ] Recheck fittings and wiring.
- [ ] Confirm the Arduino has not moved.
- [ ] Confirm no condensation or water is near electronics.
