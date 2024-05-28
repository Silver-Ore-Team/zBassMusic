# C_BassMusic_ThemeAudio

## Description

The class represents an audio file assigned to a music theme.

## Definition

```dae
class C_BassMusic_ThemeAudio {
    var string theme;
    var string type;
    var string filename;
    var string midifile;
    var float volume;
    var int loop;
    var int reverb;
    var float reverbMix;
    var float reverbTime;
    var int fadeIn;
    var int fadeInDuration;
    var int fadeOut;
    var int fadeOutDuration;
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

## Fields

| Field           | Type       | Description                                                                         |
|-----------------|------------|-------------------------------------------------------------------------------------|
| theme           | string     | The name (identifier) of a theme.                                                   |
| type            | string     | Type of the audio. Use "DEFAULT" to play it as music.                               |
| filename        | string     | Filename of the audio file.                                                         |
| midifile        | string     | Filename of a MIDI file for transition control.                                     |
| vol             | float      | Volume of the song in range [0, 1] as the % of master volume.                       |
| loop            | int (bool) | If not zero, the theme will loop. Otherwise, it will play only once per zone enter. |
| reverb          | int (bool) | If not zero, the Reverb DX8 effect is enabled.                                      | 
| reverbmix       | float      | Mix property for Reverb DX8 effect in range [-96, 0] dB.                            |
| reverbtime      | float      | Time property for Reverb DX8 effect in range [0.001, 3000] ms.                      |
| fadeIn          | int (bool) | If not zero, the fade-in effect is enabled.                                         |
| fadeInDuration  | float      | Fade-in effect duration in milliseconds.                                            |
| fadeOut         | int (bool) | If not zero, the fade-out effect is enabled.                                        |
| fadeOutDuration | float      | Fade-out effect duration in milliseconds.                                           |