#include "mod_hooker.hpp"

SFXCallback* SFXCallback::create(
    const std::function<void(CCObject*)>& callback, SEL_MenuHandler selector, CCObject* listener
) {
    auto obj = new SFXCallback();
    obj->autorelease();

    obj->m_callback = callback;
    obj->m_selector = selector;
    obj->m_listener = listener;

    return obj;
}

void SFXCallback::execute(CCObject* sender) {
    m_callback(sender);
    (m_listener->*m_selector)(sender);
}

void setCallback(CCNode* node, std::function<void(CCObject*)> callback) {
    if (!node) return;

    auto button = typeinfo_cast<CCMenuItem*>(node);
    if (!button) return;

    auto sfxCallback = SFXCallback::create(callback, button->m_pfnSelector, button->m_pListener);

    button->m_pfnSelector = menu_selector(SFXCallback::execute);
    button->m_pListener = sfxCallback;

    // prevent callback from being garbage collected
    button->setUserObject("sfx-callback"_spr, sfxCallback);
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
