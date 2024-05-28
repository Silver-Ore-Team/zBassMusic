# C_MUSICTHEME

Original Gothic class for music is defined in `System/_Intern/Music.d` script. You can create its instances
to define music themes for zBassMusic.

```dae
class C_MUSICTHEME
{
	var string file;
	var float vol;
	var int loop;
	var float reverbmix;
	var float reverbtime;
	var int transtype;
	var int transsubtype;
};

prototype C_MUSICTHEME_DEF(C_MUSICTHEME)
{
	vol = 1;
	loop = 1;
	transtype = TRANSITION_TYPE_NONE;
	transsubtype = TRANSITION_SUB_TYPE_MEASURE;
	reverbmix = -80;
	reverbtime = 9000;
};
```

To define a music theme using this class, you create an instance in `System/Music/MusicInst.d` script.
The name of an instance must be the same as the music zone where it plays.

```dae
// OC_Day_Std = Old Camp, Day, Standard
instance OC_Day_Std(C_MUSICTHEME_DEF)
{
	file = "MyCustomMusic.mp3";
	transtype = TRANSITION_TYPE_ENDANDINTRO;
	reverbmix = -12;
	reverbtime = 3000;
	vol = 1;
	loop = 1;
};
```

## Limitations

The original class is not detailed enough to use all zBassMusic features. The limitations are:

* The instance name must match music zone + variant. Only one song per music zone + variant is possible.
* `reverbmix` and `reverbtime` parameters have a strict restriction on range. zBassMusic will clamp the values to [-96, 0] and [0.001, 3000], respectively.
* `transtype` and `transsubtype` are DirectMusic-specific transitions that are impossible to 100% emulate in zBassMusic.

If you would like to have more options, use [zBassMusic Theme](basstheme.md) classes instead.
