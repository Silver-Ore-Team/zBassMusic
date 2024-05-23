# Scripting (Daedalus)

The plugin defines several external functions that let us interact with the Music Engine from Daedalus scripts.
The full list of available functions can be found in the [Reference](../reference/index.md) page, and here we 
will cover only the "Full Script Control" mode.

## Full Script Control

You can enable the Full Script Control mode in scripts to disable automatic music scheduling based on music zones
and play your own music themes without any interference from the standard mechanism.

```dae
BassMusic_SetFullScriptControl(true);
```

When the Full Script Control mode is enabled, the plugin will not play any music themes automatically and you 
have to do it yourself using the `BassMusic_Play` function.

```dae
BassMusic_Play("MyTheme");
```


When you are done with the custom music playback, you can disable the Full Script Control mode and get the
original schedulers back.

```dae
BassMusic_SetFullScriptControl(false);
```

## Global variables

You can access currently playing music theme using global variables.

```dae
var string BassMusic_ActiveThemeFilename;   // eg. OLD_CAMP.mp3
var string BassMusic_ActiveThemeID;         // eg. OLD_CAMP
```

## Events

You can handle some music events in the scripts by registering a callback function.

### OnEnd

```dae
func void OnEnd() {
    // Do something when the music ends
};

BassMusic_OnEndEvent(OnEnd);
```

### OnTransition

```dae
func void OnTransition(var int time_left_ms) {
    // Do something when the transition is about to happen
    // time_left_ms is the time left to the end of the current
};

BassMusic_OnTransitionEvent(OnTransition);
```

### OnChange

```dae
func void OnChange() {
    // Do something when the music theme changes
};

BassMusic_OnChangeEvent(OnChange);
```

## Options

Some .ini options can be set from the scripts. It may be useful if you are taking full control over the music playback.

```dae
func void BassMusic_Opt_TransitionTime(var float time) {};
func void BassMusic_Opt_ForceDisableReverb(var int enabled) {};
func void BassMusic_Opt_ForceFadeTransition(var int enabled) {};
```

