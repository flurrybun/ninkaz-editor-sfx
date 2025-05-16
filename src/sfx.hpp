#pragma once

#include <filesystem>
#include <Geode/Result.hpp>

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

    std::string getSoundName(EditorSFX sound);
    geode::Result<std::filesystem::path> getSoundPath(EditorSFX sound);
    std::string getSoundPackName();
    geode::Result<std::filesystem::path> getSoundPackDir();
    geode::Result<> moveDefaultSoundPack();

    void altTabFix();
    float generateRandomFloat(float min, float max);
}
