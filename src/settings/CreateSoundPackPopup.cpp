#include "CreateSoundPackPopup.hpp"

bool CreateSoundPackPopup::init(SoundPackSettingNode* settingNode) {
    if (!Popup::init(370.f, 230.f)) return false;

    setTitle("Add Sound Pack");
    m_settingNode = settingNode;

    auto importPackOption = createOption(
        "Import Existing",
        "Import an existing sound pack folder. It should only contain the sound files.",
        menu_selector(CreateSoundPackPopup::onImport),
        false
    );

    auto createPackOption = createOption(
        "Create New",
        "Create an empty sound pack folder. See the mod description for more info on how to create a sound pack.",
        menu_selector(CreateSoundPackPopup::onCreate),
        true
    );

    auto mainContainer = CCNode::create();
    mainContainer->setContentSize({ 320.f, 220.f });
    mainContainer->setAnchorPoint({ 0.5f, 0.5f });
    mainContainer->setLayout(
        SimpleColumnLayout::create()
            ->setGap(20.f)
    );

    mainContainer->addChild(importPackOption);
    mainContainer->addChild(createPackOption);
    mainContainer->updateLayout();

    m_mainLayer->addChildAtPosition(mainContainer, Anchor::Center, { 0.f, -7.f });

    return true;
}

CCNode* CreateSoundPackPopup::createOption(
    const std::string& header, const std::string& description, SEL_MenuHandler callback, bool addInput
) {
    auto menu = CCMenu::create();
    menu->setContentSize({ 320.f, 60.f });

    auto leftContainer = CCNode::create();
    leftContainer->setContentSize({ 180.f, 100.f });
    leftContainer->setAnchorPoint({ 0.f, 0.5f });
    menu->addChildAtPosition(leftContainer, Anchor::Left);

    leftContainer->setLayout(
        SimpleColumnLayout::create()
            ->setGap(5.f)
            ->setCrossAxisAlignment(CrossAxisAlignment::Start)
    );

    auto headerLabel = CCLabelBMFont::create(header.c_str(), "bigFont.fnt");
    headerLabel->setScale(0.5f);
    leftContainer->addChild(headerLabel);

    auto descLabel = CCLabelBMFont::create(description.c_str(), "chatFont.fnt", 180.f);
    descLabel->setScale(0.7f);
    leftContainer->addChild(descLabel);

    leftContainer->updateLayout();

    if (addInput) {
        auto createTop = CCSprite::createWithSpriteFrameName("edit_addCBtn_001.png");
        auto createBase = CCSprite::create("GJ_button_01.png");
        createBase->setScale(0.6f);
        createBase->addChildAtPosition(createTop, Anchor::Center);

        auto createBtn = CCMenuItemSpriteExtra::create(createBase, this, callback);
        createBtn->setAnchorPoint({ 1.f, 0.5f });

        // this is just excluding the characters that are invalid in folder names on windows
        // perhaps other platforms have more lax rules but i don't think it matters much

        m_input = TextInput::create(120.f, "Name");
        m_input->setScale(0.8f);
        m_input->setAnchorPoint({ 1.f, 0.5f });
        m_input->setFilter("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ#_-+&$%^~\'{}()[]=!@,;. ");

        menu->addChildAtPosition(m_input, Anchor::Right, { -30.f, 0.f });
        menu->addChildAtPosition(createBtn, Anchor::Right);
    } else {
        auto buttonSpr = ButtonSprite::create(
            "Import", "bigFont.fnt", "GJ_button_01.png"
        );
        buttonSpr->setScale(0.7f);
        auto button = CCMenuItemSpriteExtra::create(buttonSpr, this, callback);
        menu->addChildAtPosition(button, Anchor::Right, { -46.5f, 0.f });
    }

    return menu;
}

void CreateSoundPackPopup::onImport(CCObject* sender) {
    async::spawn(
        file::pick(
            file::PickMode::OpenFolder,
            {
                Mod::get()->getConfigDir(),
                std::vector<file::FilePickOptions::Filter>()
            }
        ),
        [this](Result<std::optional<std::filesystem::path>> result) {
            if (result.isErr()) {
                FLAlertLayer::create(
                    "Error",
                    fmt::format("Failed to add sound pack: {}", result.unwrapErr()),
                    "OK"
                )->show();
                return;
            }

            auto path = result.unwrap();
            if (!path) return;

            validateSoundPackDir(*path);
        }
    );
}

