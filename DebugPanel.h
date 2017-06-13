#ifndef DEBUGPANEL_H
#define DEBUGPANEL_H

#include <memory>

#include <OISKeyboard.h>
#include <SdkTrays.h>

namespace Ogre { class RenderWindow; class InputContext; class Camera; } 

namespace catapult {
  
  /** Most of the code comes from the debug panel of the Ogre tutorials. 
      I touched only "just enough" to pull it in its own class. */
  class DebugPanel : public OIS::KeyListener, public OgreBites::SdkTrayListener
  {
  public: 
    DebugPanel();
    
    void initialize(Ogre::RenderWindow* window, OgreBites::InputContext& ic);
    
    void setFilteringMode(const Ogre::String& newVal);
    void readCameraPosition(const Ogre::Camera* camera);
    
    /*KeyListener*/
      virtual bool keyPressed(const OIS::KeyEvent &arg) final;
      virtual bool keyReleased(const OIS::KeyEvent &arg ) final { return true; }
      
          
    std::unique_ptr<OgreBites::SdkTrayManager> trayManager;
    OgreBites::ParamsPanel* detailsPanel;
  };
}

#endif
