# Roadmap

## Done

### Core functions
!!! success "Music Engine (v0.1.0)"
    Music engine capable of replacing original DM system in the scope of playing audio and switching songs based on game events.

!!! success "Daedalus API for controlling playback (v0.1.0)"
    External functions to take control over music scheduling in Daedalus scripts. API is stable and shouldn't introduce any breaking changes.

!!! success "Simple crossfade (v0.1.0)"
    Every music theme can opt-in for simple fade-in and fade-out transitions to smoothly crossfade changing themes. The crossfade time is 
    set globally and the transition starts instantly after receiving an event from the game.


## In Progress 

###  Transition Scheduler

!!! info "Transition Scheduler (v0.1.x)"
    Scheduler for executing advanced transitions between songs based on defined rules. The system should be flexible and offer different 
    transition effects behind an easy-to-use interface. The artist should be able to define rules with high precision (soft goal: <10ms latency).

    !!! success "Instant Transition (Done)"
        Default transition starts the effect instantly after receiving an event from the game.

    !!! success "OnBeat Transition (Done)"
        Transition accepts a list of time points when the transition can happen and schedules it for the closest point. This way the song may 
        switch exactly in a moment when the beat ends to match the rhythm.

    !!! warning "Jingle Transition (To Do)"
        Transition plays an additional short audio during the transition as a one-time jingle. For example, battle music transitioning into 
        normal can play some theme to emphasize the end of a fight.

    !!! question "Format for defining the transition rules (Analysis / Planning)"
        We are still thinking about how can we accept the transition rules from developers. The options to consider are:

        * Daedalus API (not very ergonomic to pass lots of data)
        * MIDI file (easy to work with for composers because they can do it directly in their DAW)
        * Custom file format (we prefer not to introduce additional custom formats that require custom tooling)

        We may choose more than one option. At this moment the best option seems to be MIDI for defining time points + Daedalus API to setup 
        additional metadata.

## Future Plans

!!! question "New features?"
    We still may plan to implement some new features if they can provide value.

!!! danger "Complete Product Release (v1.0.0)"
    Before we release version v1.0.0 and mark zBassMusic as a finished product, we need to:
    
    * finish all the features in the backlog
    * battle-test the system by having it work in multiple released Gothic mods
    * fix all bugs and crashes that have a meaningful probability of causing Access Violations on players' games