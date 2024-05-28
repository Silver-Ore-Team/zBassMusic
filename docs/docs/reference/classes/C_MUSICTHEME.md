# C_MUSICTHEME

## Description

The class represents a music theme that can be played by the plugin.

## Definition

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

## Fields

| Field        | Type       | Description                                                                         |
|--------------|------------|-------------------------------------------------------------------------------------|
| file         | string     | The filename of an audio file.                                                      |
| vol          | float      | Volume of the song in range [0, 1] as the % of master volume.                       |
| loop         | int (bool) | If not zero, the theme will loop. Otherwise, it will play only once per zone enter. |
| reverbmix    | float      | Mix property for Reverb DX8 effect in range [-96, 0] dB.                            |
| reverbtime   | float      | Time property for Reverb DX8 effect in range [0.001, 3000] ms.                      |
| transtype    | int        | Transition type. Check the table below.                                             |
| transsubtype | int        | Ignored.                                                                            |

| transtype                   | Value | Transition                                                                                                 |
|-----------------------------|-------|------------------------------------------------------------------------------------------------------------|
| TRANSITION_TYPE_NONE        | 1     | Ignored.                                                                                                   |
| TRANSITION_TYPE_GROOVE      | 2     | Ignored.                                                                                                   |
| TRANSITION_TYPE_FILL        | 3     | Ignored.                                                                                                   |
| TRANSITION_TYPE_BREAK       | 4     | Ignored.                                                                                                   |
| TRANSITION_TYPE_INTRO       | 5     | Enables fade-in transition with the duration form [BassMusic].TransitionTime ini option.                   |
| TRANSITION_TYPE_END         | 6     | Enables fade-out transition with the duration form [BassMusic].TransitionTime ini option.                  |
| TRANSITION_TYPE_ENDANDINTRO | 7     | Enables both fade-in and fade-out transition with the duration form [BassMusic].TransitionTime ini option. |