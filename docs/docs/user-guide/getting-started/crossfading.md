# Crossfading

zBassMusic implements basic fade-in and fade-out effects to crossfade between songs but it's activated only 
if the music theme defines a correct `transtype`. We support:

* `TRANSITION_TYPE_INTRO` (= 5) - fade-in effect at the start
* `TRANSITION_TYPE_END` (= 6) - fade-out effect at the end
* `TRANSITION_TYPE_ENDANDINTRO` (= 7) - both fade-in and fade-out effects

The best default is to use `TRANSITION_TYPE_ENDANDINTRO` because it defines both start and end effects. If you would have one song with `TRANSITION_TYPE_END` and another song without `TRANSITION_TYPE_INTRO`, then during the transition the first sound would fade out but the second one would start minutely at 100% volume.

```dae
prototype C_MUSICTHEME_FIGHT(C_MUSICTHEME)
{
	file = "My_Super_Music.mp3"; 

    // Use TRANSITION_TYPE_ENDANDINTRO to enable crossfade between themes
	transtype = TRANSITION_TYPE_ENDANDINTRO;
	transsubtype = TRANSITION_SUB_TYPE_MEASURE;
	reverbmix = -12;
	reverbtime = 9000;
	vol = 1;
	loop = 1;
};
```

Other values of `transtype` and all values of `transsubtype` are ignored because zBassMusic can't emulate them on arbitrary audio files with no MIDI metadata. The advanced transitions are implemented by the plugin in [Transition Scheduler](../transition-scheduler/index.md).