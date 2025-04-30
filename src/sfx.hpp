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

    void updateCooldowns(float dt);
    static void resetCooldown(EditorSFX sound);

    static std::filesystem::path getSoundPath(EditorSFX sound);

    void altTabFix();
}
