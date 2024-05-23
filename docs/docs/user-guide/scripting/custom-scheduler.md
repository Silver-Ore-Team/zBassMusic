# Custom Scheduler

Example of a custom scheduler that plays [OLD_CAMP, NEW_CAMP, SWAMP_CAMP] in a loop.
The event handler listens for transition start and schedules next theme based on the current one.

```dae
void OnTransition(var float time_left) {
    if (Hlp_StrCmp(BassMusic_ActiveThemeID, "OLD_CAMP") == 0) {
        BassMusic_Play("NEW_CAMP");
    } else if (Hlp_StrCmp(BassMusic_ActiveThemeID, "NEW_CAMP") == 0) {
        BassMusic_Play("SWAMP_CAMP");
    } else if (Hlp_StrCmp(BassMusic_ActiveThemeID, "SWAMP_CAMP") == 0) {
        BassMusic_Play("OLD_CAMPP");
    }
}; 

void INIT_GLOBAL() {
	Game_InitGerman();

    BassMusic_SetFullScriptControl(true);
    BassMusic_OnTransitionEvent(OnTransition);
    BassMusic_Play("OLD_CAMP"); 
};
``` 
