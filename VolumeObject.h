/** Wraps a voxel volume with some functions to manipulate it as required
 *  in the game.
 *  
 *  Voxel library taken from http://www.volumesoffun.com/polyvox-about/
 */

#ifndef VOLUMEOBJECT_H
#define VOLUMEOBJECT_H

#include <vector>
#include <cstdint>
#include <memory>

#include <OgreVector3.h>

#include "PolyVoxCore/SimpleVolume.h"

namespace catapult {
  
  class VolumeObject {
  public:
    /** Converts the given mesh in a volume (hopefully of the same shape). */
    VolumeObject(const std::vector<uint32_t>& indices,
                 const std::vector<Ogre::Vector3>& vertices);
    
    /** Compute the mesh representation of the volume. 
        The vertices are "unpacked": 6 consecutive values are 1 vertex (3 coordinates
        and 3 normal's components).*/
    void dumpMesh(std::vector<uint32_t>& indices,
                  std::vector<float>& vertices);
    
    /** Poor man's collision detection. Returns true if any voxel is full in the shpere 
        centered in the vector and of the given radius. */
    bool fullVoxelInRadius(const PolyVox::Vector3DInt32& centre, const uint32_t radius) const;
    
    
    /** Marks all the voxels in the sphere of given radius centered in the vector as empty. */
    void carveHole(const PolyVox::Vector3DInt32& centre, const uint32_t radius);
    
  private:
    /** "Standard value" that indicates the voxel is empty. */
    static const uint8_t emptyVoxel;
    
    /** "Standard value" that indicates the voxel is full. */
    static const uint8_t fullVoxel;
    
    /** Contains the actual voxels. 
        uint8_t is the "thing" we are storing in the 3D field (the voxel proper).*/
    std::unique_ptr< PolyVox::SimpleVolume<uint8_t> > rawVolume;
    
    PolyVox::Vector3DInt32 lowCorner;  ///< Limit for the PolyVox region.
    PolyVox::Vector3DInt32 topCorner;  ///< Limit for the PolyVox region.
    
    /** Fills the voxels that are inside the mesh. */
    void scanIntoVolume(const std::vector<uint32_t>& indices,
                        const std::vector<Ogre::Vector3>& vertices);
    
    /** Set all the voxel in the sphere at the given value. */
    void setSphere(const PolyVox::Vector3DInt32& centre, const uint32_t radius, const uint8_t value);
  };
  
}

#endif
