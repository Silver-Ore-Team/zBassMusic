# zBassMusic
zBassMusic is a modern music system for Gothic I and Gothic II NotR based on [BASS Audio Library](https://www.un4seen.com/) distributed as a Union plugin. It replaces the old DirectMusic system to let the modders create music for Gothic as regular audio files instead of unfriendly DirectMusic format.  

Features:
* Music playback with modern audio formats like WAV, MP3, OGG
* Out-of-the-box support for existing `C_MUSICTHEME` instances 
* Scriptable interface to take full control of music scheduling
* Loading music files from VDFS volumes (excluding .sgt)
* Backwards compatibility with DirectMusic .sgt files

## Project Status


## Usage

Download `zBassMusic.vdf` and place it inside `<GOTHIC_ROOT>/Data` with Union installed to automatically load the system at the start of the game. With default settings you can use modern audio formats directly in `MusicInst.d` like:
```cpp
instance SYS_MENU(C_MUSICTHEME_DEF)
{
    // wav file will load and play instead of .sgt
    file = "modern_audio_file.wav";
	// file = "gamestart.sgt";
	transtype = TRANSITION_TYPE_NONE;
	transsubtype = TRANSITION_SUB_TYPE_BEAT;
	reverbmix = -12;
	reverbtime = 8500;
};
```

For more advanced usage, check our docs:
- [Options reference (.ini)](docs/options.md)
- [Transitions and effects](docs/transitions-effects.md)
- [Scriptable interface (externals)](docs/scriptable-interface.md)

## Build

To build the project you need Visual Studio 2022 with v143 toolset (eg. 14.36.32532). Then you can clone the repository and update submodules:
```
git clone git@github.com:Silver-Ore-Team/zBassMusic.git
cd zBassMusic
git submodule update --remote --recursive
```

The project is based on CMake, so you can open it in Visual Studio, CLion or any other IDE supporting CMake, and build `plugin` target to compile `zBassMusic.dll`. 

Run `cmake --install out/build/x86-release --prefix out/install/x86-release` to generate release directory with all required DLLs and VDF file to include in Gothic.

## Support

If you have found a bug in zBassMusic, please [create an issue](https://github.com/Silver-Ore-Team/zBassMusic/issues/new) and state your problem in detail. Include zBassMusic version, plugin version and your environment (other plugins, stack traces etc.).

## License

zBassMusic is licensed under [MIT license](LICENSE) excluding some dependencies.

[union-api](https://gitlab.com/union-framework/union-api) and [gothic-api](https://gitlab.com/union-framework/gothic-api) are licensed under [GNU GENERAL PUBLIC LICENSE V3](https://gitlab.com/union-framework/union-api-/blob/main/LICENSE).

BASS library is licensed under a proprietary license that's free for non-commercial use only. For commercial licensing please refer to [www.un4seen.com](https://www.un4seen.com/).

GothicVDFS 2.6 [Copyright (c) 2001-2003, Nico Bendlin, Copyright (c) 1994-2002, Peter Sabath / TRIACOM Software GmbH](vdf/License.txt)