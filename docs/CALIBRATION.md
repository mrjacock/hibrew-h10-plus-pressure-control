# Calibration

The default firmware assumes a 0-12 bar pressure sensor with 0.5-4.5 V analog output powered from the same 5 V rail used by the Arduino ADC reference.

## Default mapping

| Pressure | Sensor voltage | Approx. ADC value |
|---:|---:|---:|
| 0 bar | 0.5 V | 102 |
| 9 bar | 3.5 V | 716 |
| 12 bar | 4.5 V | 921 |

Firmware constants:

```cpp
const int ADC_0_BAR  = 102;
const int ADC_12_BAR = 921;
const int SENSOR_MAX_CBAR = 1200;
```

## When to calibrate

Calibrate or verify the mapping if:

- your sensor has a different pressure range;
- your sensor output is not 0.5-4.5 V;
- your Arduino 5 V rail is not close to the sensor supply;
- the analog gauge and digital reading disagree significantly;
- pressure control cuts too early or too late.

## Basic calibration approach

1. Confirm the sensor datasheet or listing.
2. Confirm sensor wiring.
3. With the machine unplugged from mains, upload a safe test sketch if needed.
4. Power the sensor only from a safe 5 V source during bench calibration.
5. Measure sensor output at rest.
6. Compare the digital reading against a known pressure reference if available.
7. Update `ADC_0_BAR` and `ADC_12_BAR` only after confirming the sensor behavior.

## Do not calibrate through USB on a powered machine

Do not connect the Arduino USB cable to a PC while the espresso machine is powered from mains. Use isolated tools or separate bench testing for sensor calibration.

## Pressure filter tuning

`EMA_SHIFT` controls smoothing:

| Value | Behavior |
|---:|---|
| `3` | Faster response, more visible pump pulsation |
| `4` | Default balance |
| `5` | Smoother response, slower reaction |

A vibration pump produces real pressure pulsations. Do not chase every pulse with the control loop; the goal is stable average extraction pressure.
