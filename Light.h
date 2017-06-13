#ifndef LIGHT_H
#define LIGHT_H

namespace Ogre { class SceneNode; }

namespace catapult {
  class CameraMan;
  class OgreFacade;
 
  /** Basic light that can follow the camera node. Creating a "proper" camera (attached to the same
      node as the light etc.) is too complex for this project. */
  class Light {
  public:
    Light(OgreFacade& ogre, const CameraMan& camera);
    void moveTo(const CameraMan& camera);
    
  private:
    Ogre::SceneNode* light;
  };
  
}
#endif 
