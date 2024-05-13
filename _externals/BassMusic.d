
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

// ---------------------------------------------------------------
// BassMusic_TransitionRule_OnBeat: Creates an OnBeat transition rule for the given music file name.
// @param var string name: Name of the music file including extensions (eg. OLD_CAMP.mp3)
// @param var string interval: Duration of single beat repeated throughout the playback to schedule transition
//                                (in seconds, eq. "7.43")
// @param var string timeCodes: Semicolon-separated time codes at which music can transition
//                                (in seconds, eq. "1.2;4.8;7.3;9.443")
// ---------------------------------------------------------------
func void BassMusic_TransitionRule_OnBeat(var string name, var string interval, var string timeCodes) {};