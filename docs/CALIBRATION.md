# Calibration

The pressure sensor used for this project is a 0-12 bar sensor with a nominal 0.5-4.5 V analog output and 5 V supply.

Typical mapping:

```text
0.5 V -> 0 bar
4.5 V -> 12 bar
```

This means the useful sensor span is 4.0 V over 12 bar.

Always verify the real sensor behavior against the analog gauge before trusting the firmware values.

Suggested first target:

```text
TARGET_BAR_X10 = 90   // 9.0 bar
```

Use conservative tests first. Do not chase perfect behavior before confirming that the wiring, sensor reading, and pump command path are correct.
