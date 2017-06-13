#include "Castle.h"

#include <cmath>

#include <OgreEntity.h>
#include <OgreRoot.h>
#include <OgreMeshManager.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>

#include "MeshConversions.h"
#include "Stone.h"

namespace catapult {

Castle::Castle(Ogre::SceneManager& sceneManager) :
  volumeBasedMesh(nullptr),
  voxels(nullptr),
  baseCastleVisibility(false),
  volumeMeshName("CastleVolume"),
  baseMeshName("Castle.mesh") // Has to match the file name... 
{
  using namespace Ogre;
  
  Ogre::Entity* ogreEntity = sceneManager.createEntity(baseMeshName, "General");
  baseCastle = sceneManager.getRootSceneNode()->createChildSceneNode();
  baseCastle->attachObject(ogreEntity);
  baseCastle->setVisible(baseCastleVisibility);
  
  castleMaterialName = ogreEntity->getSubEntity(0)->getMaterialName();
  
  Ogre::MeshPtr meshPointer = MeshManager::getSingleton().getByName(baseMeshName);
  std::vector<Vector3> vertices;
  std::vector<uint32_t> indices;
  
  MeshConversions::getMeshInformation(meshPointer.get(),
            baseCastle->getPosition(),
            baseCastle->getOrientation(),
            baseCastle->getScale(),
            indices,
            vertices
            ); 
  
  voxels.reset( new VolumeObject(indices, vertices) );
  
  buildMeshFromVolume(sceneManager);
  
  // Also load the terrain "as if" it is part of the castle.
  // It is not worth to put it in a standalone object - it is never touched again.
  Ogre::Entity* terrainEntity = sceneManager.createEntity("Terrain.mesh", "General");
  Ogre::SceneNode* terrainNode = sceneManager.getRootSceneNode()->createChildSceneNode();
  terrainNode->attachObject(terrainEntity);
}


void Castle::buildMeshFromVolume(Ogre::SceneManager& sceneManager) {
  using namespace Ogre;
  
  if (volumeBasedMesh) {  // Must remove the old one. I suspect leaks here...
    sceneManager.destroyEntity(dynamic_cast<Entity*>(volumeBasedMesh->getAttachedObject(0)));
      sceneManager.getRootSceneNode()->removeAndDestroyChild(volumeBasedMesh);
    MeshManager::getSingleton().remove(volumeMeshName);
    volumeBasedMesh = nullptr;
  }
  
  std::vector<uint32_t> indices;
  std::vector<float> vertices;
  voxels->dumpMesh(indices, vertices);
   
  MeshConversions::createOgreMesh(indices, vertices, volumeMeshName);

  Entity* thisEntity = sceneManager.createEntity(volumeMeshName, "General");
  thisEntity->setMaterialName(castleMaterialName);
  SceneNode* thisSceneNode = sceneManager.getRootSceneNode()->createChildSceneNode();
  thisSceneNode->setPosition(baseCastle->getPosition());
  thisSceneNode->setOrientation(baseCastle->getOrientation());
  thisSceneNode->setScale(baseCastle->getScale());
  thisSceneNode->attachObject(thisEntity);
  
  volumeBasedMesh = thisSceneNode;
}


void Castle::advanceSimulation(Ogre::SceneManager& sceneManager, const Stone& flyingStone) {  
  const Ogre::Vector3 stoneInVolumeCoordinates = volumeBasedMesh->_getFullTransform().inverse() * flyingStone.position();
  
  const int stoneSize = static_cast<int>(flyingStone.radius);
  const PolyVox::Vector3DInt32 stoneIntegerPosition(std::round(stoneInVolumeCoordinates.x),
                                                    std::round(stoneInVolumeCoordinates.y),
                                                    std::round(stoneInVolumeCoordinates.z));
  // Not the most efficient thing, probably. Could be optimized with aabb checks, make carveHole return
  // true if the mesh needs refreshing etc. It is fast enough for this project.
  if (voxels->fullVoxelInRadius(stoneIntegerPosition, stoneSize)) {
    voxels->carveHole(stoneIntegerPosition, stoneSize);
    buildMeshFromVolume(sceneManager);
  }
}


bool Castle::keyPressed(const OIS::KeyEvent& arg) {
  if (arg.key == OIS::KC_V) {
    baseCastleVisibility = ! baseCastleVisibility;
    baseCastle->setVisible(baseCastleVisibility);
  }
  
  return true;
}



}
