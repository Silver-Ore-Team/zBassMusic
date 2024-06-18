# zBassMusic

[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=zbassmusic&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=zbassmusic)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=zbassmusic&metric=bugs)](https://sonarcloud.io/summary/new_code?id=zbassmusic)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=zbassmusic&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=zbassmusic)
[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=zbassmusic&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=zbassmusic)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=zbassmusic&metric=reliability_rating)](https://sonarcloud.io/summary/new_code?id=zbassmusic)
[![Technical Debt](https://sonarcloud.io/api/project_badges/measure?project=zbassmusic&metric=sqale_index)](https://sonarcloud.io/summary/new_code?id=zbassmusic)

zBassMusic is a modern music system for Gothic games based on ZenGin.
It replaces the original DirectMusic system with a
custom engine built on top of [BASS Audio Library](https://www.un4seen.com/) to provide easier workflow for composers
and additional functions for the
perfect soundtrack design. The most important features include:

Features:

* Direct playback of common audio formats (WAV, OGG, MP3) instead of DirectMusic proprietary formats
* Out-of-the-box support for existing music theme definitions (just change the audio file)
* Alternative scheduling & transition systems to support smooth cross-fades between themes
* Loading of music files from VDF volumes (DirectMusic required physical files)
* Scriptable interface for controlling the system from Deadalus scripts
* Backward compatibility with original .sgt music (it's redirected to the DirectMusic system)

## Getting Started

Download `zBassMusic.vdf` and place it inside `<GOTHIC_ROOT>/Data` with Union installed to automatically load the system
at the start of the game. With default settings you can use modern audio formats directly in `MusicInst.d` like:

```cpp
instance SYS_MENU(C_MUSICTHEME_DEF)
{
	// 1. WAV file will load and play instead of .sgt
	file = "modern_audio_file.wav";      // old: file = "gamestart.sgt";
	
	// 2. You can use `transtype` to enable simple fade-in-fade-out transitions
	//      TRANSITION_TYPE_INTRO        - fade-in on start
	//      TRANSITION_TYPE_END          - fade-out on end
	//      TRANSITION_TYPE_ENDANDINTRO  - fade-in on start AND fade-out on end
	transtype = TRANSITION_TYPE_ENDANDINTRO;
	transsubtype = TRANSITION_SUB_TYPE_BEAT;
	
	// 3. Bass can emulate DX8 Reverb effect but in a limited range 
	reverbmix = -12;    // reverbmix in [-96, 0]        (dB)
	reverbtime = 3000;  // reverbtime in [0.001, 3000]  (milliseconds0)
	// If your instance exceeds these limits, the effect will not apply!
};
```

## Documentation

* [zBassMusic](https://silver-ore-team.github.io/zBassMusic/)
* [User Guide](https://silver-ore-team.github.io/zBassMusic/user-guide/)
    * [Getting Started](https://silver-ore-team.github.io/zBassMusic/user-guide/getting-started/)
* [Reference](https://silver-ore-team.github.io/zBassMusic/reference/)
* [Developer Guide](https://silver-ore-team.github.io/zBassMusic/developer-guide/)

## Build

Build instructions are available in
the [Developer Guide](https://silver-ore-team.github.io/zBassMusic/developer-guide/).

## Support

If you have found a bug in zBassMusic,
please [create an issue](https://github.com/Silver-Ore-Team/zBassMusic/issues/new).

Before creating a new issue, you can try to enable debug logging and get the logs to include in the issue.
See [Debugging](https://silver-ore-team.github.io/zBassMusic/user-guide/debugging/)

### Discord

You can contact the authors and maintainers on Discord. 
Look for [@tehe.official](https://discordapp.com/users/1014655735856111678).

#### Gothic Modding Community

Biggest community of international Gothic modders.
https://discord.gg/6rQQGVQK

#### 🇵🇱 Historia Neka

Official Discord of Nek's History, the mod we are working on and started zBassMusic for it.
https://discord.gg/cvd6jhKB`

## License

zBassMusic is licensed under [MIT license](LICENSE) excluding some dependencies.

[union-api](https://gitlab.com/union-framework/union-api)
and [gothic-api](https://gitlab.com/union-framework/gothic-api) are licensed
under [GNU GENERAL PUBLIC LICENSE V3](https://gitlab.com/union-framework/union-api-/blob/main/LICENSE).

BASS library is licensed under a proprietary license that's free for non-commercial use only. For commercial licensing
please refer to [www.un4seen.com](https://www.un4seen.com/).

GothicVDFS
2.6 [Copyright (c) 2001-2003, Nico Bendlin, Copyright (c) 1994-2002, Peter Sabath / TRIACOM Software GmbH](vdf/License.txt)