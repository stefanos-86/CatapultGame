#ifndef STONE_H
#define STONE_H

#include <OgreVector3.h>

namespace Ogre { class SceneNode; class SceneManager;}

namespace catapult {
  
  /** It is a flying sphere, follows the classical projectile motion.
   *  We call it "stone" because that is what we expect a catapult to throw. */
  class Stone
  {
  public:
    /** Size of the stone. Must be visible to the collision/carving algorithm. */
    static const float radius;
    
    /** Creates the stone with the given parameters for the parabolic motion computation. 
    Both parameters are supposed to be in world coordinates. The velocity should already be
    pointing in the "launch direction". */
    Stone(Ogre::SceneManager& sceneManager,
          const Ogre::Vector3& startingPosition,
          const Ogre::Vector3& initialVelocity
    );
    ~Stone();
    
    /** Computes the stone motion. */
    void advanceSimulation(const float elapsedSeconds);
    
    /** Returns the position of the stone. The "master data" is the Ogre node position. */
    const Ogre::Vector3 position() const;
    
  private:
    /** Reference kept to destroy the mesh at destruction time. */
    Ogre::SceneManager& scene;
    Ogre::SceneNode* mesh;
    
    /** Keeps track of the velocity at every simulation step.
        Ogre classes don't deal with motion, so this is the "master data" for it. */
    Ogre::Vector3 velocity;
  };

}
#endif 
