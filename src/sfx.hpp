#pragma once

#include <filesystem>
#include <Geode/Result.hpp>

enum class EditorSFX {
    Copy,
    Delete,
    Deselect,
    Duplicate,
    EnterEditor,
    Lock,
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
    Unlock,
    ZoomIn,
    ZoomOut,
    _Count
};

namespace sfx {
    void queue(EditorSFX sound);
    void removeFromQueue(EditorSFX sound);
    void clearQueue();
    bool isQueued(EditorSFX sound);

    void playQueuedSounds();
    static void playSound(EditorSFX sound);
    static float getSpeed(EditorSFX sound);

    void setCooldown(EditorSFX sound, float cooldown);
    void updateCooldowns(float dt);
    static void resetCooldown(EditorSFX sound);
    static void resetDelay(EditorSFX sound);

    std::string getSoundName(EditorSFX sound);
    geode::Result<std::filesystem::path> getSoundPath(EditorSFX sound);
    std::string getSoundPackName();
    geode::Result<std::filesystem::path> getSoundPackDir();
    geode::Result<> moveDefaultSoundPack();

    void altTabFix();
    float generateRandomFloat(float min, float max);
}
