# OnBeat Transition

!!! note "Preview"
This feature is still under development and may change significantly in the future.

OnBeat transition is a rule that schedules the transition for the closest point in the future from the list of
allowed points.
In this Release, the OnBeat rule can be activated only by using a Daedalus external and matches the input theme
to any output.
In the future, we plan to allow providing time points using a MIDI file synchronized with the music
and scope the rule to specific (input, output) pairs.

## Daedalus API

### Activate on an interval

The second argument accepts a string formatted as a double that defines the interval of the beat in seconds.
Transition can happen at any multiplier of the interval.

```dae
// Will activate on [2.66, 5.32, 7.98, ...]
BassMusic_TransitionRule_OnBeat("Input_Theme", "2.66", "");
```

### Activate on a specific time points

The third argument accepts a string formatted as a semicolon-separated list of doubles that defines the exact time
points in seconds.
Transition can happen only at the specified time points.

```dae
// Will activate on [1.2, 2.4, 7.33, 12.97]
BassMusic_TransitionRule_OnBeat("Input_Theme", "0", "1.2;2.4;7.33;12.97");
```

You can also mix both arguments to create a rule with constant interval + some additional points.
The scheduler will always choose the closest point in the future.