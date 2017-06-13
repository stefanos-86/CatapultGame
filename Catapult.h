#ifndef CATAPULT_H
#define CATAPULT_H

#include <memory>

#include <OgreMath.h>
#include <OISKeyboard.h>

#include "Stone.h"

namespace Ogre { class SceneManager; class SceneNode;}

namespace catapult {
  class Stone;
  class CameraMan;

  class Catapult : public OIS::KeyListener
  {
    
    
  public:
    Catapult(Ogre::SceneManager& sceneManager);
    
    /*KeyListener*/
    bool keyPressed(const OIS::KeyEvent &arg) final;
    bool keyReleased(const OIS::KeyEvent &arg ) final;
    
    void advanceSimulation(const float elapsedSeconds);
    
    std::unique_ptr<Stone> inFligthStone;
    
  private:
    /** Shorthand constants for aiming. */
    enum class Elevation{UP, DOWN, HALT};
    enum class Traverse {LEFT, RIGHT, HALT};
    
    /** Turns up and down the catapult.
        This is somewhat inelegant (the wheels should stay on the ground...) but it is a cheap
        way to aim it.*/
    void elevate(const float elapsedSeconds);
    
    /** Dual of elevate, for horizontal aiming. */
    void traverse(const float elapsedSeconds);
    
    bool shotRequested;
    Ogre::SceneManager& sceneManager;
    
    /** The scene node holds the visible Ogre object, but its position/orientation also
        count for the simulation.*/
    Ogre::SceneNode* ogreNode;
    
    /** Elevation and traverse speed */
    const Ogre::Radian aimingRotationRadSeconds;
    
    Elevation currentElevationMotion;
    Traverse currentTraverseMotion;
  };

}
#endif
