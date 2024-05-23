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
```

## Fields

| Field          | Type   | Description                         |
|----------------|--------|-------------------------------------|
| `file`         | string | The filename of the music theme.    |
| `vol`          | float  | The volume of the music theme.      |
| `loop`         | int    | If true, music will loop after nd   |
| `reverbmix`    | float  | The reverb mix of the music theme.  |
| `reverbtime`   | float  | The reverb time of the music theme. |
| `transtype`    | int    | The type of the transition.         |
| `transsubtype` | int    | The subtype of the transition.      |