#pragma once

#include "sfx.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class CCSFXCallback : public CCObject {
private:
    std::function<void(CCObject*)> m_callback;
public:
    static CCSFXCallback* create(const std::function<void(CCObject*)>& callback);
    void execute(CCObject* sender);
};

void setCallback(CCNode* node, std::function<void(CCObject*)> callback);
void setSFX(CCNode* node, EditorSFX sfx);
void setToggleSFX(CCNode* node, EditorSFX sfx);
void setToggleSFX(CCNode* node, EditorSFX sfxOn, EditorSFX sfxOff);
