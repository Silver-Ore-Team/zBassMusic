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

## <a href="#bassmusic_opt_transitionrule_onbeat">BassMusic_TransitionRule_OnBeat</a>

Add a new OnBeat rule to the Transition Scheduler.

```dae
func void BassMusic_TransitionRule_OnBeat(var string name, var string interval, var string timeCodes) {}
```

**Parameters**

* `#!dae var string name`<br>
  Input music theme name
* `#!dae var string interval`<br>
  String formatted as double, the interval of a beat in seconds. Set "0" to disable.
* `#!dae var string timeCodes`<br>
  String formatted as a list of semicolon-separated doubles, the time codes in seconds when the transition can happen,
  eg. `"0.0;1.0;2.0;3.0"`

</div>