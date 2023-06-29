# Transition effects
zBassMusic can transition between music themes using simple fade-out and fade-in effects over a timespan.

To use transitions, at least one must be true:
* **C_MUSICTHEME.transtype == zMUS_TR_INTRO (5)**: theme will use fade-in
* **C_MUSICTHEME.transtype == zMUS_TR_END (6)**: theme will use fade-out
* **C_MUSICTHEME.transtype == zMUS_TR_ENDANDINTRO (7)**: theme will use both fade-in and fade-out
* **BASSMUSIC.ForceFadeTransition** ini is set true, all themes will use both fade-in and fade-out

Transition time can be set by **BASSMUSIC.TransitionTime** ini option.

Scripts can control transition time dynamically using:
```cpp
BassMuisc_Opt_TransitionTime(3000.0); // 3 seconds
```