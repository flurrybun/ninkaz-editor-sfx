#include "sfx.hpp"
#include "mod_hooker.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

// hooking other mods is volatile and nodes may be added or removed at any time
// this is especially true for hooking popups, as they may have collisions with other mods
// setCallback, setSFX, etc. check if the node is valid first for you, but otherwise
// always make sure to check if a node added by a mod is valid before using it

class $modify(EditorUI) {
    static void onModify(auto& self) {
        if (!self.setHookPriority("EditorUI::init", Priority::VeryLatePost)) {
            log::warn("Failed to set hook priority for EditorUI::init");
        }
    }

    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel)) return false;

        if (Loader::get()->isModLoaded("hjfod.betteredit")) {
            setSFX(
                querySelector("undo-menu > hjfod.betteredit/hide-ui-toggle"),
                EditorSFX::ToggleButton
            );
            setToggleSFX(
                m_scaleControl->querySelector("hjfod.betteredit/snap-lock"),
                EditorSFX::Lock, EditorSFX::Unlock
            );
            setToggleSFX(
                m_rotationControl->querySelector("hjfod.betteredit/snap-lock"),
                EditorSFX::Lock, EditorSFX::Unlock
            );
            setToggleSFX(
                m_rotationControl->querySelector("hjfod.betteredit/pos-lock"),
                EditorSFX::Lock, EditorSFX::Unlock
            );
            setSFX(
                m_scaleControl->querySelector("hjfod.betteredit/snap-lock-size"),
                EditorSFX::ToggleButton
            );
            setSFX(
                m_rotationControl->querySelector("hjfod.betteredit/snap-lock-size"),
                EditorSFX::ToggleButton
            );
        }

        if (Loader::get()->isModLoaded("razoom.object_groups")) {
            for (auto editButtonBar : CCArrayExt<EditButtonBar*>(m_createButtonBars)) {
                for (auto page : CCArrayExt<ButtonPage*>(editButtonBar->m_pagesArray)) {
                    auto menu = page->getChildByType<CCMenu>(0);
                    if (!menu) continue;

                    for (auto child : CCArrayExt<CCNode*>(menu->getChildren())) {
                        auto menuItem = typeinfo_cast<CreateMenuItem*>(child);
                        if (!menuItem) continue;

                        if (child->getTag() == 0) {
                            setSFX(menuItem, EditorSFX::SwitchTab);
                        }
                    }
                }
            }
        }

        if (Loader::get()->isModLoaded("alk.allium")) {
            if (auto bar = typeinfo_cast<EditButtonBar*>(getChildByID("alk.allium/allium-bar"))) {
                if (auto page = static_cast<ButtonPage*>(bar->m_pagesArray->objectAtIndex(0))) {
                    if (auto menu = page->getChildByType<CCMenu>(0)) {
                        setSFX(
                            menu->getChildByID("alk.allium/pan-toggle"),
                            EditorSFX::ToggleButton
                        );
                        setSFX(
                            menu->getChildByID("alk.allium/line-toggle"),
                            EditorSFX::ToggleButton
                        );
                        setSFX(
                            menu->getChildByID("alk.allium/curve-toggle"),
                            EditorSFX::ToggleButton
                        );
                        setSFX(
                            menu->getChildByID("alk.allium/free-toggle"),
                            EditorSFX::ToggleButton
                        );
                        setSFX(
                            menu->getChildByID("alk.allium/polygon-toggle"),
                            EditorSFX::ToggleButton
                        );
                        setSFX(
                            menu->getChildByID("alk.allium/text-toggle"),
                            EditorSFX::ToggleButton
                        );
                    }
                }
            }
        }

        return true;
    }
};

class $modify(MenuLayer) {
    $override
    bool init() {
        if (!MenuLayer::init()) return false;

        static bool hooksRegistered = false;

        if (hooksRegistered) return true;
        hooksRegistered = true;

        registerPopupHook("ObjectSelectPopup", "alphalaneous.creative_mode", [](FLAlertLayer* self) {
            auto tabsMenu = self->m_mainLayer->getChildByID("tabs-menu");
            if (!tabsMenu) return;

            for (auto child : CCArrayExt<CCNode*>(tabsMenu->getChildren())) {
                setSFX(child, EditorSFX::SwitchTab);
            }
        });

        // registerPopupHook("PasteStatePopup", "ninkaz.editor_utils", [](FLAlertLayer* self) {

        // });

        return true;
    }
};
