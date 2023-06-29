# Options reference (.ini)
```ini
[BASSMUSIC]

; Transition time between audio themes in miliseconds
TransitionTime = 2000.0

; Disable reverb effect globally
ForceDisableReverb = false

; Force fade-in fade-out transition even if theme doesn't specify it
ForceFadeTransition = false

; Creates C_MUSICTHEME class in main parser to make it easier to use scriptable interface.
; It does not provide any coop with music parser.
CreateMainParserCMusicTheme = true

; Logger level for Union console (DEBUG, INFO, WARN, ERROR, OFF)
LoggerLevelUnion=INFO

; Logger level for ZSpy console (DEBUG, INFO, WARN, ERROR, OFF)
LoggerLevelZSpy=DEBUG
```