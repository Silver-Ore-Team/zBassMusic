# Debugging

If you encounter any issues with the application, you can use the following tools to help you debug the problem.

## Union Console

Union can start a console window alongside with Gothic and catch standard output where we print a lot of logs.
To enable the console, you need to set following settings in the `SystemPack.ini` file:

```ini
[CORE]
ShowDebugWindow = true
UseDebugWindowHost = true
```

## Logging Level

By default, we print only the severity >=INFO to Union Console and >=DEBUG to ZSpy.
To print all logs to the console, you can set TRACE logging level in `Gothic.ini`:

```ini
[BASSMUSIC]
LoggerLevelUnion=TRACE
LoggerLevelZSpy=TRACE
```

## Debug Symbols

Standard Release build does not contain debug symbols, so you cannot debug the application in the debugger easily
because you will see only the optimized assembly code.

If you encounter a crash on Release build,
please download the builds suffixed with `-pdb` like `zBassMusic-v0.1.4-pdb.zip` and replace Release with them.
These builds contain debug symbols, and now you can start Gothic with a debugger attached to catch the exception
causing the crash and see the source code where it happened.
If you get some additional information about the crash from the debugger, please include it in the bug report.
