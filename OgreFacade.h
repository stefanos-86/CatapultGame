#ifndef OGREFACADE_H
#define OGREFACADE_H

#include <memory>

#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreWindowEventUtilities.h>  // This class "handles" the window directly.
#include <SdkTrays.h> // Also has the OverlaySystem.

#include "InputFacade.h"

namespace Ogre { class Camera; }
namespace OgreBites { class InputContext; }

namespace catapult {
  
  /** Collects all the "structural" Ogre objects. 
   *  Other parts of the project may still use bits of the Ogre libraries. They are taken as "standard"
      in this project. There is no reason to completely encapsulate them.
      Some of the objects are even kept there as public members. */
  class OgreFacade : public Ogre::WindowEventListener
  {
  public:
    OgreFacade();
    ~OgreFacade();
    
    /** Returns true if it could open the window. */
    bool configureWindow();
    
    OgreBites::InputContext prepareInputSystem(OIS::KeyListener* keys, OIS::MouseListener* mice);
    
    /** Do not call before the camera creation. Core dump otherwise.
        There must be a coupling inside Ogre between the camera "stuff" and whatever this does.
        It "works enough", I won't check the details.*/
    void finalizeResources();
    
    void createViewports(Ogre::Camera* camera);
    
    void recaptureInputDevices();
    
    // Ogre::WindowEventListener
      /// Adjust mouse clipping area
      virtual void windowResized(Ogre::RenderWindow* rw) final;
      
      /// Unattach OIS before window shutdown (very important under Linux)
      virtual void windowClosed(Ogre::RenderWindow* rw) final;
      
      
    
    std::unique_ptr<Ogre::Root> root;
    Ogre::RenderWindow* window;
    Ogre::OverlaySystem* overlay;
    Ogre::SceneManager* scene;
    
    InputFacade inputSystem;
    
  private:
    void setupResources(void);
  };


}
#endif
