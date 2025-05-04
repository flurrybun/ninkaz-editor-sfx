#include "mod_hooker.hpp"
#include <Geode/modify/CCMenuItem.hpp>

CCSFXCallback* CCSFXCallback::create(const std::function<void(CCObject*)>& callback) {
    auto obj = new CCSFXCallback();
    obj->autorelease();
    obj->m_callback = callback;
    return obj;
}

void CCSFXCallback::execute(CCObject* sender) {
    if (m_callback) m_callback(sender);
}

// i too wish i didn't have to hook ccmenuitem
// if there's a better way to do this i'd love to see it

class $modify(CCMenuItem) {
    $override
    void activate() {
        CCMenuItem::activate();

        auto callback = typeinfo_cast<CCSFXCallback*>(getUserObject("sfx-callback"_spr));
        if (!callback) return;

        callback->execute(this);
    }
};

void setCallback(CCNode* node, std::function<void(CCObject*)> callback) {
    if (!node) return;
    if (auto button = typeinfo_cast<CCMenuItem*>(node)) {
        auto sfxCallback = CCSFXCallback::create(callback);
        button->setUserObject("sfx-callback"_spr, sfxCallback);
    }
}

void setSFX(CCNode* node, EditorSFX sfx) {
    setCallback(node, [sfx](CCObject* sender) {
        sfx::queue(sfx);
    });
}

void setToggleSFX(CCNode* node, EditorSFX sfxOn, EditorSFX sfxOff) {
    setCallback(node, [sfxOn, sfxOff](CCObject* sender) {
        auto button = typeinfo_cast<CCMenuItemToggler*>(sender);
        if (!button) return;

        if (button->isOn()) {
            sfx::queue(sfxOn);
        } else {
            sfx::queue(sfxOff);
        }
    });
}

#ifndef GEODE_IS_WINDOWS
#include <cxxabi.h>
#endif

// this function was ripped straight from devtools and i have no shame

std::string getNodeName(CCNode* node) {
#ifdef GEODE_IS_WINDOWS
    return typeid(*node).name() + 6;
#else
    {
        std::string ret;

        int status = 0;
        auto demangle = abi::__cxa_demangle(typeid(*node).name(), 0, 0, &status);
        if (status == 0) {
            ret = demangle;
        }
        free(demangle);

        return ret;
    }
#endif
}
