#include "CameraMan.h"
#include "DebugPanel.h"

#include <OgreCamera.h>

namespace catapult {
  
CameraMan::CameraMan(Ogre::SceneManager* sm) :
  camera(sm->createCamera("PlayerCam")),
  cameraMan(new OgreBites::SdkCameraMan(camera))
{
  camera->setPosition(Ogre::Vector3(0, 5, 58));  // A little behind the catapult.
  camera->lookAt(Ogre::Vector3(0,0,0));
  camera->setNearClipDistance(5);
}

const Ogre::Quaternion& CameraMan::orientation() const {
  return camera->getOrientation();
}

const Ogre::Vector3& CameraMan::position() const {
  return camera->getPosition();
}


bool CameraMan::keyPressed(const OIS::KeyEvent &arg) {
  
  if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
  {
    Ogre::String newVal;
    Ogre::PolygonMode pm;

    switch (camera->getPolygonMode())
    {
    case Ogre::PM_SOLID:
        newVal = "Wireframe";
        pm = Ogre::PM_WIREFRAME;
        break;
    case Ogre::PM_WIREFRAME:
        newVal = "Points";
        pm = Ogre::PM_POINTS;
        break;
    default:
        newVal = "Solid";
        pm = Ogre::PM_SOLID;
    }

  camera->setPolygonMode(pm);
  debugPanel->setFilteringMode(newVal);
  }
    
  /* The arrow key are used to aim the catapult, but also in the Ogre build-in camera.
      I don't know how to "remove" them from the camera, so I filter them "quick and dirty" here.*/
  if (arg.key != OIS::KC_UP &&
      arg.key != OIS::KC_DOWN &&
      arg.key != OIS::KC_LEFT &&
      arg.key != OIS::KC_RIGHT)
    cameraMan->injectKeyDown(arg);
  
  return true;
}


bool CameraMan::keyReleased(const OIS::KeyEvent &arg) {
  if (arg.key != OIS::KC_UP &&
    arg.key != OIS::KC_DOWN &&
    arg.key != OIS::KC_LEFT &&
    arg.key != OIS::KC_RIGHT)
    cameraMan->injectKeyUp(arg);
  
  return true;
}


bool CameraMan::mouseMoved( const OIS::MouseEvent &arg )
{
  cameraMan->injectPointerMove(arg);
  return true;
}


bool CameraMan::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
  cameraMan->injectPointerDown(arg, id);
  return true;
}


bool CameraMan::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
  cameraMan->injectPointerUp(arg, id);
  return true;
}

}