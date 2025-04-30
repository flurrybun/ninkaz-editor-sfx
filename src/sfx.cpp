#include "sfx.hpp"
#include <Geode/modify/EditorUI.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

std::unordered_set<EditorSFX> queuedSounds;

constexpr float SOUND_COOLDOWN = 0.03f;
constexpr float SLIDER_TICK_COOLDOWN = 0.06f;
std::unordered_map<EditorSFX, float> soundCooldowns;

void sfx::queue(EditorSFX sound) {
    queuedSounds.insert(sound);
}

void sfx::clearQueue() {
    queuedSounds.clear();
}

void sfx::playQueuedSounds() {
    if (queuedSounds.empty()) return;
    log::debug("Sounds: {}", queuedSounds.size());

    if (queuedSounds.count(EditorSFX::Paste) || queuedSounds.count(EditorSFX::Duplicate)) {
        queuedSounds.erase(EditorSFX::Place);
        queuedSounds.erase(EditorSFX::Select);
    }

    if (queuedSounds.count(EditorSFX::Place)) {
        queuedSounds.erase(EditorSFX::Select);
    }

    for (EditorSFX sound : queuedSounds) {
        playSound(sound);
    }

    queuedSounds.clear();
}

static void sfx::playSound(EditorSFX sound) {
    if (soundCooldowns[sound] > 0.f) {
        log::debug("Sound {} is on cooldown", static_cast<int>(sound));
        return;
    }

    auto path = getSoundPath(sound);
    if (path.empty()) {
        log::error("Sound not found: {}", path.string());
        return;
    }

    log::debug("Playing sound: {}", path.string());

    resetCooldown(sound);
    altTabFix();

    float volume = Mod::get()->getSettingValue<int64_t>("volume") / 100.f;
    FMODAudioEngine::get()->playEffect(path.string(), 1, 0, volume);
}

void sfx::updateCooldowns(float dt) {
    for (auto& [sound, cooldown] : soundCooldowns) {
        if (cooldown == 0.f) continue;

        cooldown -= dt;
        if (cooldown < 0.f) {
            cooldown = 0.f;
        }
    }
}

static void sfx::resetCooldown(EditorSFX sound) {
    if (sound == EditorSFX::SliderTick) {
        soundCooldowns[sound] = SLIDER_TICK_COOLDOWN;
    } else {
        soundCooldowns[sound] = SOUND_COOLDOWN;
    }
}

static std::filesystem::path sfx::getSoundPath(EditorSFX sound) {
    switch (sound) {
        case EditorSFX::Copy: return "copy.wav"_spr;
        case EditorSFX::Delete: return "delete.wav"_spr;
        case EditorSFX::Deselect: return "deselect.wav"_spr;
        case EditorSFX::Duplicate: return "duplicate.wav"_spr;
        case EditorSFX::Link: return "link.wav"_spr;
        case EditorSFX::Move: return "move.wav"_spr;
        case EditorSFX::Transform: return "transform.wav"_spr;
        case EditorSFX::Paste: return "paste.wav"_spr;
        case EditorSFX::Place: return "place.wav"_spr;
        case EditorSFX::Popup: return "popup.wav"_spr;
        case EditorSFX::Redo: return "redo.wav"_spr;
        case EditorSFX::Select: return "select.wav"_spr;
        case EditorSFX::SliderTick: return "slider-tick.wav"_spr;
        case EditorSFX::SwitchMode: return "switch-mode.wav"_spr;
        case EditorSFX::SwitchTab: return "switch-tab.wav"_spr;
        case EditorSFX::SwitchObject: return "switch-object.wav"_spr;
        case EditorSFX::SwitchLayer: return "switch-layer.wav"_spr;
        case EditorSFX::ToggleButton: return "toggle-button.wav"_spr;
        case EditorSFX::Undo: return "undo.wav"_spr;
        case EditorSFX::Unlink: return "unlink.wav"_spr;
        case EditorSFX::ZoomIn: return "zoom-in.wav"_spr;
        case EditorSFX::ZoomOut: return "zoom-out.wav"_spr;
        default: return std::filesystem::path();
    }
}

void sfx::altTabFix() {
    FMODAudioEngine::get()->m_globalChannel->setPaused(false);
}
