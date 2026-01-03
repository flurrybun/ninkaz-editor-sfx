#include "sfx.hpp"
#include "settings/SoundPackSetting.hpp"

#include <Geode/Geode.hpp>
#include <random>
using namespace geode::prelude;

std::array<bool, static_cast<size_t>(EditorSFX::_Count)> queuedSounds;
std::array<float, static_cast<size_t>(EditorSFX::_Count)> soundCooldowns;
std::array<float, static_cast<size_t>(EditorSFX::_Count)> delayBetweenSounds;

constexpr float SOUND_COOLDOWN = 0.03f;
constexpr float SLIDER_TICK_COOLDOWN = 0.06f;

void sfx::queue(EditorSFX sound) {
    queuedSounds[static_cast<size_t>(sound)] = true;
}

void sfx::removeFromQueue(EditorSFX sound) {
    queuedSounds[static_cast<size_t>(sound)] = false;
}

void sfx::clearQueue() {
    queuedSounds.fill(false);
}

bool sfx::isQueued(EditorSFX sound) {
    return queuedSounds[static_cast<size_t>(sound)];
}

void sfx::playQueuedSounds() {
    if (isQueued(EditorSFX::Paste) || isQueued(EditorSFX::Duplicate)) {
        removeFromQueue(EditorSFX::Place);
        removeFromQueue(EditorSFX::Select);
    }

    if (isQueued(EditorSFX::Place)) {
        removeFromQueue(EditorSFX::Select);
        removeFromQueue(EditorSFX::Lock);
    }

    for (size_t i = 0; i < queuedSounds.size(); ++i) {
        if (!queuedSounds[i]) continue;

        playSound(static_cast<EditorSFX>(i));
        queuedSounds[i] = false;
    }
}

static void sfx::playSound(EditorSFX sound) {
    if (soundCooldowns[static_cast<size_t>(sound)] > 0.f) {
        resetDelay(sound);

        log::debug("Sound {} is on cooldown", static_cast<int>(sound));
        return;
    }

    GEODE_UNWRAP_OR_ELSE(path, err, getSoundPath(sound)) {
        log::error("{}", err);
        return;
    }

    log::debug("Playing sound: {}", path.string());

    float volume = Mod::get()->getSettingValue<int64_t>("volume") / 100.f;
    float speed = getSpeed(sound);

    resetCooldown(sound);
    resetDelay(sound);

    altTabFix();

    FMODAudioEngine::get()->playEffect(path.string(), speed, 0, volume);
}

float sfx::getSpeed(EditorSFX sound) {
    float delay = delayBetweenSounds[static_cast<size_t>(sound)];
    float speedMod = 1.f;

    float factor = sound == EditorSFX::SliderTick ? 15.f : 5.f;
    float minDelay = sound == EditorSFX::SliderTick ? 0.025f : 0.04f;

    if (delay < minDelay) {
        speedMod += (minDelay - delay) * factor;
    }

    switch (sound) {
        case EditorSFX::Place:
        case EditorSFX::Delete:
        case EditorSFX::Move:
            return speedMod * generateRandomFloat(0.97f, 1.03f);
        case EditorSFX::SliderTick:
            return speedMod * generateRandomFloat(0.94f, 1.06f);
        case EditorSFX::Transform:
        case EditorSFX::ZoomIn:
        case EditorSFX::ZoomOut:
            return generateRandomFloat(0.97f, 1.03f);
        default:
            return 1.f;
    }
}

void sfx::updateCooldowns(float dt) {
    for (size_t i = 0; i < soundCooldowns.size(); ++i) {
        if (soundCooldowns[i] > 0.f) {
            soundCooldowns[i] -= dt;
        }

        if (delayBetweenSounds[i] < 1.f) {
            delayBetweenSounds[i] += dt;
        }
    }
}

static void sfx::resetCooldown(EditorSFX sound) {
    soundCooldowns[static_cast<size_t>(sound)] = sound == EditorSFX::SliderTick
        ? SLIDER_TICK_COOLDOWN
        : SOUND_COOLDOWN;
}

static void sfx::resetDelay(EditorSFX sound) {
    delayBetweenSounds[static_cast<size_t>(sound)] = 0.f;
}

std::string sfx::getSoundName(EditorSFX sound) {
    switch (sound) {
        case EditorSFX::Copy: return "copy";
        case EditorSFX::Delete: return "delete";
        case EditorSFX::Deselect: return "deselect";
        case EditorSFX::Duplicate: return "duplicate";
        case EditorSFX::EnterEditor: return "enter-editor";
        case EditorSFX::Lock: return "lock";
        case EditorSFX::Move: return "move";
        case EditorSFX::Transform: return "transform";
        case EditorSFX::Paste: return "paste";
        case EditorSFX::Place: return "place";
        case EditorSFX::Popup: return "popup";
        case EditorSFX::Redo: return "redo";
        case EditorSFX::Select: return "select";
        case EditorSFX::SliderTick: return "slider-tick";
        case EditorSFX::SwitchMode: return "switch-mode";
        case EditorSFX::SwitchTab: return "switch-tab";
        case EditorSFX::SwitchObject: return "switch-object";
        case EditorSFX::SwitchLayer: return "switch-layer";
        case EditorSFX::ToggleButton: return "toggle-button";
        case EditorSFX::Undo: return "undo";
        case EditorSFX::Unlock: return "unlock";
        case EditorSFX::ZoomIn: return "zoom-in";
        case EditorSFX::ZoomOut: return "zoom-out";
        default:
            log::error("Missing sound name: {}", static_cast<int>(sound));
            return "missing";
    }
}

Result<std::filesystem::path> sfx::getSoundPath(EditorSFX sound) {
    std::filesystem::path soundPackDir = Mod::get()->getSettingValue<SoundPack>("sound-pack").path;
    std::string soundName = getSoundName(sound);
    std::vector<std::string> extensions = {".wav", ".mp3", ".ogg", ".flac"};

    for (const auto& ext : extensions) {
        std::filesystem::path soundPath = soundPackDir / (soundName + ext);

        if (std::filesystem::exists(soundPath)) {
            return Ok(soundPath);
        }
    }

    // fallback to default sfx

    std::filesystem::path defaultSoundPath = Mod::get()->getConfigDir() / "NinKaz's SFX v1" / (soundName + ".wav");

    if (std::filesystem::exists(defaultSoundPath)) {
        return Ok(defaultSoundPath);
    }

    return Err("Unable to find file for sound '{}'", soundName);
}

void sfx::altTabFix() {
    FMODAudioEngine::get()->m_globalChannel->setPaused(false);
}

float sfx::generateRandomFloat(float min, float max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);

    return dis(gen);
}
