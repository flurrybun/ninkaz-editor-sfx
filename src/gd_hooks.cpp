#include "sfx.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/GJScaleControl.hpp>
#include <Geode/modify/GJTransformControl.hpp>
#include <Geode/modify/CCLayerColor.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class $modify(SFXEditorUI, EditorUI) {
    struct Fields {
        float prevScaleX;
        float prevScaleY;
        float prevRotationAngle;
    };

    static void onModify(auto& self) {
        if (!self.setHookPriorityBeforePre("EditorUI::angleChanged", "hjfod.betteredit")) {
            log::warn("Failed to set hook priority for EditorUI::angleChanged");
        }
        if (!self.setHookPriorityBeforePre("EditorUI::scrollWheel", "hjfod.betteredit")) {
            log::warn("Failed to set hook priority for EditorUI::scrollWheel");
        }
        if (!self.setHookPriorityAfterPost("EditorUI::onPasteState", "ninkaz.editor_utils")) {
            log::warn("Failed to set hook priority for EditorUI::onPasteState");
        }
    }

    $override
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel)) return false;

        sfx::clearQueue();
        schedule(schedule_selector(SFXEditorUI::updateSFX));

        return true;
    }

    void updateSFX(float dt) {
        sfx::updateCooldowns(dt);

        if (m_editorLayer->m_playbackMode != PlaybackMode::Playing) {
            sfx::playQueuedSounds();
        } else {
            sfx::clearQueue();
        }
    }

    bool isEditorButtonDisabled(CCObject* sender) {
        auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
        if (!button) return false;

        return button->getOpacity() == 175;
    }

    $override
    void keyDown(enumKeyCodes key) {
        bool playSound = m_selectedObjects->count() > 0 || m_selectedObject;

        EditorUI::keyDown(key);

        // windows 2.2074 inlines EditorUI::onDeselectAll in EditorUI::keyDown
        if (
            key == enumKeyCodes::KEY_D &&
            CCKeyboardDispatcher::get()->getAltKeyPressed() &&
            playSound
        ) {
            sfx::queue(EditorSFX::Deselect);
        }
    }

    $override
    void selectObject(GameObject* object, bool p1) {
        EditorUI::selectObject(object, p1);
        sfx::queue(EditorSFX::Select);
    }

    $override
    void selectObjects(CCArray* objects, bool p1) {
        unsigned int prevSelectedObjectCount = m_selectedObjects->count();
        EditorUI::selectObjects(objects, p1);

        if (prevSelectedObjectCount < m_selectedObjects->count()) {
            sfx::queue(EditorSFX::Select);
        }
    }

    $override
    void onDeselectAll(CCObject* sender) {
        bool playSound = m_selectedObjects->count() > 0 || m_selectedObject;
        EditorUI::onDeselectAll(sender);

        if (playSound) {
            sfx::queue(EditorSFX::Deselect);
        }
    }

    $override
    void onDelete(CCObject* sender) {
        int prevObjectCount = m_editorLayer->m_objects->count();
        EditorUI::onDelete(sender);

        if (prevObjectCount > m_editorLayer->m_objects->count()) {
            sfx::queue(EditorSFX::Delete);
        }
    }

    $override
    void onDeleteSelected(CCObject* sender) {
        int prevObjectCount = m_editorLayer->m_objects->count();
        EditorUI::onDeleteSelected(sender);

        if (prevObjectCount > m_editorLayer->m_objects->count()) {
            sfx::queue(EditorSFX::Delete);
        }
    }

    $override
    void doCopyObjects(bool p0) {
        EditorUI::doCopyObjects(p0);
        if (isEditorButtonDisabled(m_copyBtn)) return;

        sfx::queue(EditorSFX::Copy);
    }

    $override
    void doPasteObjects(bool p0) {
        EditorUI::doPasteObjects(p0);
        if (isEditorButtonDisabled(m_pasteBtn)) return;

        sfx::queue(EditorSFX::Paste);
    }

    $override
    void onDuplicate(CCObject* sender) {
        bool playSound = m_selectedObjects->count() > 0 || m_selectedObject;

        EditorUI::onDuplicate(sender);
        if (!playSound || isEditorButtonDisabled(sender)) return;

        sfx::queue(EditorSFX::Duplicate);
    }

    $override
    void onCopyState(CCObject* sender) {
        EditorUI::onCopyState(sender);
        if (isEditorButtonDisabled(sender)) return;

        sfx::queue(EditorSFX::Copy);
    }

    $override
    void onPasteState(CCObject* sender) {
        EditorUI::onPasteState(sender);
        if (isEditorButtonDisabled(sender)) return;

        sfx::queue(EditorSFX::Paste);
    }

    $override
    void onPasteColor(CCObject* sender) {
        EditorUI::onPasteColor(sender);
        if (isEditorButtonDisabled(sender)) return;

        sfx::queue(EditorSFX::Paste);
    }

    $override
    void toggleMode(CCObject* sender) {
        int prevMode = m_selectedMode;
        EditorUI::toggleMode(sender);

        if (m_selectedMode != prevMode) {
            sfx::queue(EditorSFX::SwitchMode);
        }
    }

    $override
    void onSelectBuildTab(CCObject* sender) {
        int prevTab = m_selectedTab;
        EditorUI::onSelectBuildTab(sender);

        if (m_selectedTab != prevTab) {
            sfx::queue(EditorSFX::SwitchTab);
        }
    }

    $override
    void onCreateButton(CCObject* sender) {
        int prevIndex = m_selectedObjectIndex;
        EditorUI::onCreateButton(sender);

        if (m_selectedObjectIndex != prevIndex) {
            sfx::queue(EditorSFX::SwitchObject);
        }
    }

    $override
    void toggleSwipe(CCObject* sender) {
        EditorUI::toggleSwipe(sender);
        sfx::queue(EditorSFX::ToggleButton);
    }

    $override
    void toggleEnableRotate(CCObject* sender) {
        EditorUI::toggleEnableRotate(sender);
        sfx::queue(EditorSFX::ToggleButton);
    }

    $override
    void toggleFreeMove(CCObject* sender) {
        EditorUI::toggleFreeMove(sender);
        sfx::queue(EditorSFX::ToggleButton);
    }

    $override
    void toggleSnap(CCObject* sender) {
        EditorUI::toggleSnap(sender);
        sfx::queue(EditorSFX::ToggleButton);
    }

    $override
    void onUpdateDeleteFilter(CCObject* sender) {
        int prevDeleteFilter = GameManager::get()->getIntGameVariable("0005");
        EditorUI::onUpdateDeleteFilter(sender);

        if (prevDeleteFilter != GameManager::get()->getIntGameVariable("0005")) {
            sfx::queue(EditorSFX::ToggleButton);
        }
    }

    $override
    void moveObjectCall(EditCommand p0) {
        bool playSound = m_selectedObjects->count() > 0 || m_selectedObject;
        EditorUI::moveObjectCall(p0);

        if (playSound) {
            sfx::queue(EditorSFX::Move);
        }
    }

    $override
    void transformObjectCall(EditCommand command) {
        bool playSound = m_selectedObjects->count() > 0 || m_selectedObject;
        EditorUI::transformObjectCall(command);

        if (playSound) {
            sfx::queue(EditorSFX::Transform);
        }
    }

    $override
    void onGroupSticky(CCObject* sender) {
        if (!isEditorButtonDisabled(sender)) {
            sfx::queue(EditorSFX::Lock);
        }

        EditorUI::onGroupSticky(sender);
    }

    $override
    void onUngroupSticky(CCObject* sender) {
        if (!isEditorButtonDisabled(sender)) {
            sfx::queue(EditorSFX::Unlock);
        }

        EditorUI::onUngroupSticky(sender);
    }

    $override
    void updateGroupIDLabel() {
        std::string prevLabelString = m_currentLayerLabel->getString();
        EditorUI::updateGroupIDLabel();

        if (prevLabelString != m_currentLayerLabel->getString()) {
            sfx::queue(EditorSFX::SwitchLayer);
        }
    }

    $override
    void onLockLayer(CCObject* sender) {
        EditorUI::onLockLayer(sender);

        short currentLayer = m_editorLayer->m_currentLayer;
        gd::vector<bool>& lockedLayers = m_editorLayer->m_lockedLayers;

        if (lockedLayers[currentLayer]) {
            sfx::queue(EditorSFX::Lock);
        } else {
            sfx::queue(EditorSFX::Unlock);
        }
    }

    $override
    void updateZoom(float zoom) {
        float prevZoom = m_editorLayer->m_objectLayer->getScale();
        EditorUI::updateZoom(zoom);

        float newZoom = m_editorLayer->m_objectLayer->getScale();
        if (std::abs(prevZoom - newZoom) < 0.01f) return;

        if (m_editorZoom < zoom) {
            sfx::queue(EditorSFX::ZoomIn);
        } else if (m_editorZoom > zoom) {
            sfx::queue(EditorSFX::ZoomOut);
        }
    }

    $override
    void scrollWheel(float p0, float p1) {
        CCPoint prevPosition = m_editorLayer->m_objectLayer->getPosition();
        EditorUI::scrollWheel(p0, p1);

        if (CCKeyboardDispatcher::get()->getControlKeyPressed()) return;
        CCPoint newPosition = m_editorLayer->m_objectLayer->getPosition();

        if (std::abs(prevPosition.x - newPosition.x) > 0.01f || std::abs(prevPosition.y - newPosition.y) > 0.01f) {
            sfx::queue(EditorSFX::SliderTick);
        }
    }

    $override
    void sliderChanged(CCObject* sender) {
        float prevXPos = m_editorLayer->m_objectLayer->getPositionX();

        EditorUI::sliderChanged(sender);

        if (prevXPos != m_editorLayer->m_objectLayer->getPositionX()) {
            sfx::queue(EditorSFX::SliderTick);
        }
    }

    $override
    void scaleXYChanged(float scaleX, float scaleY, bool lock) {
        if (scaleX != m_fields->prevScaleX || scaleY != m_fields->prevScaleY) {
            sfx::queue(EditorSFX::SliderTick);
        }

        EditorUI::scaleXYChanged(scaleX, scaleY, lock);

        m_fields->prevScaleX = scaleX;
        m_fields->prevScaleY = scaleY;
    }

    $override
    void scaleXChanged(float scaleX, bool lock) {
        EditorUI::scaleXChanged(scaleX, lock);

        if (scaleX != m_fields->prevScaleX) {
            sfx::queue(EditorSFX::SliderTick);
        }

        m_fields->prevScaleX = scaleX;
    }

    $override
    void scaleYChanged(float scaleY, bool lock) {
        EditorUI::scaleYChanged(scaleY, lock);

        if (scaleY != m_fields->prevScaleY) {
            sfx::queue(EditorSFX::SliderTick);
        }

        m_fields->prevScaleY = scaleY;
    }

    $override
    void angleChanged(float angle) {
        EditorUI::angleChanged(angle);

        if (angle != m_fields->prevRotationAngle) {
            sfx::queue(EditorSFX::SliderTick);
        }

        m_fields->prevRotationAngle = angle;
    }

    $override
    void onPause(CCObject* sender) {
        EditorUI::onPause(sender);
        sfx::queue(EditorSFX::Popup);
    }
};

