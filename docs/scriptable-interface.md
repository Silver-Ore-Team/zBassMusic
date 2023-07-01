# Scriptable interface (externals)
## Reference

If you use Visual Studio Code with Daedalus extension from Kirides, you can copy `_externals` directory to your project root to register them for autocomplete.

```cpp

// ---------------------------------------------------------------
// var string BassMusic_ActiveThemeFilename
//	Holds the filename of currently played song or the last song if none is playing now.
// ---------------------------------------------------------------
var string BassMusic_ActiveThemeFilename;

// ---------------------------------------------------------------
// var string BassMusic_ActiveThemeID
//	Holds the instance name/id of currently played song or the last song if none is playing now.
// ---------------------------------------------------------------
var string BassMusic_ActiveThemeID;

// ---------------------------------------------------------------
// var string BassMusic_EventThemeFilename
//	Holds the filename of theme in current event
// ---------------------------------------------------------------
var string BassMusic_EventThemeFilename;

// ---------------------------------------------------------------
// var string BassMusic_EventhemeID
//	Holds the instance name/id of theme in current eventr
// ---------------------------------------------------------------
var string BassMusic_EventThemeID;

// ---------------------------------------------------------------
// BassMusic_Play: Play a CMusicTheme by script name.
// @param var string id: Name of CMusicTheme instance.
// ---------------------------------------------------------------
func void BassMusic_Play(var string id) {};

// ---------------------------------------------------------------
// BassMusic_OnEndEvent: Add event listener for the end/loop of music theme.
// @param var func callback: Function with signature func void CB()
// ---------------------------------------------------------------
func void BassMusic_OnEndEvent(var func callback) {};

// ---------------------------------------------------------------
// BassMusic_OnTransitionEvent: Add event listener for the moment when track is finishing and transition should start.
// @param var func callback: Function with signature func void CB(var int time_left_ms)
// ---------------------------------------------------------------
func void BassMusic_OnTransitionEvent(var func callback) {};

// ---------------------------------------------------------------
// BassMusic_OnChangeEvent: Add event listener for the change of a theme.
// @param var func callback: Function with signature func void CB()
// ---------------------------------------------------------------
func void BassMusic_OnChangeEvent(var func callback) {};

// ---------------------------------------------------------------
// BassMusic_SetFullScriptControl: Sets full script control state. When true, the engine won't change music themes.
// @param var int active: Boolean if full script control should be activated.
// ---------------------------------------------------------------
func void BassMusic_SetFullScriptControl(var int active) {};

// ---------------------------------------------------------------
// BassMusic_Opt_TransitionTime: Set TransitionTime option value.
// @param var float time: Transition time in miliseconds
// ---------------------------------------------------------------
func void BassMusic_Opt_TransitionTime(var float time) {};

// ---------------------------------------------------------------
// BassMusic_Opt_ForceDisableReverb: Set ForceDisableReverb option value.
// @param var int enabled: Boolean value
// ---------------------------------------------------------------
func void BassMusic_Opt_ForceDisableReverb(var int enabled) {};

// ---------------------------------------------------------------
// BassMusic_Opt_ForceFadeTransition: Set ForceFadeTransition option value.
// @param var int enabled: Boolean value
// ---------------------------------------------------------------
func void BassMusic_Opt_ForceFadeTransition(var int enabled) {};
```

## Full Script Control

It's possible to disable automatic scheduling of music by the engine by setting:
```cpp
BassMusic_SetFullScriptControl(true);
```

After that the changes of music zones or time will not interfere with controlling the playback from scripts. To create custom scheduling logic, you can use event subscribers:
```cpp
func void MyMusicEndSubscriber()
{
	if (STR_Compare(MEM_BassMusic_EventThemeID, "START_THEME") == 0)
	{
		BassMusic_Play("NEXT_THEME_1");
	};
	
	if (STR_Compare(MEM_BassMusic_EventThemeID, "NEXT_THEME_1") == 0)
	{
		BassMusic_Play("NEXT_THEME_2");
	};
	
	if (STR_Compare(MEM_BassMusic_EventThemeID, "NEXT_THEME_2") == 0)
	{
		BassMusic_Play("START_THEME");
	};
};

BassMusic_OnEndEvent(MyMusicEndSubscriber);
BassMusic_Play("START_THEME");
```

Each music theme needs to be defined as `C_MUSICTHEME` instance like:
```cpp
prototype Base_Theme(C_MUSICTHEME)
{
	file = "";
	transtype = 7;		// TRANSITION_TYPE_ENDANDINTRO
	transsubtype = 7;	// TRANSITION_TYPE_ENDANDINTRO
	reverbmix = -8;
	reverbtime = 9000;
	vol = 1;
	loop = 1;
};

instance START_THEME(Base_Theme)
{
	file = "Start_Theme.wav";
};

instance NEXT_THEME_1(Base_Theme)
{
	file = "Next_Theme_1.wav";
};

instance NEXT_THEME_2(Base_Theme)
{
	file = "Next_Theme_2.wav";
};
```