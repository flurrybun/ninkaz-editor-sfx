#pragma once

#include <filesystem>

enum class EditorSFX {
    Copy,
    Delete,
    Deselect,
    Duplicate,
    Link,
    Move,
    Transform,
    Paste,
    Place,
    Popup,
    Redo,
    Select,
    SliderTick,
    SwitchMode,
    SwitchTab,
    SwitchObject,
    SwitchLayer,
    ToggleButton,
    Undo,
    Unlink,
    ZoomIn,
    ZoomOut
};

namespace sfx {
    void queue(EditorSFX sound);
    void clearQueue();

    void playQueuedSounds();
    static void playSound(EditorSFX sound);
    static float getSpeed(EditorSFX sound);

    void updateCooldowns(float dt);
    static void resetCooldown(EditorSFX sound);

    std::filesystem::path getSoundPath(EditorSFX sound);
    void altTabFix();
    float generateRandomFloat(float min, float max);
}
