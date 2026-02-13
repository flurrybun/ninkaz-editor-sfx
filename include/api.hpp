#pragma once

#include <Geode/loader/Dispatch.hpp>

#ifdef MY_MOD_ID
    #undef MY_MOD_ID
#endif

#define MY_MOD_ID "ninkaz.editor_sfx"

namespace sfx {
    enum class Sound {
        // Copying an object
        Copy = 0,
        // Deleting an object
        Delete = 1,
        // Deselecting an object
        Deselect = 2,
        // Duplicating an object
        Duplicate = 3,
        // Entering the editor
        EnterEditor = 4,
        // Locking objects/layers
        Lock = 5,
        // Moving an object
        Move = 6,
        // Rotating/flipping an object
        Transform = 7,
        // Pasting an object
        Paste = 8,
        // Placing an object
        Place = 9,
        // Opening a top-level popup
        Popup = 10,
        // Redoing an action
        Redo = 11,
        // Selecting an object
        Select = 12,
        // Moving a slider
        SliderTick = 13,
        // Switching between build, edit, and delete mode
        SwitchMode = 14,
        // Switching tabs in build mode
        SwitchTab = 15,
        // Switching highlighted object in build mode
        SwitchObject = 16,
        // Switching editor layers
        SwitchLayer = 17,
        // Toggling a setting, e.g. swipe or free move
        ToggleButton = 18,
        // Undoing an action
        Undo = 19,
        // Unlocking objects/layers
        Unlock = 20,
        // Zooming camera in
        ZoomIn = 21,
        // Zooming camera out
        ZoomOut = 22
    };

    /**
     * Queue a sound effect to be played on the next frame
     */
    inline void play(sfx::Sound sound) GEODE_EVENT_EXPORT_NORES(&play, (sound));

    /**
     * Block a sound effect from playing on the next frame
     */
    inline void suppress(sfx::Sound sound) GEODE_EVENT_EXPORT_NORES(&suppress, (sound));
}
