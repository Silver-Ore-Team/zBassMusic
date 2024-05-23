# Options (.ini)

<div class="zbassmusic-externals" markdown>

Options are defined under `[BassMusic]` sction.

## <a href="#transitiontime">TransitionTime</a>

The time of the crossfade transition in milliseconds.

```dae
TransitionTime=2000
```

## <a href="#forcedisblereverb">ForceDisableReverb</a>

If true, all reverb effects are disabled.

```dae
ForceDisableReverb=0
```

## <a href="#forcefadetransition">ForceFadeTransition</a>

If true, all transitions use cross-fade even if it's not set in the theme.

```dae
ForceFadeTransition=0
```

## <a href="#createmainparsercmusictheme">CreateMainParserCMusicTheme</a>

If true, the class [C_MUSICTHEME](../classes/C_MUSICTHEME.md) is also created within the main scripts' parser
and you can define music themes in Content scripts. Be aware that it doesn't provide any sharing between parsers
so you can't use instance from music parser in the main parser - then you have to redefine it in the other

```dae
CreateMainParserCMusicTheme=1
```

## <a href="#loggerlevelunion">LoggerLevelUnion</a>

Sets the logger level for Union Console logs

```dae
LoggerLevelUnion=INFO
```

## <a href="#loggerlevelzspy">LoggerLevelZSpy</a>

Sets the logger level for ZSpy logs

```dae
LoggerLevelZSpy=DEBUG
```

</div>