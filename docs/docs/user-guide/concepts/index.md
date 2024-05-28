# Concepts

## Music Theme

Music Theme is a single song in the game soundtrack. It contains Audio Files, Effects, Transitions and Zones.

### Audio File

An audio file with the song or a jingle in a specific format (MP3, WAVE, OPUS, FLAC). 
The audio files are placed in `_work/Data/Music/` directory and referenced by a Music Theme.

### Effects

Effects are the audio effects applied on a song during playback. Supported effects include:

* Volume - volume level of the song as a percent of master volume.
* Reverb DX8 - reverb effects based on DirectX 8 implementation
* Fade In - fade-in transition when the song starts
* Fade OUt - fade-out transition when the song ends

### Transitions

Rules defining how the to schedule the change of a song. Transition may define the time span when change
occurs and the effects used to switch thet theme.

### Zone

The area where a song will play. Music zone are created using Spacer and a song may be attached to it.

## VDFS

Virtual file system used by Gothic to load assets. zBassMusic supports it fully, and we may reference some directory as:

### Physical

Located on your computer hard drive in the Gothic installation directory

### Virtual

A *.vdf or *.mod file loaded by Gothic, they contain the same directory structure as physical, 
but all files are inside the file (like a ZIP archive).