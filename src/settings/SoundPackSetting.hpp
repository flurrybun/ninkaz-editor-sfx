#pragma once

#include <Geode/loader/Setting.hpp>

using namespace geode::prelude;

struct SoundPack {
    std::filesystem::path path;

    bool operator==(SoundPack const& other) const = default;

    SoundPack() = default;
    SoundPack(std::filesystem::path const& path) : path(path) {}
    SoundPack(SoundPack const&) = default;
};

template <>
struct matjson::Serialize<SoundPack> {
    static geode::Result<SoundPack> fromJson(matjson::Value const& value) {
        GEODE_UNWRAP_INTO(const std::string str, value.asString());

#ifdef GEODE_IS_WINDOWS
        return geode::Ok(
            std::filesystem::path(geode::utils::string::utf8ToWide(str)).make_preferred()
        );
#else
        return geode::Ok(std::filesystem::path(str).make_preferred());
#endif
    }

    static matjson::Value toJson(SoundPack const& value) {
        return matjson::Value(geode::utils::string::pathToString(value.path));
    }
};

class SoundPackSetting : public SettingBaseValueV3<SoundPack, SoundPack const&> {
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json);
    Result<> isValid(SoundPack const& value) const override;

    SettingNodeV3* createNode(float width) override;
};

template <>
struct geode::SettingTypeForValueType<SoundPack> {
    using SettingType = SoundPackSetting;
};

class SoundPackSettingNode : public SettingValueNode<SoundPackSetting> {
protected:
    CCLabelBMFont* m_label;
    CCNode* m_labelContainer;
    CCSprite* m_delSpr;
    CCMenuItemSpriteExtra* m_delBtn;

    bool init(std::shared_ptr<SoundPackSetting> setting, float width);
    void updateState(CCNode* invoker) override;

    void onArrow(CCObject* sender);
    void onAdd(CCObject* sender);
    void onDelete(CCObject* sender);
    void onOpenFolder(CCObject* sender);

public:
    static std::vector<std::filesystem::path> getSoundPacks();
    static std::string getNameForSoundPackPath(std::filesystem::path const& path);

    static SoundPackSettingNode* create(std::shared_ptr<SoundPackSetting> setting, float width);
};
