---
name: zBassMusic
hide:
  - navigation
---

# zBassMusic

zBassMusic is a modern music system for Gothic games based on ZenGin. 
It replaces the original DirectMusic system with a custom engine built on top of [BASS Library](https://www.un4seen.com/)
to provide easier workflow for composers and additional functions for the perfect soundtrack design. 
The most important features include:

* Direct playback of common audio formats (WAV, OGG, MP3) instead of DirectMusic propertiary formats
* Out-of-the-box support for existing music theme definitions (just change the audio file)
* Alternative scheduling & transition systems to support smooth crossfades between themes 
* Loading of music files from VDF volumes (DirectMusic required physical files)
* Scriptable interface for controlling the system from Deadalus scripts
* Backward compatibility with original `.sgt` music (it's redirected to the DirectMusic system)


!!! tip "Project Status"

    zBassMusic is still in the development phase and some APIs may be unstable, incomplete or buggy. You can help the project by
    using the plugin and reporting bugs or proposing ideas for the next features. The current state and plans for the future are
    on the [Roadmap](roadmap/index.md).
        

<div class="grid cards" markdown>

-  ::octicons-rocket-16:{ .lg .middle } __Release Builds__

    ---

    Release builds are ready to be used in Gothic and we publish them automatically on our GitHub.

    [:octicons-arrow-right-24: Releases](https://github.com/Silver-Ore-Team/zBassMusic/releases)

-  :fontawesome-brands-github:{ .lg .middle } __Source Code__

    ---

    zBassMusic is open source and you can access the code on our public repo hosted by GitHub.

    [:octicons-arrow-right-24: GitHub](https://github.com/Silver-Ore-Team/zBassMusic)

-  :fontawesome-solid-bug:{ .lg .middle } __Bug Reporting__

    ---

    If you have found a bug in zBassMusic, please create an issue on GitHub and tell us what's wrong.

    [:octicons-arrow-right-24: Issues](https://github.com/Silver-Ore-Team/zBassMusic/issues)

-  :fontawesome-regular-message:{ .lg .middle } __Discussions__

    ---

    We listen to ideas for new features, so if something is missing from zBassMusic, let us know and we'll do it.

    [:octicons-arrow-right-24: Discussions](https://github.com/Silver-Ore-Team/zBassMusic/discussions)

</div>


## For modders

zBassMusic is designed exclusively for Gothic modders to include it in their mods and it's licensed 
under a permissive [MIT License](https://github.com/Silver-Ore-Team/zBassMusic/blob/main/LICENSE),
so you can use the plugin for free in any project[^1]. 
It's built using the new [union-api](https://gitlab.com/union-framework/union-api) and can be embedded 
either as a Union 1.0m plugin or as a completely standalone plugin for base Gothic with System Pack.

Check out [Getting Started](getting-started/index.md) for instructions how to start working with zBassMusic.

[^1]: zBassMusic depends on vendored libraries [union-api](https://gitlab.com/union-framework/union-api) and [gothic-api]()
        licensed under [GNU GPL Version 3](https://gitlab.com/union-framework/union-api/-/blob/main/LICENSE)
        and on propertiary licensed [BASS Library](https://www.un4seen.com/) that's free for non-commercial use only.
        Make sure you are not violating these terms while using zBassMusic.

## For players

Unlike most Union plugins, zBassMusic doesn't provide any functions directly for the players who just play base game or some
other mod utilizing the original music system. In order to make use of the plugin, you have to actually put your music in the
game, so there is no point of installing zBassMusic if you are not making a mod.

## Repository

zBassMusic is an open source project with the source code available on GitHub.

- [GitHub Repository](https://github.com/Silver-Ore-Team/zBassMusic)
- [Release Builds](https://github.com/Silver-Ore-Team/zBassMusic/releases)
- [Bug Reporting](https://github.com/Silver-Ore-Team/zBassMusic/issues)
- [Discussions](https://github.com/Silver-Ore-Team/zBassMusic/discussions)

## Authors

zBassMusic development started in 2023 as an internal plugin for [Nek's History](https://silveroreteam.pl/en/projects/nek)
to ease the collaboration with our composer[^2] who had zero knowledge of DirectMusic Producer workflow and forcing him to learn it
would be a complete waste of time and possibly limit the creativity. Sometime later the internal code base based on Union SDK 1.0m
was rewritten for [union&#8209;api](https://gitlab.com/union-framework/union-api) &mdash;&nbsp;the successor of Union SDK, and published as an open source project.

The project is maintained under [Silver Ore Team](https://silveroreteam.pl) modding group by [tehe](https://github.com/piotrmacha):

* Email: [piotr.macha@silveroreteam.pl](mailto:piotr.macha@silveroreteam.pl)
* Discord: [@tehe.official](https://discordapp.com/users/1014655735856111678)

The mentionable contributors for the early stage development are [Boguś](https://github.com/bogu9821) and [Emu](https://github.com/muczc1wek) who have helped sorting some things up. If you'd like to also help zBassMusic development, check out [Developer Guide](developer-guide/index.md) 
and feel free to submit a Pull Request. One accepted merge and you will join the gallery.

<div class="zbassmusic-contributos" markdown>

![Mosaic of zBassMusic contributors](https://contrib.nn.ci/api?repo=Silver-Ore-Team/zBassMusic)

</div>

[^2]: [Adam Dzieżyk](https://adamdziezyk.com/), composer for Golden Gate, Golden Gate 2, Nek's History and many non-Gothic projects.
        Also a reason why zBassMusic exists.