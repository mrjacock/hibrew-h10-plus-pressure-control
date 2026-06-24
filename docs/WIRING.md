# Wiring

## Connector type

The reversible harness uses JST XH 2.54 mm 10-pin connectors.

## Signal path

The `MOTO` line from the original MCU board is intercepted before it reaches the machine control/power board.

```text
Original MCU board MOTO pin
        ↓
Arduino D2 input
        ↓
Arduino pressure-control logic
        ↓
Arduino D5 output
        ↓
Original TRIAC pump power board
        ↓
ULKA vibration pump
```

The TRIAC power board drives the pump. The pump does not drive the TRIAC.

## Used Arduino pins

| Signal | Arduino pin | Direction | Notes |
|---|---:|---|---|
| Original `MOTO` | `D2` | input | Command from original MCU board |
| Modified `MOTO` | `D5` | output | Command sent to original TRIAC/pump power board |
| `ZERO` | `D3` | input / available | Available for reference; not required by the current main logic |
| Pressure sensor | `A4` | analog input | 0.5-4.5 V sensor signal |
| 5 V | `5V` | supply | Arduino and pressure sensor supply |
| Ground | `GND` | reference | Common low-voltage reference |

![Electrical wiring layout](../images/electrical-wiring-layout.png)

## Critical USB rule

Never connect the Arduino USB cable to a computer while the espresso machine is powered from mains.

Read [Safety](safety.md) before wiring or debugging.
