#include "SoundPackSetting.hpp"
#include "CreateSoundPackPopup.hpp"

Result<std::shared_ptr<SettingV3>> SoundPackSetting::parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
    auto ret = std::make_shared<SoundPackSetting>();

    auto root = checkJson(json, "SoundPackSetting");
    ret->parseBaseProperties(key, modID, root);

    std::filesystem::path path = Mod::get()->getResourcesDir();
    ret->setDefaultValue(path);
    ret->setValue(path);

    root.checkUnknownKeys();
    return root.ok(std::static_pointer_cast<SettingV3>(ret));
}

Result<> SoundPackSetting::isValid(SoundPack const& value) const {
    std::error_code ec;

    if (!std::filesystem::is_directory(value.path, ec)) {
        return Err("Value must be a folder");
    }

    return Ok();
}

SettingNode* SoundPackSetting::createNode(float width) {
    return SoundPackSettingNode::create(
        std::static_pointer_cast<SoundPackSetting>(shared_from_this()),
        width
    );
}

bool SoundPackSettingNode::init(std::shared_ptr<SoundPackSetting> setting, float width) {
    if (!SettingValueNodeV3::init(setting, width)) return false;

    getButtonMenu()->setLayout(
        SimpleRowLayout::create()
            ->setMainAxisScaling(AxisScaling::Grow)
            ->setGap(5.f)
    );

    auto folderSpr = CCSprite::createWithSpriteFrameName("folderIcon_001.png");
    folderSpr->setScale(0.25f);
    m_label = CCLabelBMFont::create("", "bigFont.fnt");

    m_labelContainer = CCNode::create();
    m_labelContainer->setContentSize({ 110.f, 20.f });
    m_labelContainer->addChild(folderSpr);
    m_labelContainer->addChild(m_label);
    m_labelContainer->setLayout(
        SimpleRowLayout::create()
            ->setGap(3.f)
    );

    auto labelBtn = CCMenuItemSpriteExtra::create(
        m_labelContainer, this, menu_selector(SoundPackSettingNode::onOpenFolder)
    );
    labelBtn->setContentSize({ 110.f, 20.f });
    m_labelContainer->setZOrder(1);
    m_labelContainer->setLayoutOptions(
        AnchorLayoutOptions::create()
            ->setAnchor(Anchor::Center)
    );

    auto labelBG = CCScale9Sprite::create("square02b_001.png");
    labelBG->setScale(0.35f);
    labelBG->setScaledContentSize({ 110.f, 20.f });
    labelBG->setColor(ccBLACK);
    labelBG->setOpacity(90);
    labelBtn->addChildAtPosition(labelBG, Anchor::Center);

    auto arrowLeftSpr = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    arrowLeftSpr->setFlipX(true);
    arrowLeftSpr->setScale(0.4f);
    auto arrowLeftBtn = CCMenuItemSpriteExtra::create(
        arrowLeftSpr, this, menu_selector(SoundPackSettingNode::onArrow)
    );
    arrowLeftBtn->setTag(-1);

    auto arrowRightSpr = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    arrowRightSpr->setScale(0.4f);
    auto arrowRightBtn = CCMenuItemSpriteExtra::create(
        arrowRightSpr, this, menu_selector(SoundPackSettingNode::onArrow)
    );
    arrowRightBtn->setTag(1);

    auto addTop = CCSprite::createWithSpriteFrameName("edit_addCBtn_001.png");
    auto addBase = CCSprite::create("GJ_button_01.png");
    addBase->addChildAtPosition(addTop, Anchor::Center);
    addBase->setScale(0.5f);
    auto addBtn = CCMenuItemSpriteExtra::create(
        addBase, this, menu_selector(SoundPackSettingNode::onAdd)
    );

    auto delTop = CCSprite::createWithSpriteFrameName("edit_delCBtn_001.png");
    m_delSpr = CCSprite::create("GJ_button_06.png");
    m_delSpr->addChildAtPosition(delTop, Anchor::Center);
    m_delSpr->setScale(0.5f);
    m_delBtn = CCMenuItemSpriteExtra::create(
        m_delSpr, this, menu_selector(SoundPackSettingNode::onDelete)
    );

    m_delSpr->setCascadeColorEnabled(true);
    m_delSpr->setCascadeOpacityEnabled(true);

    getButtonMenu()->addChild(arrowLeftBtn);
    getButtonMenu()->addChild(labelBtn);
    getButtonMenu()->addChild(arrowRightBtn);
    getButtonMenu()->addChild(addBtn);
    getButtonMenu()->addChild(m_delBtn);
    getButtonMenu()->updateLayout();

    updateState(nullptr);

    return true;
}

