#ifndef MESHCONVERSIONS_H
#define MESHCONVERSIONS_H

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

#include <OgreVector3.h>

namespace Ogre { class Vector3; class Quaternion; class Mesh; }

namespace catapult {

  namespace MeshConversions {
    
  /** Create a mesh into the ogre scene. 
      Code obtained from http://www.ogre3d.org/tikiwiki/Generating+A+Mesh and other tutorials. */
  void createOgreMesh(const std::vector<uint32_t>& indices,
                      const std::vector<float>& vertices,
                      const std::string& name);

  /** Scan the Ogre mesh and fills the vector with vertices and face indices (triplets with the 3 vertices).
      Straight out of http://www.ogre3d.org/tikiwiki/RetrieveVertexData - I only adapted the signature to use
      vectors instead of arrays.*/
  void getMeshInformation(const Ogre::Mesh* const mesh,
                          const Ogre::Vector3& position,
                          const Ogre::Quaternion& orientation,
                          const Ogre::Vector3& scale,
                          std::vector<uint32_t>& indices,
                          std::vector<Ogre::Vector3>& vertices);  
  
  /** Returns the extremes axis aligned bounding box that contains all the vertices. */
  std::tuple<Ogre::Vector3, Ogre::Vector3> aabbCorners(const std::vector<Ogre::Vector3>& vertices);
  
  /** Returns the extremes axis aligned bounding box that contains all the vertices.
      The vertices array is the "fully-fledged" one (3 coordinates, 3 for normal, 2 for UV mapping. */
  std::tuple<Ogre::Vector3, Ogre::Vector3> aabbCorners(const std::vector<float>& vertices);
  
  }

}

#endif
