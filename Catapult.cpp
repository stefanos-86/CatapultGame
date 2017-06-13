#include "Catapult.h"

#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

namespace catapult {
  
Catapult::Catapult(Ogre::SceneManager& sceneManager) :
  inFligthStone(nullptr),
  shotRequested(false),
  sceneManager(sceneManager),
  aimingRotationRadSeconds(2 * 3.14 / 30), // Approximatively 10 degree/second.
  currentElevationMotion(Elevation::HALT),
  currentTraverseMotion(Traverse::HALT)
{    
    Ogre::Entity* ogreEntity = sceneManager.createEntity("Catapult.mesh", "General");
    ogreNode = sceneManager.getRootSceneNode()->createChildSceneNode();
    ogreNode->attachObject(ogreEntity);
    ogreNode->setPosition(0, 0, 45);
    // Does any of the above require destruction???
    
}


bool Catapult::keyPressed(const OIS::KeyEvent& arg) {
  switch (arg.key) {
    case OIS::KC_SPACE:
      shotRequested = true;
      break;
      
    case OIS::KC_LEFT:
      currentTraverseMotion = Traverse::LEFT;
      break;
      
    case OIS::KC_RIGHT:
      currentTraverseMotion = Traverse::RIGHT;
      break;
      
    case OIS::KC_UP:
      currentElevationMotion = Elevation::UP;
      break;
      
    case OIS::KC_DOWN:
      currentElevationMotion = Elevation::DOWN;
      break;
      
    default: ;
      // Do nothing.
  }
  
  return true;
}


bool Catapult::keyReleased(const OIS::KeyEvent& arg) {
  switch (arg.key) {
    case OIS::KC_LEFT:
    case OIS::KC_RIGHT:
      currentTraverseMotion = Traverse::HALT;
      break;
      
    case OIS::KC_UP:
    case OIS::KC_DOWN:
      currentElevationMotion = Elevation::HALT;
      break;
      
    default: ;
      // Do nothing.
  }
  
  return true;
}




void Catapult::advanceSimulation(const float elapsedSeconds) {
  if (shotRequested) {
    shotRequested = false;
    
    // To keep the catapulr as level as possible I "pitch up" the shot angle.
    const Ogre::Quaternion extraElevation(Ogre::Degree(30), Ogre::Vector3::UNIT_X);
    
    const Ogre::Vector3 baseVelocity(0, 0, -25); // Apparently "-Z" is the forward direction (e. g. when applied to the camera).
    inFligthStone.reset(new Stone(sceneManager,
                                  ogreNode->getPosition(),                      // Start where the catapult is.
                                  ogreNode->getOrientation() * extraElevation* baseVelocity)    // Shoot "forward" with respect to the catapult.
    );
  }
  
  if (inFligthStone)
    inFligthStone->advanceSimulation(elapsedSeconds);
  
  traverse(elapsedSeconds);
  elevate(elapsedSeconds);
}


void Catapult::elevate(const float elapsedSeconds) {
  const Ogre::Vector3 axis(1, 0, 0);
    
  if (currentElevationMotion == Elevation::UP)
    ogreNode->rotate(axis, aimingRotationRadSeconds * elapsedSeconds);
  else if (currentElevationMotion == Elevation::DOWN)
    ogreNode->rotate(axis, -aimingRotationRadSeconds * elapsedSeconds);
}


void Catapult::traverse(const float elapsedSeconds) {
  const Ogre::Vector3 axis(0, 1, 0);
  
  // Using world coordinates make it turn "flat", as if mounted on a rotating platform.
  // Rotations relative to the object look like an aircraft roll.
  if (currentTraverseMotion == Traverse::LEFT)
    ogreNode->rotate(axis, aimingRotationRadSeconds * elapsedSeconds, Ogre::Node::TS_WORLD);
  else if (currentTraverseMotion == Traverse::RIGHT)
    ogreNode->rotate(axis, -aimingRotationRadSeconds * elapsedSeconds, Ogre::Node::TS_WORLD);
}


}