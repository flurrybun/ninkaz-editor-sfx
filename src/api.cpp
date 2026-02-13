#define GEODE_DEFINE_EVENT_EXPORTS
#include "../include/api.hpp"
#include "sfx.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

void sfx::play(sfx::Sound sound) {
    sfx::queue(static_cast<EditorSFX>(sound));
}

void sfx::suppress(sfx::Sound sound) {
    sfx::suppressSound(static_cast<EditorSFX>(sound));
}
