#ifndef CASTLE_H
#define CASTLE_H

#include "VolumeObject.h"

#include <memory>
#include <string>

#include <OISKeyboard.h>

namespace Ogre { class SceneManager; class SceneNode; class Vector3; }

namespace catapult {
  class Stone;

  /** Target for the catapult.
      Credits for the castle wall texture: https://pixabay.com/en/wall-stone-masonry-castle-wall-578204/
      The grass for the terrain: https://pixabay.com/en/meadow-253616/
  */
  class Castle : public OIS::KeyListener {
  public:
    /** Load the base mesh and extract/converts the data into the volume. */
    Castle(Ogre::SceneManager& sceneManager);
    
    /** The castle simulations are the impacts. */
    void advanceSimulation(Ogre::SceneManager& sceneManager, const Stone& flyingStone);
    
    // KeyListener
      virtual bool keyPressed(const OIS::KeyEvent& arg) final;
      virtual bool keyReleased( const OIS::KeyEvent& arg ) final { return true; };

  private:
    Ogre::SceneNode* volumeBasedMesh;
    Ogre::SceneNode* baseCastle;
    std::unique_ptr<VolumeObject> voxels;
    
    bool baseCastleVisibility;

    const std::string volumeMeshName;
    const std::string baseMeshName;
    
    /** Saves the material from the base castle to reuse the texture (if not the mapping) for the voxel-based mesh.*/
    std::string castleMaterialName;
    
    void buildMeshFromVolume(Ogre::SceneManager& sceneManager);
  
};

}

#endif
