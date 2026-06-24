# Software

Firmware path:

```text
firmware/hibrew_h10_pressure_control/hibrew_h10_pressure_control.ino
```

## Arduino IDE settings

Use:

| Setting | Value |
|---|---|
| Board | Arduino Nano |
| Processor | ATmega328P |
| Logic level | 5 V |
| Bootloader | Try `ATmega328P (Old Bootloader)` if upload fails on a clone |

## Upload safety

Upload only when the espresso machine is unplugged from mains.

Do not connect USB to the Arduino while the machine is powered from mains.

## Main firmware behavior

The firmware uses:

- `INT0` on `D2` to react to the original `MOTO` signal;
- direct AVR port access for fast `D5` updates;
- `Timer1` as an idle timeout to release `D5` if original pump pulses stop;
- `analogRead(A4)` for pressure sensor input;
- an exponential moving average filter for pressure smoothing;
- pulse-density modulation to pass or skip pump trigger pulses.

## Parameters

| Parameter | Default | Meaning |
|---|---:|---|
| `ENABLE_PRESSURE_CONTROL` | `true` | Set to `false` for pure D2 -> D5 pass-through |
| `ADC_0_BAR` | `102` | Expected ADC value at 0 bar |
| `ADC_12_BAR` | `921` | Expected ADC value at 12 bar |
| `TARGET_CBAR` | `900` | Target pressure: 9.00 bar |
| `FULL_COPY_UNTIL_CBAR` | `850` | Full pass-through below 8.50 bar |
| `HARD_CUT_CBAR` | `1050` | Aggressive cut above 10.50 bar |
| `HOLD_DUTY_PERMILLE` | `720` | Approximate duty near target |
| `KP_PER_CBAR` | `4` | Simplified proportional response |
| `EMA_SHIFT` | `4` | Pressure filter strength |
| `MIN_BURST_PULSES` | `2` | Minimum consecutive passed pulses |
| `MAX_BLOCKED_PULSES` | `150` | Maximum blocked pulse streak before a safety burst |
| `SAFETY_BURST_PULSES` | `2` | Pulses passed during safety burst |
| `SAFETY_BURST_MAX_CBAR` | `1000` | Safety burst only below this pressure |

## Pressure units

The firmware uses centibar:

```text
900 cbar = 9.00 bar
1050 cbar = 10.50 bar
1200 cbar = 12.00 bar
```

## Fail-open behavior

If the sensor reading is outside the expected range, the firmware sets duty to full pass-through. This is intentional: a broken or disconnected pressure signal should not cause the firmware to silently block the pump command.

## Why Serial is not used

The working firmware is designed for timing stability and does not use Serial debugging. It also disables Timer0, which means normal Arduino timing helpers such as `millis()` and `delay()` are not available.

For live debugging, use isolated equipment and do not connect the USB port while the machine is powered from mains.
