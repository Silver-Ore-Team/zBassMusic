# Transitions

zBassMusic implements a scheduler to do advanced transition between music themes.
This allows a composer or developer to set up seamless transitions for a greater artistic value of a soundtrack.

## Settings

### Default

If no transition was explicitly defined, the default strategy implements an immediate cross-fade, taking into
account the fade-in and fade-out effect settings of the themes.

### Timing

Timing settings let you limit on which moments a transition may happen and what time spans to use for effects.
Check [Timing](timing.md) for more information.

### Jingle

Jingle plays an additional audio file in-between transitioning songs. You can use it, for example to transition
from fight music to standard music with a nice finish sound. Check [Jingle](jingle.md) for more information.

### MIDI Files

Some time-dependent parameters of a transition can be set up in a special MIDI track to make it easier for an artist to design.
Check [MIDI Files](midi.md) for more information.

## Filter

A transition is assigned to Music Theme with a many-to-one relation. 
Every function to add a transition has a `filter` parameter that defines for which target themes this transition applies.

To set up default transition, use an empty string ("") for the filer. Default filter matches every transition which
wasn't explicitly matched by another.  

If `filter` is not an empty string, it contains a regular expression for the target theme to match.

* `#!regex TargetThemeA` - matches every string that includes `TargetThemeA`
* `#!regex ^TargetThemeA$` - matches string that equals exactly `TargetThemeA`
* `#!regex TargetThemeA|TargetThemeB|TargetThemeC` - matches every string that includes one of `TargetThemeA`, `TargetThemeB`, `TargetThemeC`
* `#!regex ^(TargetThemeA|TargetThemeB|TargetThemeC)$` - matches every string that equals exactly one of `TargetThemeA`, `TargetThemeB`, `TargetThemeC`
* `#!regex ^Target` - matches every string that starts with `Target`
* `#!regex Fight$` - matches every string that ends with `Fight`

Filters are tested in no guaranteed order and the first match wins. 
If you are using fuzzy matchers, make sure to avoid overlaps or your transitions will be non-deterministic.
