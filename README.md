# <img src="logo.png" width="40"> NinKaz's Editor SFX

Alleviate the deafening silence of the editor with over 20+ tactile sound effects, covering almost every action in the editor.

This mod is available on the [Geode index](https://geode-sdk.org/mods/ninkaz.editor_sfx) for Windows, Mac, Android, and iOS. After installing Geode, search for it in the in-game mod browser and click install.

## API Usage

While this mod has [special logic for existing mod compatibility](https://github.com/flurrybun/ninkaz-editor-sfx/blob/main/src/mod_hooks.cpp), it also exposes a dead-simple optional API for new mods to use (it's just two functions).

To use it, first list this mod as an optional dependency in your `mod.json`:

```json
"dependencies": {
    "ninkaz.editor_sfx": {
        "version": "1.0.0",
        "required": false
    }
}
```

Then, include the header and call `sfx::play` with the appropriate sound effect:

```cpp
#include <ninkaz.editor_sfx/include/api.hpp>

sfx::play(sfx::Sound::ToggleButton);
```

This function takes in an `sfx::Sound` enum. The available sound effects are as follows:

- `Place` - placing an object
- `Delete` - deleting an object
- `Move` - moving an object
- `Transform` - rotating/flipping an object
- `Select` - selecting an object
- `Deselect` - deselecting an object
- `Copy` - copying an object
- `Paste` - pasting an object
- `Duplicate` - duplicating an object
- `Undo` - undoing an action
- `Redo` - redoing an action
- `Lock` - locking objects/layers
- `Unlock` - unlocking objects/layers
- `ToggleButton` - toggling a setting, sometimes used as a generic button sound
- `SwitchLayer` - switching editor layers
- `SwitchMode` - switching between build, edit, and delete mode
- `SwitchTab` - switching tabs in build mode
- `SwitchObject` - switching highlighted object in build mode
- `ZoomIn` - zooming camera in
- `ZoomOut` - zooming camera out
- `SliderTick` - moving a slider
- `EnterEditor` - entering the editor

There may be instances where you want to prevent a sound effect from playing. You could of course set the hook priority to overwrite one of my lovely hooks, but you should instead use `sfx::suppress`.

As long as it's called on the same frame as `sfx::play` is called, it'll stop the sound effect from playing, as all sound effects are queued until the next frame rather than playing immediately. Additionally, playing the same sound effect twice won't cause it to double-up:

```cpp
sfx::play(sfx::Sound::Place);
sfx::suppress(sfx::Sound::Place);
// no sound is played

sfx::suppress(sfx::Sound::Place);
sfx::play(sfx::Sound::Place);
// no sound is played

sfx::play(sfx::Sound::Place);
sfx::play(sfx::Sound::Place);
// only one sound is played
```

## Build instructions

This mod can be built just like any other Geode mod. For more information, see the [Geode docs](https://docs.geode-sdk.org/getting-started/cpp-stuff/).

```sh
# Assuming you have the Geode CLI set up already
geode build
```

Some Geode-specific macros don't compile on MSVC. If you're having issues, try using Clang.
