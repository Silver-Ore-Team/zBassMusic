# Transitions Scheduler

!!! note "Preview"
    This feature is still under development and may change significantly in the future.

Transition Scheduler is a module providing advanced transitions between music themes that need to be precisely
synchronized and accord to the defined rules.
For example, OnBeat rule limits the time points when the transition can occur and schedules the transition for 
the closest point in the future, so the composer can design seamless transitions with a perfect match to the beat. 

Available transition rules:

* [Instant](instant.md) - immediate transition [default]
* [OnBeat](on-beat.md) - transition only on specific time points
* [Jingle](jingle.md) - transition with another sound inbetween