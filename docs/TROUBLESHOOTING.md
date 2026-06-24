# Troubleshooting

## Pump does not run

Check the `MOTO` signal path:

```text
Original MCU board -> Arduino D2 -> Arduino D5 -> original TRIAC pump power board -> pump
```

The TRIAC board drives the pump. If the Arduino blocks or fails to reproduce the `MOTO` command, the pump will not receive a valid command.

## Pressure does not rise

Possible causes:

- grinder too coarse or no hydraulic restriction;
- pressure sensor branch leaking;
- pump command being blocked too much;
- wrong target or pressure calibration;
- MOTO input/output wiring swapped;
- machine not actually requesting pump operation.

## Pressure overshoots

Possible causes:

- target window too narrow;
- smoothing too slow;
- minimum burst too aggressive;
- pressure sensor reading delayed or noisy;
- pump command not being blocked when target pressure is reached.

## Serial output is missing

That is expected. The firmware should not rely on live `Serial` debugging while the machine is powered, because connecting USB to a PC in that state is unsafe.

Unplug the machine before reconnecting USB.
