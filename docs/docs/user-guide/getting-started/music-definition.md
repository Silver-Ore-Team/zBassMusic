# Music Definition

zBassMusic uses the same music definitions as the original game, so you can work directly on existing `C_MUSICTHEME`
instances.
The instances are in `Scripts/System/Music/MusicInst.d`.

```dae
prototype C_MUSICTHEME_FIGHT(C_MUSICTHEME)
{
    // With zBassMusic you can load mp3 and other music formats
	file = "My_Super_Music.mp3"; 

    // Use TRANSITION_TYPE_ENDANDINTRO to enable crossfade between themes
	transtype = TRANSITION_TYPE_ENDANDINTRO;
	transsubtype = TRANSITION_SUB_TYPE_MEASURE;

    // reverbmix and reverbtime might have to be adjusted
	reverbmix = -12;
	reverbtime = 9000;
	vol = 1;
	loop = 1;
};
```

## Caveat: Reverb Effect

zBassMusic emulates the original reverb effects using `BASS_DX8_REVERB` from BASS library but it has strict limits on
the
acceptable parameters range. If you would like to use the effect, make sure that:

* `reverbmix` is in range `[-96, 0]`
* `reverbtime` is in range `[0.001, 3000]`

If the parameter value is outside these ranges, the effect won't be applied.