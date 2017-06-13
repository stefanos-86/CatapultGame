#include "VolumeObject.h"

#include <algorithm>
#include <tuple>
#include <vector>

//#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h" Kept for reference.
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/SimpleVolume.h"

#include "MeshConversions.h"


namespace catapult {

using namespace PolyVox;
using namespace Ogre;

/** "Epsilon" small value to check divison underflow. */
static const float SMALL_NUM = 0.00000001;

 /* Ray-Triangle intersection.
  * 
  * Taken from http://geomalgorithms.com/a06-_intersect-2.html#intersect3D_RayTriangle()
  * Adapted to use Ogre classes for geometry (as they are the "standard 3D vector" for this project)
  * and to be "C++-ish" (e. g. adding const where possible).
  * 
  * Can probably be optimized (e. g. pre-compute all the u, v, n) but this version is fast enough.
  
  Copyright 2001 softSurfer, 2012 Dan Sunday
  This code may be freely used and modified for any purpose 
  providing that this copyright notice is included with it.
  SoftSurfer makes no warranty for this code, and cannot be held
  liable for any real or imagined damage resulting from its use.
  Users of this code must verify correctness for their application.
 
   Input:  a ray R, and a triangle T
   Output: *I = intersection point (when it exists)
   Return: -1 = triangle is degenerate (a segment or point)
            0 =  disjoint (no intersect)
            1 =  intersect in unique point I1
            2 =  are in the same plane
*/
static int intersect3D_RayTriangle( const std::tuple<Vector3, Vector3>& R,
                                    const std::tuple<Vector3, Vector3, Vector3>& T,
                                    Vector3& I )
{
    const Vector3& V0 = std::get<0>(T);
    const Vector3& V1 = std::get<1>(T);
    const Vector3& V2 = std::get<2>(T);
    
    // get triangle edge vectors and plane normal    
    const Vector3 u = V1 - V0;
    const Vector3 v = V2 - V0;
    const Vector3 n = u.crossProduct(v);
    
    if (n.isZeroLength())  // triangle is degenerate
        return -1;         // do not deal with this case

    const Vector3& P0 = std::get<0>(R);
    const Vector3& P1 = std::get<1>(R);
    
    // Ray vectors
    const Vector3 dir = P1 - P0; // ray direction vector          
    const Vector3 w0 = V0 - P0;

    // Ray-plane intersection parameters.
    const double a = n.dotProduct(w0);
    const double b = n.dotProduct(dir);
    
    if (fabs(b) < SMALL_NUM) { // ray is  parallel to triangle plane
        if (a == 0)            //   ray lies in triangle plane
            return 2;
        else return 0;          //  ray disjoint from plane
    }

    // get intersect point of ray with triangle plane
    const double r = a / b;
    
    if (r < 0.0) // ray goes away from triangle
      return 0;  // => no intersect
    // for a segment, also test if (r > 1.0) => no intersect - not needed in this project.

    // intersect point of ray and plane
    I = P0 + (r * dir);

    // is I inside T?
    const float uu = u.dotProduct(u);
    const float uv = u.dotProduct(v);
    const float vv = v.dotProduct(v);
    const Vector3 w = I - V0;
    const float wu = w.dotProduct(u);
    const float wv = w.dotProduct(v);
    const float D = uv * uv - uu * vv;

    // get and test parametric coords
    const float s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)         // I is outside T
     return 0;
    
    const float t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)  // I is outside T
        return 0;

    return 1;                       // I is in T
}


/** Returns true if the given point is inside the mesh.
    Shoot a ray from the point: if it crosses an odd number of faces, it is inside.*/
static bool pointInsideMesh(const Vector3& point,
                            const std::vector<uint32_t>& indices,
                            const std::vector<Vector3>& vertices)
{  
    // Define the ray for this point. In theory The direction does not matter, so the ray goes straight up.
    // In practice there are approximations and the results depend (a lot!) on the direction.
    // There must be a smarter way...
    const Vector3 rayDirection(0, 1, 0);
    
    const uint32_t faceCount = indices.size() / 3;
    uint32_t intersectionsWithFaces = 0;
     
    for (uint32_t face = 0; face < faceCount; ++face) {
      const uint32_t baseForFace = face * 3;
      const size_t indexV0 = indices.at(baseForFace);
      const size_t indexV1 = indices.at(baseForFace + 1);
      const size_t indexV2 = indices.at(baseForFace + 2);
      
      const auto& V0 = vertices.at(indexV0);
      const auto& V1 = vertices.at(indexV1);
      const auto& V2 = vertices.at(indexV2);
  
      Vector3 intersectionPointIDoNotCareAbout;  //But the code of the library computes it anyway...
      
      if (1 == intersect3D_RayTriangle(std::make_tuple(point, rayDirection + point),
                                       std::make_tuple(V0, V1, V2),
                                       intersectionPointIDoNotCareAbout))
        intersectionsWithFaces++;
      
    }
    
    return (intersectionsWithFaces % 2) == 1;
}


/** Returns the extremes for the volume so that the volume contains all the vertices. */
static std::tuple<Vector3DInt32, Vector3DInt32> volumeCorners(const std::vector<Vector3>& vertices) {
  const auto corners = MeshConversions::aabbCorners(vertices);
  
  const auto& lowFloatCorner = std::get<0>(corners);
  const auto& topFloatCorner = std::get<1>(corners);
  Vector3DInt32 lowCorner(lowFloatCorner.x,
                          lowFloatCorner.y,
                          lowFloatCorner.z);
  Vector3DInt32 topCorner(topFloatCorner.x,
                          topFloatCorner.y,
                          topFloatCorner.z);
  
  // The above code pushes float coordinates into integer vectors.
  // Push the limits "1 unit away" to be sure that the mesh fits inside even if the decimals are truncated.
  lowCorner -= Vector3DInt32(1, 1, 1);
  topCorner += Vector3DInt32(1, 1, 1);
  
  return std::make_tuple(lowCorner, topCorner);
}


