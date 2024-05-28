# C_BassMusic_Theme

## Description

The class represents a music theme with zBassMusic extensions that can be played by the plugin.

## Definition

```dae
const int BASSMUSIC_THEME_TYPE_NORMAL = 0;

class C_BassMusic_Theme {
    var string name;
    var string zones;
    var int type;
};

prototype BassMusic_Theme(C_BassMusic_Theme) {
    type = BASSMUSIC_THEME_TYPE_NORMAL;
};
```

## Fields

| Field | Type   | Description                                                                       |
|-------|--------|-----------------------------------------------------------------------------------|
| name  | string | The name (identifier) of a theme.                                                 |
| zones | string | Comma-separated list of zones which play this theme (e.g. "SYS_MENU,OC_DAY_STD"). |
| type  | int    | Reserved for future use. Set it to BASSMUSIC_THEME_TYPE_NORMAL for now.           |

| type                        | Value | Type             |
|-----------------------------|-------|------------------|
| BASSMUSIC_THEME_TYPE_NORMAL | 0     | The normal type. |