void CreateSoundPackPopup::onCreate(CCObject* sender) {
    std::string name = m_input->getString();

    if (name.empty()) {
        FLAlertLayer::create(
            "Error",
            "Invalid sound pack name.",
            "OK"
        )->show();
        return;
    }

    std::filesystem::path path = Mod::get()->getConfigDir() / name;
    auto res = file::createDirectory(path);

    if (res.isErr()) {
        FLAlertLayer::create(
            "Error",
            fmt::format("Failed to create directory: {}", res.unwrapErr()),
            "OK"
        )->show();
        return;
    }

    FLAlertLayer::create(
        "Success",
        fmt::format("Successfully created new sound pack <cy>{}</c>.", name),
        "OK"
    )->show();

    file::openFolder(path);
    m_settingNode->setValue(path, nullptr);
    onClose(nullptr);
}

void CreateSoundPackPopup::validateSoundPackDir(std::filesystem::path const& path) {
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        FLAlertLayer::create(
            "Error",
            "The selected path is not a valid directory.",
            "OK"
        )->show();
        return;
    }

    std::vector<std::string> warnings;
    std::vector<std::string> invalidFiles;
    std::vector<std::string> subdirectories;
    std::error_code ec;

    for (const auto& entry : std::filesystem::directory_iterator(path, ec)) {
        if (ec) {
            FLAlertLayer::create(
                "Error",
                fmt::format("Failed to read directory: {}", ec.message()),
                "OK"
            )->show();
            return;
        }

        if (entry.is_directory()) {
            subdirectories.push_back(entry.path().filename().string());
        } else if (entry.is_regular_file()) {
            auto ext = entry.path().extension().string();

            if (ext != ".wav" && ext != ".mp3" && ext != ".ogg" && ext != ".flac") {
                invalidFiles.push_back(entry.path().filename().string());
            }
        }
    }

    if (!subdirectories.empty()) {
        if (subdirectories.size() == 1) {
            warnings.push_back(fmt::format("Found subdirectory: {}", subdirectories[0]));
        } else {
            warnings.push_back(
                fmt::format("Found {} subdirectories: {}", subdirectories.size(), fmt::join(subdirectories, ", "))
            );
        }
    }

    if (!invalidFiles.empty()) {
        if (invalidFiles.size() == 1) {
            warnings.push_back(fmt::format("Found invalid file: {}", invalidFiles[0]));
        } else {
            warnings.push_back(fmt::format(
                "Found {} invalid files: {}", invalidFiles.size(), fmt::join(invalidFiles, ", ")
            ));
        }
    }

    if (warnings.empty()) {
        copySoundPack(path);
        return;
    }

    std::string warningMessage = "The following <cr>issues</c> were found:\n\n";
    for (const auto& warning : warnings) {
        warningMessage += warning + "\n";
    }
    warningMessage += "\nDo you want to import this pack anyway?";

    createQuickPopup(
        "Warning",
        warningMessage,
        "Cancel", "Import",
        [this, path](auto, bool isImport) {
            if (!isImport) return;
            copySoundPack(path);
        }
    );
}

void CreateSoundPackPopup::copySoundPack(const std::filesystem::path& src) {
    auto dest = Mod::get()->getConfigDir() / src.filename();

    std::error_code ec;
    std::filesystem::copy(src, dest, std::filesystem::copy_options::recursive, ec);

    if (ec) {
        FLAlertLayer::create(
            "Error",
            fmt::format("Failed to copy sound pack into config folder: {}", ec.message()),
            "OK"
        )->show();
        return;
    }

    FLAlertLayer::create(
        "Success",
        fmt::format(
            "Successfully added sound pack <cy>{}</c>.",
            SoundPackSettingNode::getNameForSoundPackPath(dest)
        ),
        "OK"
    )->show();

    m_settingNode->setValue(dest, nullptr);
    onClose(nullptr);
}
