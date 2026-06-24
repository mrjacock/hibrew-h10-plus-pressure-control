# Wiring

![Wiring diagram](../images/wiring-diagram.svg)

## Arduino pinout

| Signal | Arduino Nano | Direction | Description |
|---|---:|---|---|
| `MOTO_IN` | `D2` | Input | Original MOTO signal from the machine controller |
| `MOTO_OUT` | `D5` | Output | Modified MOTO signal to the pump power board |
| `ZERO` | `D3` | Input | Zero-cross signal, connected but not used by v0.2 firmware |
| `PRESSURE` | `A4` | Input | Pressure sensor analog output |
| `5V` | `5V` | Power | 5 V rail from the machine |
| `GND` | `GND` | Ground | Common low-voltage ground |

## Observed machine connector labels

On the tested unit, the connector was labeled as follows:

```text
GND | VCC 5V | KG | ZERO | WTEST | MOTO | RT | HEAT | DCF
```

Verify this on your own machine before wiring anything. Do not rely on wire color alone.

## MOTO interception

The original `MOTO` path must be interrupted and routed through the Arduino:

```text
Machine controller side  -> Arduino D2
Arduino D5               -> Pump power board side
```

This lets the firmware either copy the original pump command or skip selected pulses.

## Pressure sensor wiring

Typical 3-wire pressure sensor:

```text
Sensor +5 V     -> Arduino 5V / machine 5 V rail
Sensor GND      -> Arduino GND / machine GND
Sensor signal   -> Arduino A4
```

Confirm your sensor pinout before powering it. Many sensors have similar connectors but different wire colors.

## Reversible harness recommendation

Use a male/female JST XH 2.5 mm 10-pin harness so the original machine wiring does not need to be permanently cut.

Recommended concept:

```text
Original machine harness -> adapter harness -> original board connector
                              |
                              +-> Arduino and pressure sensor taps
```

## Routing and insulation

- Keep wires short and tidy.
- Keep the Arduino away from the thermoblock and hot metal parts.
- Keep low-voltage wiring away from mains wiring.
- Add strain relief.
- Cover every solder joint with heat shrink.
- Do not leave exposed copper inside the machine.

## Upload rule

Always upload firmware with the espresso machine unplugged from mains.

Never power the machine from mains while the Arduino is connected by USB to a computer.
