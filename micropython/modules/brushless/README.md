# Brushless Motors <!-- omit in toc -->

The `brushless` module contains `BrushlessCluster`, for driving three-phase brushless motors
from a PIO state machine, with centre-aligned pulses that stay concentric as their duties
change. All commutation stays with the caller: a duty is the PWM level of one motor leg,
0.0 to 1.0, with 0.5 as the neutral point.

- [BrushlessCluster](#brushlesscluster)
  - [Constructor](#constructor)
  - [Duties](#duties)
  - [Phase and Frequency](#phase-and-frequency)

## BrushlessCluster

### Constructor

```python
BrushlessCluster(pio, sm, pins, direction=NORMAL_DIR, freq=25000, auto_phase=False)
```

`pins` selects one of two forms, as integers or `machine.Pin` objects:

- A list of 3-tuples, one signal pin per phase: `[(u1, v1, w1), (u2, v2, w2)]`. The gate
  driver generates each low side and owns its deadtime. A single bare 3-tuple is also
  accepted as a one-motor cluster.
- A list of 3-tuples of 2-tuples, three `(signal, inverse)` bridge pairs per motor:
  `[((1, 0), (3, 2), (5, 4))]`, for drivers whose paired inputs cannot act as independent
  half bridges. The inverse is an exact logical complement with no deadtime inserted.

`NORMAL_DIR` and `REVERSED_DIR` come from the shared `pimoroni` module; reversing a motor
swaps its V and W phases.

### Duties

The three legs of a motor are written together, which is what a commutation loop wants:

```python
cluster.duties(motor(s), u, v, w, load=True)
cluster.all_to_duties(u, v, w, load=True)
cluster.u_duty(motor)   # also v_duty(), w_duty()
```

`disable(motor(s))` drives all three legs low, which is a low-side short, not a coast;
coasting needs the driver's own enable pins. The stored duties survive a disable and
reapply on `enable(motor(s))`.

Passing `load=False` defers the hardware update until `load()` is called, batching
several changes into one transition list rebuild.

### Phase and Frequency

```python
cluster.phase(motor)                       # pulse start offset, 0.0 to 1.0 of the period
cluster.phase(motor(s), phase, load=True)
cluster.all_to_phase(phase, load=True)
cluster.frequency()                        # getter, and setter taking 10Hz to 200KHz
```

A motor's three legs share one offset, keeping their centred pulses concentric; offsetting
motors relative to each other spreads their edges to cut peak current draw, which
`auto_phase=True` applies automatically at construction.
