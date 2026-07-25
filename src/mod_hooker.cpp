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

class $modify(CCMenuItem) {
    $override
    void activate() {
        CCMenuItem::activate();

        auto callback = static_cast<CCSFXCallback*>(getUserObject("sfx-callback"_spr));
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
        auto node = typeinfo_cast<CCNode*>(sender);
        if (!node || !nodeIsVisible(node)) return;

        sfx::queue(sfx);
    });
}

void setToggleSFX(CCNode* node, EditorSFX sfx) {
    setCallback(node, [sfx](CCObject* sender) {
        auto button = typeinfo_cast<CCMenuItemToggler*>(sender);
        if (!button || !nodeIsVisible(button)) return;

        sfx::queue(sfx);
    });
}

void setToggleSFX(CCNode* node, EditorSFX sfxOn, EditorSFX sfxOff) {
    setCallback(node, [sfxOn, sfxOff](CCObject* sender) {
        auto button = typeinfo_cast<CCMenuItemToggler*>(sender);
        if (!button || !nodeIsVisible(button)) return;

        if (button->isOn()) {
            sfx::queue(sfxOn);
        } else {
            sfx::queue(sfxOff);
        }
    });
}