void SoundPackSettingNode::updateState(CCNode* invoker) {
    SettingValueNodeV3::updateState(invoker);

    auto path = getValue().path;

    m_label->setCString(getNameForSoundPackPath(path).c_str());
    m_label->limitLabelWidth(90.f, 0.4f, 0.f);

    bool isDefault = path == Mod::get()->getResourcesDir();
    m_delBtn->setEnabled(!isDefault);
    m_delSpr->setOpacity(isDefault ? 100 : 255);
    m_delSpr->setColor(isDefault ? ccGRAY : ccWHITE);

    getButtonMenu()->updateLayout();
    m_labelContainer->updateLayout();
}

void SoundPackSettingNode::onArrow(CCObject* sender) {
    auto packs = getSoundPacks();
    auto path = getValue().path;

    auto it = std::find(packs.begin(), packs.end(), path);
    int index = (it != packs.end()) ? std::distance(packs.begin(), it) : 0;

    if (sender->getTag() > 0) {
        index = index < packs.size() - 1 ? index + 1 : 0;
    } else {
        index = index > 0 ? index - 1 : packs.size() - 1;
    }

    setValue(packs.at(index), static_cast<CCNode*>(sender));
}

void SoundPackSettingNode::onAdd(CCObject* sender) {
    // m_pickListener.bind([this](geode::Task<geode::Result<std::filesystem::path>>::Event* event) {
    //     auto value = event->getValue();
    //     if (!value) return;

    //     if (value->isErr()) {
    //         FLAlertLayer::create(
    //             "Error",
    //             fmt::format("Failed to add sound pack: {}", value->unwrapErr()),
    //             "OK"
    //         )->show();
    //         return;
    //     }

    //     auto srcDir = value->unwrap();
    //     auto destDir = Mod::get()->getConfigDir() / srcDir.filename();

    //     std::error_code ec;
    //     std::filesystem::copy(srcDir, destDir, std::filesystem::copy_options::recursive, ec);

    //     if (ec) {
    //         FLAlertLayer::create(
    //             "Error",
    //             fmt::format("Failed to copy sound pack into config folder: {}", ec.message()),
    //             "OK"
    //         )->show();
    //         return;
    //     }

    //     FLAlertLayer::create(
    //         "Success",
    //         fmt::format("Successfully added sound pack \"{}\".", getNameForSoundPackPath(destDir)),
    //         "OK"
    //     )->show();

    //     setValue(destDir, nullptr);
    // });

    // std::error_code ec;

    // m_pickListener.setFilter(file::pick(
    //     file::PickMode::OpenFolder,
    //     {
    //         Mod::get()->getConfigDir(),
    //         std::vector<file::FilePickOptions::Filter>()
    //     }
    // ));

    auto popup = CreateSoundPackPopup::create(this);
    popup->show();
}

void SoundPackSettingNode::onDelete(CCObject* sender) {
    auto path = getValue().path;
    if (path == Mod::get()->getResourcesDir()) return;

    geode::createQuickPopup(
        "Delete Sound Pack",
        fmt::format("Are you sure you want to delete the sound pack \"{}\"? <cr>This action cannot be undone</c>.", getNameForSoundPackPath(path)),
        "Cancel", "Delete",
        [this, path](auto, bool isDelete) {
            if (!isDelete) return;

            std::error_code ec;
            std::filesystem::remove_all(path, ec);

            if (ec) {
                FLAlertLayer::create(
                    "Error",
                    fmt::format("Failed to delete sound pack: {}", ec.message()),
                    "OK"
                )->show();
                return;
            }

            setValue(Mod::get()->getResourcesDir(), nullptr);
        }
    );
}

void SoundPackSettingNode::onOpenFolder(CCObject* sender) {
    auto path = getValue().path;

    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }

    file::openFolder(path);
}

std::vector<std::filesystem::path> SoundPackSettingNode::getSoundPacks() {
    std::vector<std::filesystem::path> packs;
    auto soundPacksDir = Mod::get()->getConfigDir();

    packs.push_back(Mod::get()->getResourcesDir());

    for (const auto& entry : std::filesystem::directory_iterator(soundPacksDir)) {
        if (entry.is_directory()) {
            packs.push_back(entry.path());
        }
    }

    return packs;
}

std::string SoundPackSettingNode::getNameForSoundPackPath(std::filesystem::path const& path) {
    if (path == Mod::get()->getResourcesDir()) {
        return "Default";
    }

    return path.filename().string();
}

SoundPackSettingNode* SoundPackSettingNode::create(std::shared_ptr<SoundPackSetting> setting, float width) {
    auto ret = new SoundPackSettingNode();

    if (ret->init(setting, width)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

$execute {
    (void)Mod::get()->registerCustomSettingType("sound-pack", &SoundPackSetting::parse);
}
