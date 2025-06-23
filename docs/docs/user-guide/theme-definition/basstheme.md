---
hide:
  - toc
---

# zBassMusic Theme

zBassMusic defines custom classes for music themes to support additional information. In order to use it, place
following code in your music scripts, for example in `System/Music/MusicInst.d`.

```dae
const int BASSMUSIC_THEME_TYPE_NORMAL = 0;

class C_BassMusic_Theme {
	var string name;
	var string zones;
	var int type;
};

class C_BassMusic_ThemeAudio {
	var string theme;
	var string type;
	var string filename;
	var string midifile;
	var float volume;
	var int loop;
	var float loopStart;
	var float loopEnd;
	var int reverb;
	var float reverbMix;
	var float reverbTime;
	var int fadeIn;
	var int fadeInDuration;
	var int fadeOut;
	var int fadeOutDuration;
};

prototype BassMusic_Theme(C_BassMusic_Theme) {
    type = BASSMUSIC_THEME_TYPE_NORMAL;
};

prototype BassMusic_ThemeAudio(C_BassMusic_ThemeAudio) {
    type = "DEFAULT";
    volume = 1;
    loop = 1;
    reverb = 0;
    fadeIn = 1;
    fadeInDuration = 2000;
    fadeOut = 1;
    fadeOutDuration = 2000;
};
```

To define a music theme you can create an instance of `BassMusic_Theme` prototype and also a default audio file
using `BassMusic_ThemeAudio` prototype.

```dae
instance MyCustomTheme(BassMusic_Theme)
{
	name = "MyCustomTheme";
	zones = "SYS_MENU,OC_DAY_STD";
};

instance MyCustomTheme_Audio(BassMusic_ThemeAudio)
{
	theme = "MyCustomTheme";
	filename = "SYS_MENU.mp3";
};
```

Theme defines zones to play in as a comma-separated list in `zones` field. 
We treat both the zone (e.g. OC_DAY) and variant (e.g. STD) as a single entity.
Each theme variant would use a separate instance.

The audio `theme` field must be the same as theme's `name` to assign it for this theme. 

The audio instance must  use `DEFAULT` as a `type`, in order to play. 
In the future, we may introduce features that use multiple files per theme, hence this separation.

The `filename` field contains the name of the file that should be played in the custom theme. 
Note that when using Union, only the filename should be provided, regardless of whether it is located in subdirectories 
of the Music folder.

When `loop` is enabled, additionally `loopStart` and `loopEnd` can be specified to define the loop region in seconds.