class $modify(SFXLevelEditorLayer, LevelEditorLayer) {
    $override
    GameObject* createObject(int p0, CCPoint p1, bool p2) {
        sfx::queue(EditorSFX::Place);
        return LevelEditorLayer::createObject(p0, p1, p2);
    }

    // in win 2.2074, LevelEditorLayer::removeObject is inlined in quite a few places

    // $override
    // void removeObject(GameObject* p0, bool p1) {
    //     sfx::queue(EditorSFX::Delete);
    //     LevelEditorLayer::removeObject(p0, p1);
    // }

    $override
    void removeAllObjectsOfType(int objectID) {
        int prevObjectCount = m_objects->count();
        LevelEditorLayer::removeAllObjectsOfType(objectID);

        if (prevObjectCount > m_objects->count()) {
            sfx::queue(EditorSFX::Delete);
        }
    }

    $override
    void handleAction(bool isUndo, CCArray* undoObjects) {
        if (isUndo) sfx::queue(EditorSFX::Undo);
        else sfx::queue(EditorSFX::Redo);

        if (undoObjects->count() == 0) return LevelEditorLayer::handleAction(isUndo, undoObjects);
        auto undoObject = typeinfo_cast<UndoObject*>(undoObjects->lastObject());

        log::info("isUndo: {}", isUndo);

        switch (undoObject->m_command) {
            case UndoCommand::Delete: log::info("Delete"); break;
            case UndoCommand::New: log::info("New"); break;
            case UndoCommand::Paste: log::info("Paste"); break;
            case UndoCommand::DeleteMulti: log::info("DeleteMulti"); break;
            case UndoCommand::Transform: log::info("Transform"); break;
            case UndoCommand::Select: log::info("Select"); break;
        }

        #define caseSound(command, redoSound, undoSound) case command: sfx::queue(isUndo ? undoSound : redoSound); break;

        switch (undoObject->m_command) {
            caseSound(UndoCommand::Delete, EditorSFX::Delete, EditorSFX::Place)
            caseSound(UndoCommand::New, EditorSFX::Place, EditorSFX::Delete)
            caseSound(UndoCommand::Paste, EditorSFX::Place, EditorSFX::Delete)
            caseSound(UndoCommand::DeleteMulti, EditorSFX::Delete, EditorSFX::Place)
            caseSound(UndoCommand::Transform, EditorSFX::Move, EditorSFX::Move)
            caseSound(UndoCommand::Select, EditorSFX::Deselect, EditorSFX::Select)
        }

        LevelEditorLayer::handleAction(isUndo, undoObjects);
    }
};