const uint8_t VolumeObject::emptyVoxel = 0;
const uint8_t VolumeObject::fullVoxel = 255;

VolumeObject::VolumeObject(const std::vector<uint32_t>& indices,
                           const std::vector<Vector3>& vertices) {
  scanIntoVolume(indices, vertices);
}


void VolumeObject::dumpMesh(std::vector<uint32_t>& indices,
                            std::vector<float>& vertices){
  vertices.clear();
  indices.clear();
  
  SurfaceMesh<PositionMaterialNormal> mesh;
  MarchingCubesSurfaceExtractor< SimpleVolume<uint8_t> > surfaceExtractor(rawVolume.get(), rawVolume->getEnclosingRegion(), &mesh);
  
  // CubicSurfaceExtractorWithNormals< SimpleVolume<uint8_t> > surfaceExtractor(rawVolume.get(), rawVolume->getEnclosingRegion(), &mesh);
  // Can't be used anymore: there is a core dump somewhere (I suspect incorrect enclosing region).
  
  surfaceExtractor.execute();
  
  // Ogre and PolyVox indices have the same format.
  std::copy(mesh.getIndices().begin(), mesh.getIndices().end(), std::back_inserter(indices));
  
  for (const PositionMaterialNormal& vertex: mesh.getVertices()) {
    
    // From some experiments, it seems that the extractor uses only the positive octant for the mesh, no matter where
    // the region is defined. Vertices always have positive coordinates.
    // The vertices must be translated so that the "orgin" is the lower corner of the region.
    // The additional "half cube" accounts for the cube thickness (voxels are centered around their integer coordinates).
    vertices.push_back(vertex.getPosition().getX() + lowCorner.getX() - 0.5);
    vertices.push_back(vertex.getPosition().getY() + lowCorner.getY() - 0.5);
    vertices.push_back(vertex.getPosition().getZ() + lowCorner.getZ() - 0.5);
    
    // Normals don't change with translations - a simple copy is enough.
    vertices.push_back(vertex.getNormal().getX());
    vertices.push_back(vertex.getNormal().getY());
    vertices.push_back(vertex.getNormal().getZ());
    
    // Pretend those are meaningful UV coords. Pretend they work with any mesh...
    // It is a flat projection - the side of the castle will look awful.
    // The texture scale is defined in the blender-exported material.
    // Trying more complicated algorithms is "too much" for this project.
    vertices.push_back(vertex.getPosition().getX());
    vertices.push_back(vertex.getPosition().getY());
  }
}


bool VolumeObject::fullVoxelInRadius(const PolyVox::Vector3DInt32& center, const uint32_t radius) const {
  for (auto x = lowCorner.getX(); x < topCorner.getX(); ++x)
    for (auto y = lowCorner.getY(); y < topCorner.getY(); ++y)
      for (auto z = lowCorner.getZ(); z < topCorner.getZ(); ++z) {
          const Vector3DInt32 currentVoxel(x,y,z);
          //And compute how far the current position is from the center of the "scanned volume".
          float distance = (currentVoxel - center).length();

          if (distance <= radius) {
            if (rawVolume->getVoxelAt(currentVoxel) > emptyVoxel)
              return true;
          }
      }
  
  return false;
}

void VolumeObject::carveHole(const PolyVox::Vector3DInt32& centre, const uint32_t radius) {
  setSphere(centre, radius, emptyVoxel);
}


void VolumeObject::scanIntoVolume(const std::vector< uint32_t >& indices,
                                  const std::vector< Vector3 >& vertices) {
  // How big a volume do we need?
  const std::tuple<Vector3DInt32, Vector3DInt32> extremes = volumeCorners(vertices);
  lowCorner = std::get<0>(extremes);
  topCorner = std::get<1>(extremes);
  
  // For reasons I could not ascertain, the lowest limit of the region are not visited.
  // If the "bottom" of the volume is full, the extractor won't "see" it and add it to the mesh
  // (which will look like it is "open" on that side). Probably depends on how PolyVox condiders
  // the region limits. So I add another "slice" to the volume that won't be ever used (and so is never full).
  rawVolume.reset(
    new SimpleVolume<uint8_t> (PolyVox::Region(lowCorner - Vector3DInt32(1, 1, 1), topCorner))
  );
  
  for (auto x = lowCorner.getX() ; x < topCorner.getX(); ++x)
    for (auto y = lowCorner.getY(); y < topCorner.getY(); ++y)
      for (auto z = lowCorner.getZ(); z < topCorner.getZ(); ++z) {
         Vector3 voxelCentre(x + 0.5, y + 0.5, z + 0.5);
         if (pointInsideMesh(voxelCentre, indices, vertices))
           rawVolume->setVoxelAt(x, y, z, fullVoxel);
      }
}


void VolumeObject::setSphere(const PolyVox::Vector3DInt32& centre, const uint32_t radius, const uint8_t value) {
  for (auto x = lowCorner.getX(); x < topCorner.getX(); ++x)
    for (auto y = lowCorner.getY(); y < topCorner.getY(); ++y)
      for (auto z = lowCorner.getZ(); z < topCorner.getZ(); ++z) {
        const Vector3DInt32 currentVoxel(x, y, z);
        float distance = (currentVoxel - centre).length();

        if (distance <= radius)
          rawVolume->setVoxelAt(currentVoxel, value);
      }
}


}

