---
hide:
  - navigation
---

# Roadmap

<div class="roadmap-legend" markdown>
!!! success "Done"
!!! info "In Progress"
!!! warning "Waiting in Queue"
!!! danger "Planned"
</div>

## Done

### Core functions

??? success "Music Engine (v0.1.0)"
Music engine capable of replacing the original DM system in the scope of playing audio and switching songs based on game
events.

??? success "Daedalus API for controlling playback (v0.1.0)"
External functions to take control over music scheduling in Daedalus scripts. API is stable and shouldn't introduce any
breaking changes.

??? success "Simple crossfade (v0.1.0)"
Every music theme can opt-in for simple fade-in and fade-out transitions to smoothly crossfade changing themes. The
crossfade time is
set globally and the transition starts instantly after receiving an event from the game.

## In Progress

### Transition Scheduler

??? info "Transition Scheduler (v0.2.x)"
Scheduler for executing advanced transitions between songs based on defined rules. The system should be flexible and
offer different
transition effects behind an easy-to-use interface. The artist should be able to define rules with high precision (soft
goal: <10ms latency).

    ??? success "Instant Transition (Done)"
        Default transition starts the effect instantly after receiving an event from the game.

    ??? success "OnBeat Transition (Done)"
        Transition accepts a list of time points when the transition can happen and schedules it for the closest point. This way the song may 
        switch exactly in a moment when the beat ends to match the rhythm.

    ??? warning "Jingle Transition (To Do)"
        Transition plays an additional short audio during the transition as a one-time jingle. For example, battle music transitioning into 
        normal can play some theme to emphasize the end of a fight.

??? info "MIDI bridge for Transition Scheduler (v0.2.x)"
The MIDI format is the best option for defining rules for the Transition Scheduler. The composer can just
put all the information about the transitions as MIDI events on some muted track in DAW and export it to the plugin.  
The composer uses a tool he knows, and we can extract precise information from the MIDI file perfectly synchronized
with the music.

    The bridge defines a spec how to interpret MIDI events and how they map to the internal structures of the Transition Scheduler.
    Then the bridge can load MIDI files and provide the rules to the scheduler.

## Future Plans

??? question "New features?"
We still may plan to implement some new features if they can provide value.

??? danger "Support late injection"
Right now the plugin hooks early into Gothic initialization to set itself up and replace the `zmusic` pointer with a
custom implementation.
We should add support for late initialization and let the plugin be loaded by Daedalus/Ikaus scripts.

??? danger "Complete Product Release (v1.0.0)"
Before we release version v1.0.0 and mark zBassMusic as a finished product, we need to:

    * finish all the features in the backlog
    * battle-test the system by having it work in multiple released Gothic mods
    * fix all bugs and crashes that have a meaningful probability of causing Access Violations on players' games