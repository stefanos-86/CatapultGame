#ifndef CAMERAMAN_H
#define CAMERAMAN_H

#include <memory.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>  // Needs all that stuff included before...

namespace Ogre { class Quaternion; class SceneManager; class Vector3; }

namespace catapult {
  class DebugPanel;

class CameraMan : public OIS::KeyListener, public OIS::MouseListener
{
public:
  CameraMan(Ogre::SceneManager* sm);
  
  const Ogre::Quaternion& orientation() const;
  const Ogre::Vector3& position() const;
  
  virtual bool keyPressed(const OIS::KeyEvent& arg) final;
  virtual bool keyReleased(const OIS::KeyEvent& arg) final;
  
  virtual bool mouseMoved(const OIS::MouseEvent& arg) final;
  virtual bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id) final;
  virtual bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id) final;
  
  Ogre::Camera* camera;
  std::unique_ptr<OgreBites::SdkCameraMan> cameraMan;
  DebugPanel* debugPanel; ///< Have to write on this. May want to decouple in the future...
  
  private:
};

}

#endif
