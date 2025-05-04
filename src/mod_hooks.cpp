#include "sfx.hpp"
#include "mod_hooker.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

HOOK_EDITORUI_INIT("hjfod.betteredit", {
    setSFX(
        this->querySelector("undo-menu > hjfod.betteredit/hide-ui-toggle"),
        EditorSFX::ToggleButton
    );
    setToggleSFX(
        m_scaleControl->querySelector("hjfod.betteredit/lock-menu > hjfod.betteredit/snap-lock"),
        EditorSFX::Link, EditorSFX::Unlink
    );
    setToggleSFX(
        m_rotationControl->querySelector("hjfod.betteredit/lock-menu > hjfod.betteredit/snap-lock"),
        EditorSFX::Link, EditorSFX::Unlink
    );
    setToggleSFX(
        m_rotationControl->querySelector("hjfod.betteredit/lock-menu > hjfod.betteredit/pos-lock"),
        EditorSFX::Link, EditorSFX::Unlink
    );
    setSFX(
        m_scaleControl->querySelector("hjfod.betteredit/lock-menu > hjfod.betteredit/snap-lock-size"),
        EditorSFX::ToggleButton
    );
    setSFX(
        m_rotationControl->querySelector("hjfod.betteredit/lock-menu > hjfod.betteredit/snap-lock-size"),
        EditorSFX::ToggleButton
    );
});

HOOK_EDITORUI_INIT("razoom.object_groups", {
    for (auto editButtonBar : CCArrayExt<EditButtonBar*>(m_createButtonBars)) {
        for (auto page : CCArrayExt<ButtonPage*>(editButtonBar->m_pagesArray)) {
            auto menu = page->getChildByType<CCMenu>(0);
            if (!menu) continue;

            for (auto child : CCArrayExt<CCNode*>(menu->getChildren())) {
                auto menuItem = typeinfo_cast<CreateMenuItem*>(child);
                if (!menuItem) continue;

                if (child->getTag() == 0) {
                    setSFX(
                        menuItem,
                        EditorSFX::SwitchTab
                    );
                }
            }
        }
    }
});

HOOK_MOD_POPUP("alphalaneous.creative_mode", "ObjectSelectPopup", {
    auto popup = typeinfo_cast<FLAlertLayer*>(this);
    if (!popup) return;
    auto tabsMenu = popup->m_mainLayer->getChildByID("tabs-menu");
    if (!tabsMenu) return;

    for (auto child : CCArrayExt<CCNode*>(tabsMenu->getChildren())) {
        setSFX(
            child,
            EditorSFX::SwitchTab
        );
    }
});
