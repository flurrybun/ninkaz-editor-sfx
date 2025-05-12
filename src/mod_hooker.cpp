#include "mod_hooker.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CCLayerColor.hpp>
#include <Geode/modify/CCMenuItem.hpp>

std::vector<std::function<void(EditorUI*)>> editorUIHooks;
std::unordered_map<std::string, std::function<void(FLAlertLayer*)>> popupHooks;

void registerEditorUIHook(const std::string& modID, const std::function<void(EditorUI*)>& hookFunction) {
    if (!Loader::get()->isModLoaded(modID)) return;

    log::debug("Registering EditorUI hook for mod: {}", modID);
    editorUIHooks.push_back(hookFunction);
}

void registerPopupHook(const std::string& popupName, const std::string& modID, const std::function<void(FLAlertLayer*)>& hookFunction) {
    if (!Loader::get()->isModLoaded(modID)) return;

    log::debug("Registering popup hook for mod: {} on popup: {}", modID, popupName);
    popupHooks[popupName] = hookFunction;
}

class $modify(EditorUI) {
    static void onModify(auto& self) {
        if (!self.setHookPriority("EditorUI::init", Priority::VeryLatePost)) {
            log::warn("Failed to set hook priority for EditorUI::init");
        }
    }

    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel)) return false;
        if (editorUIHooks.empty()) return true;

        for (const auto& hookFunction : editorUIHooks) {
            hookFunction(this);
        }

        return true;
    }
};

class $modify(CCLayerColor) {
    bool initWithColor(const ccColor4B& color, float width, float height) {
        if (!CCLayerColor::initWithColor(color, width, height)) return false;
        if (!EditorUI::get()) return true;
        if (popupHooks.empty()) return true;

        this->retain();

        Loader::get()->queueInMainThread([this]() {
            for (const auto& [popupName, hookFunction] : popupHooks) {
                if (popupName != getNodeName(this)) continue;

                if (auto popup = typeinfo_cast<FLAlertLayer*>(this)) {
                    hookFunction(popup);
                }
            }

            this->release();
        });

        return true;
    }
};

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

// this function was ripped straight from devtools and i have no shame

#ifndef GEODE_IS_WINDOWS
#include <cxxabi.h>
#endif

std::string getNodeName(CCNode* node) {
#ifdef GEODE_IS_WINDOWS
    return typeid(*node).name() + 6;
#else
    std::string ret;

    int status = 0;
    auto demangle = abi::__cxa_demangle(typeid(*node).name(), 0, 0, &status);
    if (status == 0) {
        ret = demangle;
    }
    free(demangle);

    return ret;
#endif
}
