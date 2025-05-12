#pragma once

#include "sfx.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

void registerEditorUIHook(const std::string& modID, const std::function<void(EditorUI*)>& hookFunction);
void registerPopupHook(const std::string& popupName, const std::string& modID, const std::function<void(FLAlertLayer*)>& hookFunction);

class CCSFXCallback : public CCObject {
private:
    std::function<void(CCObject*)> m_callback;
public:
    static CCSFXCallback* create(const std::function<void(CCObject*)>& callback);
    void execute(CCObject* sender);
};

void setCallback(CCNode* node, std::function<void(CCObject*)> callback);
void setSFX(CCNode* node, EditorSFX sfx);
void setToggleSFX(CCNode* node, EditorSFX sfxOn, EditorSFX sfxOff);

std::string getNodeName(CCNode* node);
