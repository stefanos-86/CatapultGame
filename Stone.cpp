#include "Stone.h"

#include <OgreEntity.h>
#include <OgreSceneManager.h>


namespace catapult {
  
const float Stone::radius = 2.0;


Stone::Stone(Ogre::SceneManager& sceneManager, const Ogre::Vector3& startingPosition, const Ogre::Vector3& initialVelocity) :
  scene(sceneManager),
  velocity(initialVelocity)
{
  Ogre::Entity* stoneEntity = scene.createEntity(Ogre::SceneManager::PT_SPHERE);
  mesh = scene.getRootSceneNode()->createChildSceneNode();
  mesh->attachObject(stoneEntity);
  
  // Built in sphere is 100 units in diameter (http://www.ogre3d.org/forums/viewtopic.php?f=5&t=45368).
  // Scale to the desired size.
  const float scale = radius / (float) 50;
  mesh->scale(scale, scale, scale);
  
  mesh->setPosition(startingPosition);
}


Stone::~Stone() {
  scene.getRootSceneNode()->removeAndDestroyChild(mesh);
}


void Stone::advanceSimulation(const float elapsedSeconds) {
  const Ogre::Vector3 gravity(0, -9.81, 0);
  
  // Compute velocity/position at this instant from the previous values.
  // Parabolic motion for the (Z component of the) position.
  // Its derivative for the speed.
  mesh->setPosition(mesh->getPosition() +
                    velocity * elapsedSeconds +
                    0.5 * gravity * elapsedSeconds * elapsedSeconds);
  velocity = velocity + gravity * elapsedSeconds;
}

const Ogre::Vector3 Stone::position() const {
  return mesh->getPosition();
}


}