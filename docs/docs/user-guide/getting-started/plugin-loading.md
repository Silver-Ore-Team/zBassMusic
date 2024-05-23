# Plugin Loading

The next step is loading zBassMusic.dll to the Gothic memory, so it activates. There are several ways of doing it depending on 
your environment and expectations. Some methods are global for the whole Gothic installation where the plugin will always load
indifferently if you start mod A, mod B or the original game. Other methods make it possible to isolate the plugin to a single
mod, so it doesn't mess with others.

During the development, it's fine to use global methods on a local installation but when you are going to ship the mod to other
players, you should select an option with isolation. You never know if the player has a clean Gothic only for your mod or a gigadirctory 
with hundreds of GothicStarter entries, so you don't want to break someone's game by loading unsolicited DLLs globally.

## Union 1.0m

This method requires the player to install Union. Good option if your mod already uses other Union plugins and the runtime is there.

??? danger "Global Method"

    Choose either ZIP or VDF. Both work the same.

    From ZIP release:<br>
    Copy `zBassMusic.dll`, `UnionAPI.dll`, `bass.dll` <br>
    To `<GOTHIC_DIR>/System/Autorun/`

    From VDF release:<br>
    Copy `zBassMusic.vdf` <br>
    To `<GOTHIC_DIR>/Data/`

??? success "Isolated Method"

    The isolated method is possible if you are placing your mod in `<GOTHIC_DIR>/Data/ModVDF` and declare .mod file in `YourMod.ini`.

    From VDF release:<br>
    Rename `zBassMusic.vdf` to `zBassMusic.mod`<br>
    Copy `zBassMusic.mod`<br>
    To `<GOTHIC_DIR>/Data/ModVDF/`

    In `YourMod.ini` add an additional file to [FILES].VDF key:

    ```ini
    [FILES]
    ; NOTICE DOUBLE SPACE!
    ; If you use multiple VDF files, 
    ; you have to separate them with TWO spaces
    VDF=YourMod.mod  zBassMusic.mod 
    ```

## Standalone System Pack

It's possible to load zBassMusic on a clean Gothic with only System Pack using `pre.load` 
file but it's always global.

??? danger "Global Method"

    From ZIP release:<br>
    Copy `zBassMusic.dll`, `UnionAPI.dll`, `bass.dll` <br>
    To `<GOTHIC_DIR>/System/`

    Create `pre.load` file in `<GOTHIC_DIR>/System/` with content:
    ```
    zBassMusic.dll
    ```

## Standalone Ikarus

!!! warning "Not supported now"

    In the current release, we are hooking early into Gothic initialization, so Ikarus would load the plugin too late. In future releases, we may provide an alternative initialization that would be possible at any moment.

If you are using Ikarus in your scripts, it's possible to load dynamically any DLL using `LoadLibrary("AnyLib.dll")` function.
The function is conveniently provided by Ikarus and this method is always isolated.

??? success "Isolated Method"

    From ZIP release:<br>
    Copy `zBassMusic.dll`, `UnionAPI.dll`, `bass.dll` <br>
    To `<GOTHIC_DIR>/System/`

    In your `Startup.d` add to Init:
    ```dae
    func void INIT_GLOBAL()
    {
	    Game_InitGerman();
        LoadLibrary("zBassMusic.dll");
        // ...
    }
    ```
 
## Validate

To check if zBassMusic was loaded you can either look into ZSpy for strings like:
```
zBassMusic 0.1.3 Release (build: 2024-05-20T10:21:26, branch: feature/cleanup, revision: 466f67cedf3c23bfb6aa555f90653f67bb61742a)
```

Alternatively, you can try to call one of the externals in your Daedalus scripts, for example:
```dae

    func void INIT_GLOBAL()
    {   
        BassMusic_SetFullScriptControl(false);
        // ...
    }
```

If you get an error about missing external, then zBassMusic wasn't loaded, so you have to double-check if the chosen loading method 
was executed correctly. If you don't see any error, that means everything is fine and we can proceed to [music definitions](music-definition.md).