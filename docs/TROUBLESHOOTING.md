# Troubleshooting

## The pump does not run

Possible causes:

- `MOTO_IN` and `MOTO_OUT` are swapped;
- `D2` or `D5` is wired incorrectly;
- Arduino is not powered;
- machine 5 V and Arduino GND are not common;
- firmware was not uploaded correctly;
- the original `MOTO` path was interrupted but not routed through Arduino;
- `D5` connection to the power board is open.

## The machine reports an error

Possible causes:

- too many pump pulses are being skipped;
- `MAX_BLOCKED_PULSES` is too high;
- `MIN_BURST_PULSES` is too low;
- pressure reading is wrong;
- control response is too aggressive;
- the original controller detects unexpected pump behavior.

## Pressure overshoots too much

Try:

- lowering `FULL_COPY_UNTIL_CBAR` slightly;
- lowering `HOLD_DUTY_PERMILLE`;
- increasing `KP_PER_CBAR` carefully;
- checking for sensor delay caused by trapped air;
- checking that the pressure tap point is not too damped or isolated.

## Pressure is unstable

Vibration pumps create real pressure pulsation. Some movement is expected.

Try:

- increasing `EMA_SHIFT` from `4` to `5`;
- removing trapped air from the sensor branch;
- checking for leaks;
- checking that the fitting does not create a blocked dead-end;
- using a less aggressive `KP_PER_CBAR`.

## Pressure never reaches the target

Possible causes:

- grind is too coarse;
- basket/puck offers too little resistance;
- hydraulic leak;
- pump not receiving enough pulses;
- `HOLD_DUTY_PERMILLE` too low;
- `FULL_COPY_UNTIL_CBAR` too low;
- sensor reading too high due to wrong calibration.

## Arduino upload fails

Try:

- select `Arduino Nano`;
- try `ATmega328P` and then `ATmega328P (Old Bootloader)`;
- select the correct serial port;
- use a data-capable USB cable;
- upload the Blink example first;
- unplug the espresso machine from mains before connecting USB.

## Arduino or PC gets damaged

The known dangerous condition is:

```text
Espresso machine powered from mains + Arduino connected by USB to a PC
```

Avoid this condition completely.
