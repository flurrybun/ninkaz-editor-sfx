#include "mod_hooker.hpp"
#include <Geode/modify/CCLayerColor.hpp>
#include <Geode/modify/CCMenuItem.hpp>

std::unordered_map<std::string, std::function<void(FLAlertLayer*)>> popupHooks;

void registerPopupHook(const std::string& popupName, const std::string& modID, const std::function<void(FLAlertLayer*)>& hookFunction) {
    if (!Loader::get()->isModLoaded(modID)) return;

    log::debug("Registering popup hook for mod: {} on popup: {}", modID, popupName);
    popupHooks[popupName] = hookFunction;
}

class $modify(CCLayerColor) {
    bool initWithColor(const ccColor4B& color, float width, float height) {
        if (!CCLayerColor::initWithColor(color, width, height)) return false;
        if (!EditorUI::get()) return true;
        if (popupHooks.empty()) return true;

        this->retain();

        Loader::get()->queueInMainThread([this]() {
            for (const auto& [popupName, hookFunction] : popupHooks) {
                if (popupName != getObjectName(this)) continue;

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
