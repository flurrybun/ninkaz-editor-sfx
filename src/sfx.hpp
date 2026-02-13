#pragma once

#include <filesystem>
#include <Geode/Result.hpp>

enum class EditorSFX {
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

constexpr int SFX_COUNT = 23;

namespace sfx {
    void queue(EditorSFX sound);
    void removeFromQueue(EditorSFX sound);
    void clearQueue();
    bool isQueued(EditorSFX sound);

    void suppressSound(EditorSFX sound);

    void playQueuedSounds();
    static void playSound(EditorSFX sound);
    static float getSpeed(EditorSFX sound);

    void setCooldown(EditorSFX sound, float cooldown);
    void updateCooldowns(float dt);
    static void resetCooldown(EditorSFX sound);
    static void resetDelay(EditorSFX sound);

    std::string getSoundName(EditorSFX sound);
    geode::Result<std::filesystem::path> getSoundPath(EditorSFX sound);

    void altTabFix();
    float generateRandomFloat(float min, float max);
}
