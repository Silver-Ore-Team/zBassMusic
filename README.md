# zBassMusic
zBassMusic is a modern music system for Gothic I and Gothic II NotR based on [BASS Audio Library](https://www.un4seen.com/) distributed as a Union plugin. It replaces the old DirectMusic system to let the modders create music for Gothic as regular audio files instead of unfriendly DirectMusic format.  

Features:
* Music playback with modern audio formats like WAV, MP3, OGG
* Out-of-the-box support for existing `C_MUSICTHEME` instances
* Scriptable interface to control music playback from Daedalus scripts 
* Loading music files from VDFS volumes (excluding .sgt)
* Backwards compatibility with DirectMusic .sgt files - they are forwarded to original system

## Project Status

The project is under development, but starting with the v0.1.0 version, we consider its core stable for general use. 
Now we focus on implementing more advanced control over the playback & transitions, like fine-tuning the
transition time points to cross-fade songs right on the beat.

## Getting Started

Download `zBassMusic.vdf` and place it inside `<GOTHIC_ROOT>/Data` with Union installed to automatically load the system at the start of the game. With default settings you can use modern audio formats directly in `MusicInst.d` like:
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

For more detailed information [check the project's Wiki](https://github.com/Silver-Ore-Team/zBassMusic/wiki).

## Build

To build the project you need Visual Studio 2022 with v143 toolset (eg. 14.39). Then you can clone the repository and update submodules:
```
git clone git@github.com:Silver-Ore-Team/zBassMusic.git
cd zBassMusic
git submodule init --remote
```

The project is based on CMake, so you can open it in Visual Studio, CLion or any other IDE supporting CMake, and build `plugin` target to compile `zBassMusic.dll`. 

Run `cmake --install out/build/x86-release --prefix out/install/x86-release` to generate release directory with all required DLLs and VDF file to include in Gothic.

## Support

If you have found a bug in zBassMusic, please [create an issue](https://github.com/Silver-Ore-Team/zBassMusic/issues/new) and state your problem in detail. 
Include zBassMusic version, Union version and your environment (other plugins, stack traces etc.).

Please enable Union console using `SystemPack.ini` and try to get the logs to include in the report.  
```ini
[CORE]
ShowDebugWindow = true
UseDebugWindowHost = true
```

### Discord

You can meet the plugin authors on Discord if you got a quick question. Look for `tehe`.
It's not an official support channel, so keep in mind that our SLA varies from 3 minutes to 71 weeks.   

#### Gothic Modding Community
Biggest community of international Gothic modders.
https://discord.gg/6rQQGVQK

#### 🇵🇱 Historia Neka
Official Discord of Nek's History, the mod we are working on and started zBassMusic for it. 
https://discord.gg/cvd6jhKB

## License

zBassMusic is licensed under [MIT license](LICENSE) excluding some dependencies.

[union-api](https://gitlab.com/union-framework/union-api) and [gothic-api](https://gitlab.com/union-framework/gothic-api) are licensed under [GNU GENERAL PUBLIC LICENSE V3](https://gitlab.com/union-framework/union-api-/blob/main/LICENSE).

BASS library is licensed under a proprietary license that's free for non-commercial use only. For commercial licensing please refer to [www.un4seen.com](https://www.un4seen.com/).

GothicVDFS 2.6 [Copyright (c) 2001-2003, Nico Bendlin, Copyright (c) 1994-2002, Peter Sabath / TRIACOM Software GmbH](vdf/License.txt)