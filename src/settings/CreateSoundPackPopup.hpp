#pragma once

#include "SoundPackSetting.hpp"
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class CreateSoundPackPopup : public Popup<SoundPackSettingNode*> {
protected:
    SoundPackSettingNode* m_settingNode;
    EventListener<Task<Result<std::filesystem::path>>> m_pickListener;
    // CCMenuItemSpriteExtra* m_createBtn;
    TextInput* m_input;

    bool setup(SoundPackSettingNode* settingNode) override;
    CCNode* createOption(
        const std::string& header, const std::string& description, SEL_MenuHandler callback, bool addInput
    );
    void onImport(CCObject* sender);
    void onCreate(CCObject* sender);

    void validateSoundPackDir(const std::filesystem::path& path);
    void copySoundPack(const std::filesystem::path& src);

public:
    static CreateSoundPackPopup* create(SoundPackSettingNode* settingNode) {
        auto ret = new CreateSoundPackPopup();
        if (ret->initAnchored(370.f, 230.f, settingNode)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};
