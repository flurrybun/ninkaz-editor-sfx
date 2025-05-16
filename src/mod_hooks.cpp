#include "sfx.hpp"
#include "mod_hooker.hpp"
#include <Geode/modify/MenuLayer.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

// hooking other mods is volatile and nodes may be added or removed at any time
// this is especially true for hooking popups, as they may have collisions with other mods
// setCallback, setSFX, etc. check if the node is valid first for you, but otherwise
// always make sure to check if a node added by a mod is valid before using it

void registerModHooks() {
    registerEditorUIHook("hjfod.betteredit", [](EditorUI* self) {
        setSFX(
            self->querySelector("undo-menu > hjfod.betteredit/hide-ui-toggle"),
            EditorSFX::ToggleButton
        );
        setToggleSFX(
            self->m_scaleControl->querySelector("hjfod.betteredit/snap-lock"),
            EditorSFX::Lock, EditorSFX::Unlock
        );
        setToggleSFX(
            self->m_rotationControl->querySelector("hjfod.betteredit/snap-lock"),
            EditorSFX::Lock, EditorSFX::Unlock
        );
        setToggleSFX(
            self->m_rotationControl->querySelector("hjfod.betteredit/pos-lock"),
            EditorSFX::Lock, EditorSFX::Unlock
        );
        setSFX(
            self->m_scaleControl->querySelector("hjfod.betteredit/snap-lock-size"),
            EditorSFX::ToggleButton
        );
        setSFX(
            self->m_rotationControl->querySelector("hjfod.betteredit/snap-lock-size"),
            EditorSFX::ToggleButton
        );
    });

    registerEditorUIHook("razoom.object_groups", [](EditorUI* self) {
        for (auto editButtonBar : CCArrayExt<EditButtonBar*>(self->m_createButtonBars)) {
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
    });

    registerPopupHook("ObjectSelectPopup", "alphalaneous.creative_mode", [](FLAlertLayer* self) {
        auto tabsMenu = self->m_mainLayer->getChildByID("tabs-menu");
        if (!tabsMenu) return;

        for (auto child : CCArrayExt<CCNode*>(tabsMenu->getChildren())) {
            setSFX(child, EditorSFX::SwitchTab);
        }
    });
}

class $modify(MenuLayer) {
    $override
    bool init() {
        if (!MenuLayer::init()) return false;

        static bool hooksRegistered = false;

        if (!hooksRegistered) {
            registerModHooks();
            hooksRegistered = true;
        }

        return true;
    }
};
