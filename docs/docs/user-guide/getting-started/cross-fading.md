# Cross-fading

zBassMusic implements basic fade-in and fade-out effects to crossfade between songs, but it's activated only
if the music theme defines a correct `transtype`. We support:

* `TRANSITION_TYPE_INTRO` (= 5) - fade-in effect at the start
* `TRANSITION_TYPE_END` (= 6) - fade-out effect at the end
* `TRANSITION_TYPE_ENDANDINTRO` (= 7) - both fade-in and fade-out effects

The best default is to use `TRANSITION_TYPE_ENDANDINTRO` because it defines both start and end effects.
If you had one song with `TRANSITION_TYPE_END` and another song without `TRANSITION_TYPE_INTRO`,
then during the transition the first sound would fade out but the second one would start instantly at 100% volume.

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

Other values of `transtype` and all values of `transsubtype` are ignored because zBassMusic can't emulate them on
arbitrary audio files with no MIDI metadata.

Instead of trying to emulate DirectMusic effects, we implement our own transition scheduler to perform advanced transitions. 
To learn more you can check next:

* [zBassMusic Theme Class](../theme-definition/basstheme.md) - an extended version of `C_MUSICTHEME` class made specifically for zBassMusic.
    You can use it instead of the Gothic's one to enable additional features.
* [Transitions](../transitions/index.md) - the new transition system that gives you a full control over how music changes.
    You can set up custom timing, add an in-between jingle sound or use a MIDI file to provide metadata for the engine.
    With a well-prepared transitions, your music will hit better. Like, literally - it will hit on the beat.
* [Scripting](../scripting/index.md) - external functions to control the music playback from Daedalus scripts. You can use it
    to implement a special theme flow for specific moments in the story.  