class $modify(SFXEditButtonBar, EditButtonBar) {
    $override
    void onLeft(CCObject* sender) {
        EditButtonBar::onLeft(sender);
        sfx::queue(EditorSFX::SwitchTab);
    }

    $override
    void onRight(CCObject* sender) {
        EditButtonBar::onRight(sender);
        sfx::queue(EditorSFX::SwitchTab);
    }
};

class $modify(SFXGJScaleControl, GJScaleControl) {
    $override
    void onToggleLockScale(CCObject* sender) {
        GJScaleControl::onToggleLockScale(sender);

        if (m_scaleLocked) {
            sfx::queue(EditorSFX::Lock);
        } else {
            sfx::queue(EditorSFX::Unlock);
        }
    }
};

class $modify(SFXGJTransformControl, GJTransformControl) {
    $override
    void onToggleLockScale(CCObject* sender) {
        GJTransformControl::onToggleLockScale(sender);

        if (m_warpLocked) {
            sfx::queue(EditorSFX::Lock);
        } else {
            sfx::queue(EditorSFX::Unlock);
        }
    }
};

class $modify(SFXCCLayerColor, CCLayerColor) {
    // plays sfx for top-level popups
    // hooks cclayercolor because some popups skip over flalertlayer init

    $override
    bool initWithColor(const ccColor4B& color, float width, float height) {
        if (!CCLayerColor::initWithColor(color, width, height)) return false;
        if (!EditorUI::get()) return true;
        if (EditorUI::get()->m_isPaused) return true;
        if (!typeinfo_cast<FLAlertLayer*>(this)) return true;

        this->retain();

        Loader::get()->queueInMainThread([this]() {
            auto scene = CCDirector::get()->getRunningScene();

            for (auto layer : CCArrayExt<CCNode*>(scene->getChildren())) {
                if (typeinfo_cast<FLAlertLayer*>(layer)) {
                    if (layer == this) sfx::queue(EditorSFX::Popup);
                    break;
                }
            }

            this->release();
        });

        return true;
    }
};

