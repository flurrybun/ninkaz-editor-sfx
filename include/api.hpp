#pragma once

#include <Geode/loader/Dispatch.hpp>

#ifdef MY_MOD_ID
    #undef MY_MOD_ID
#endif

#define MY_MOD_ID "ninkaz.editor_sfx"

namespace sfx {
    enum class Sound {
        Copy = 0,
        Delete = 1,
        Deselect = 2,
        Duplicate = 3,
        EnterEditor = 4,
        Lock = 5,
        Move = 6,
        Transform = 7,
        Paste = 8,
        Place = 9,
        Popup = 10,
        Redo = 11,
        Select = 12,
        SliderTick = 13,
        SwitchMode = 14,
        SwitchTab = 15,
        SwitchObject = 16,
        SwitchLayer = 17,
        ToggleButton = 18,
        Undo = 19,
        Unlock = 20,
        ZoomIn = 21,
        ZoomOut = 22
    };

    inline void play(sfx::Sound sound) GEODE_EVENT_EXPORT_NORES(&play, (sound));
}
