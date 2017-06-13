#include "Light.h"

#include <OgreRoot.h>

#include "CameraMan.h"
#include "OgreFacade.h"

namespace catapult {
  
Light::Light(OgreFacade& ogre, const CameraMan& camera) :
  light(ogre.scene->getRootSceneNode()->createChildSceneNode())
{
    ogre.scene->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
    Ogre::Light* lightObject = ogre.scene->createLight();
    light->attachObject(lightObject);
    moveTo(camera);
}

void Light::moveTo(const CameraMan& camera) {
  light->setPosition(camera.position());
}


}