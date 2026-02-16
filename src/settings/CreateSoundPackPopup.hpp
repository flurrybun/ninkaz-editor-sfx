#pragma once

#include "SoundPackSetting.hpp"
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class CreateSoundPackPopup : public Popup {
protected:
    SoundPackSettingNode* m_settingNode;
    TextInput* m_input;

    bool init(SoundPackSettingNode* settingNode);
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

        if (ret->init(settingNode)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};