class $modify(SFXFMODAudioEngine, FMODAudioEngine) {
    struct Fields {
        bool shouldUnloadAllEffects = true;
    };

    $override
    void unloadAllEffects() {
        if (m_fields->shouldUnloadAllEffects) {
            FMODAudioEngine::unloadAllEffects();
        }
    }
};

class $modify(SFXEditorPauseLayer, EditorPauseLayer) {
    $override
    void onExitEditor(CCObject* sender) {
        static_cast<SFXEditorUI*>(EditorUI::get())->unschedule(schedule_selector(SFXEditorUI::updateSFX));

        EditorPauseLayer::onExitEditor(sender);

        sfx::altTabFix();
        // @geode-ignore(unknown-resource)
        FMODAudioEngine::get()->playEffect("quitSound_01.ogg", 1, 0, 0.7);
    }

    // playStep2 normally calls FMODAudioEngine::unloadAllEffects, which cuts off the sound effect on save and play
    // i work around this by calling it in onSaveAndPlay instead

    $override
    void onSaveAndPlay(CCObject* sender) {
        sfx::altTabFix();
        FMODAudioEngine::get()->unloadAllEffects();

        // @geode-ignore(unknown-resource)
        FMODAudioEngine::get()->playEffect("playSound_01.ogg", 1, 0, 0.3);

        EditorPauseLayer::onSaveAndPlay(sender);
    }

    $override
    void playStep2() {
        auto sfxFMOD = static_cast<SFXFMODAudioEngine*>(FMODAudioEngine::get());

        sfxFMOD->m_fields->shouldUnloadAllEffects = false;
        EditorPauseLayer::playStep2();
        sfxFMOD->m_fields->shouldUnloadAllEffects = true;
    }

    $override
    void saveLevel() {
        EditorPauseLayer::saveLevel();
        sfx::clearQueue();
    }
};

class $modify(SFXEditLevelLayer, EditLevelLayer) {
    $override
    void onEdit(CCObject* sender) {
        // i don't use the queue system here because it would play the sound
        // after the lag spike from loading the level, which isn't ideal

        GEODE_UNWRAP_OR_ELSE(path, err, sfx::getSoundPath(EditorSFX::EnterEditor)) {
            log::error("{}", err);
            return EditLevelLayer::onEdit(sender);
        }

        sfx::altTabFix();
        FMODAudioEngine::get()->playEffect(path.string(), 1, 0, 0.7f);

        EditLevelLayer::onEdit(sender);
    }
};

class $modify(SFXEndLevelLayer, EndLevelLayer) {
    $override
    void onEdit(CCObject* sender) {
        EndLevelLayer::onEdit(sender);

        sfx::altTabFix();
        // @geode-ignore(unknown-resource)
        FMODAudioEngine::get()->playEffect("quitSound_01.ogg", 1, 0, 0.7f);
    }
};

class $modify(SFXPauseLayer, PauseLayer) {
    $override
    void onEdit(CCObject* sender) {
        PauseLayer::onEdit(sender);

        sfx::altTabFix();
        // @geode-ignore(unknown-resource)
        FMODAudioEngine::get()->playEffect("quitSound_01.ogg", 1, 0, 0.7f);
    }
};
