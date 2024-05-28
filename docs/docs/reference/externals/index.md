# Externals

<div class="zbassmusic-externals" markdown>

## <a href="#bassmusic_play">BassMusic_Play</a>

Triggers an event to start a new music theme.

```dae
func void BassMusic_Play(var string id)
```

**Parameters**

* `#!dae var string id`<br>
  Symbol name of the music theme ([C_MUSICTHEME](../classes/C_MUSICTHEME.md)) to play

## <a href="#bassmusic_setfullscriptcontrol">BassMusic_SetFullScriptControl</a>

Set the "Full Script Control"

```dae
func void BassMusic_SetFullScriptControl(int enabled) {}
```

**Parameters**

* `#!dae var int enabled`<br>
  Boolean value if full script control should be enabled.

## <a href="#bassmusic_onendevent">BassMusic_OnEndEvent</a>

Set a callback function to be called when the music ends.

```dae
func void BassMusic_OnEndEvent(var func callback) {}
```

**Parameters**

* `#!dae var func callback`<br>
  Callback function to be called when the music ends with signature:<br>
  `#!dae func void callback();`

## <a href="#bassmusic_ontransitionevent">BassMusic_OnTransitionEvent</a>

Set a callback function to be called when the music is close to the end and transition starts.

```dae
func void BassMusic_OnTransitionEvent(var func callback) {}
```

**Parameters**

* `#!dae var func callback`<br>
  Callback function to be called when the music is close to the end and transition starts:<br>
  `#!dae func void callback(var float time_left_ms);`

## <a href="#bassmusic_onchangeevent">BassMusic_OnChangeEvent</a>

Set a callback function to be called when the music changes.

```dae
func void BassMusic_OnChangeEvent(var func callback) {}
```

**Parameters**

* `#!dae var func callback`<br>
  Callback function to be called when the music changes with signature:<br>
  `#!dae func void callback();`

## <a href="#bassmusic_opt_transitiontime">BassMusic_Opt_TransitionTime</a>

Set [Bass].TransitionTime ini setting.

```dae
func void BassMusic_Opt_TransitionTime(var float time) {}
```

**Parameters**

* `#!dae var float time`<br>
  The transition time in miliseconds

## <a href="#bassmusic_opt_forcedisablereverb">BassMusic_Opt_ForceDisableReverb</a>

Set [Bass].ForceDisableReverb ini setting.

```dae
func void BassMusic_Opt_ForceDisableReverb(var int enabled) {}
```

**Parameters**

* `#!dae var int enabled`<br>
  If true, the reverb effect is globally disabled

## <a href="#bassmusic_opt_forcefadetransition">BassMusic_Opt_ForceFadeTransition</a>

Set [Bass].ForceFadeTransition ini setting.

```dae
func void BassMusic_Opt_ForceFadeTransition(var int enabled) {}
```

**Parameters**

* `#!dae var int enabled`<br>
  If true, the fade transition is globally enabled

## <a href="#bassmusic_addmidifile">BassMusic_AddMidiFile</a>

Add a [MIDI File](../../user-guide/transitions/midi.md) to a theme.

```dae
func void BassMusic_AddMidiFile(var string theme, var string filter, var string midiFilename) {}
```

**Parameters**

* `#!dae var string theme`<br>
  Music theme name
* `#!dae var string filter`<br>
  [Transition filter](../../user-guide/transitions/index.md#filter)
* `#!dae var string midiFilename`<br>
  Filename of a MIDI file

## <a href="#bassmusic_addtransitiontimepoint">BassMusic_AddTransitionTimePoint</a>

Add a time point for [Timing Transition](../../user-guide/transitions/timing.md).

```dae
func void BassMusic_AddTransitionTimePoint(var string theme, var string filter, var float start, var float duration, var int effect, var float nextStart, var float nextDuration, var int nextEffect) {}
```

**Parameters**

* `#!dae var string theme`<br>
  Music theme name
* `#!dae var string filter`<br>
  [Transition filter](../../user-guide/transitions/index.md#filter)
* `#!dae var float start`<br>
  Transition start in seconds
* `#!dae var float duration`<br>
  Transition duration in seconds
* `#!dae var int effect`<br>
  Effect ID (NONE = 0, CROSSFADE = 0)
* `#!dae var float nextStart`<br>
  Transition start in seconds (next song)
* `#!dae var float nextDuration`<br>
  Transition duration in seconds (next song)
* `#!dae var int nextEffect`<br>
  Effect ID (NONE = 0, CROSSFADE = 0)

## <a href="#bassmusic_addjingle">BassMusic_AddJingle</a>

Add a [Jingle](../../user-guide/transitions/jingle.md) to a theme.

```dae
func void BassMusic_AddJingle(var string theme, var string filter, var string jingle, var float delay) {}
```

**Parameters**

* `#!dae var string theme`<br>
  Music theme name
* `#!dae var string filter`<br>
  [Transition filter](../../user-guide/transitions/index.md#filter)
* `#!dae var string jingle`<br>
  Filename of a jingle audio file
* `#!dae var float delay`
  Delay in seconds before the jingle starts
  
</div>