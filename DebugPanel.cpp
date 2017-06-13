#include "DebugPanel.h"

#include <OgreMaterialManager.h>

namespace catapult {
  
  DebugPanel::DebugPanel() :
    trayManager(nullptr),
    detailsPanel(nullptr)
  {}
  
void DebugPanel::initialize(Ogre::RenderWindow* window, OgreBites::InputContext& ic) {
  
    trayManager.reset(new OgreBites::SdkTrayManager("InterfaceName", window, ic, this));
    trayManager->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    trayManager->showLogo(OgreBites::TL_BOTTOMRIGHT);
    trayManager->hideCursor();
    
    // create a params panel for displaying sample details
    Ogre::StringVector items;
    items.push_back("cam.pX");
    items.push_back("cam.pY");
    items.push_back("cam.pZ");
    items.push_back("");
    items.push_back("cam.oW");
    items.push_back("cam.oX");
    items.push_back("cam.oY");
    items.push_back("cam.oZ");
    items.push_back("");
    items.push_back("Filtering");
    items.push_back("Poly Mode");

    detailsPanel = trayManager->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 200, items);
    detailsPanel->setParamValue(9, "Bilinear");
    detailsPanel->setParamValue(10, "Solid");
    detailsPanel->hide();
}

void DebugPanel::setFilteringMode(const Ogre::String& newVal) {
  detailsPanel->setParamValue(10, newVal);
}

void DebugPanel::readCameraPosition(const Ogre::Camera* camera) {
  if (detailsPanel->isVisible())   // if details panel is visible, then update its contents
  {
    detailsPanel->setParamValue(0, Ogre::StringConverter::toString(camera->getDerivedPosition().x));
    detailsPanel->setParamValue(1, Ogre::StringConverter::toString(camera->getDerivedPosition().y));
    detailsPanel->setParamValue(2, Ogre::StringConverter::toString(camera->getDerivedPosition().z));
    detailsPanel->setParamValue(4, Ogre::StringConverter::toString(camera->getDerivedOrientation().w));
    detailsPanel->setParamValue(5, Ogre::StringConverter::toString(camera->getDerivedOrientation().x));
    detailsPanel->setParamValue(6, Ogre::StringConverter::toString(camera->getDerivedOrientation().y));
    detailsPanel->setParamValue(7, Ogre::StringConverter::toString(camera->getDerivedOrientation().z));
  }
  
}

      
bool DebugPanel::keyPressed(const OIS::KeyEvent& arg) {
  if (trayManager->isDialogVisible()) return true;   // don't process any more keys if dialog is up

      if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
      {
        trayManager->toggleAdvancedFrameStats();
      }
      
      else if (arg.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
      {
        if (detailsPanel->getTrayLocation() == OgreBites::TL_NONE)
      {
          trayManager->moveWidgetToTray(detailsPanel, OgreBites::TL_TOPRIGHT, 0);
          detailsPanel->show();
      }
      else
      {
          trayManager->removeWidgetFromTray(detailsPanel);
          detailsPanel->hide();
      }
      }
      else if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
      {
      Ogre::String newVal;
      Ogre::TextureFilterOptions tfo;
      unsigned int aniso;

      switch (detailsPanel->getParamValue(9).asUTF8()[0])
      {
      case 'B':
          newVal = "Trilinear";
          tfo = Ogre::TFO_TRILINEAR;
          aniso = 1;
          break;
      case 'T':
          newVal = "Anisotropic";
          tfo = Ogre::TFO_ANISOTROPIC;
          aniso = 8;
          break;
      case 'A':
          newVal = "None";
          tfo = Ogre::TFO_NONE;
          aniso = 1;
          break;
      default:
          newVal = "Bilinear";
          tfo = Ogre::TFO_BILINEAR;
          aniso = 1;
      }

      Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
      Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
      detailsPanel->setParamValue(9, newVal);
      }
  return true;
}
  
} // namespace