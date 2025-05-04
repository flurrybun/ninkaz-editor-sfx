#pragma once

#include "sfx.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CCLayerColor.hpp>

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
void setToggleSFX(CCNode* node, EditorSFX sfxOn, EditorSFX sfxOff);
std::string getNodeName(CCNode* node);

#define HOOK_EDITORUI_INIT(MOD_ID, CODE_BLOCK) \
class $modify(EditorUI) { \
    static void onModify(auto& self) { \
        if (!self.setHookPriorityAfterPost("EditorUI::init", MOD_ID)) { \
            geode::log::warn("Failed to set hook priority for EditorUI::init after {}", MOD_ID); \
        } \
    } \
    bool init(LevelEditorLayer* lel) { \
        if (!EditorUI::init(lel)) return false; \
        if (!Loader::get()->isModLoaded(MOD_ID)) return true; \
        CODE_BLOCK \
        return true; \
    } \
};

#define HOOK_MOD_POPUP(MOD_ID, ALERT_NAME, CODE_BLOCK) \
class $modify(CCLayerColor) { \
    bool initWithColor(const ccColor4B& color, float width, float height) { \
        if (!CCLayerColor::initWithColor(color, width, height)) return false; \
        if (!EditorUI::get()) return true; \
        if (ALERT_NAME != getNodeName(this)) return true; \
        this->retain(); \
        Loader::get()->queueInMainThread([this]() { \
            CODE_BLOCK \
            this->release(); \
        }); \
        return true; \
    } \
};
