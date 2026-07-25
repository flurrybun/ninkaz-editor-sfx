#pragma once

#include "sfx.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class SFXCallback : public CCObject {
private:
    std::function<void(CCObject*)> m_callback;
    SEL_MenuHandler m_selector;
    CCObject* m_listener;
public:
    static SFXCallback* create(
        const std::function<void(CCObject*)>& callback, SEL_MenuHandler selector, CCObject* listener
    );
    void execute(CCObject* sender);
};

void setCallback(CCNode* node, std::function<void(CCObject*)> callback);
void setSFX(CCNode* node, EditorSFX sfx);
void setToggleSFX(CCNode* node, EditorSFX sfx);
void setToggleSFX(CCNode* node, EditorSFX sfxOn, EditorSFX sfxOff);
