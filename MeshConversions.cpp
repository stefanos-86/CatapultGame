#include "MeshConversions.h"


#include <OgreHardwareBufferManager.h>
#include <OgreMaterialManager.h>
#include <OgreMeshManager.h>
#include <OgreRoot.h>
#include <OgreSubMesh.h>


namespace catapult {

namespace MeshConversions {
    
void createOgreMesh(const std::vector<uint32_t>& indices,
                    const std::vector<float>& vertices,
                    const std::string& name)
{
    using namespace Ogre;
    
    // Create the mesh via the MeshManager
    MeshPtr msh = MeshManager::getSingleton().createManual(name, "General");
 
    // Create one submesh
    SubMesh* sub = msh->createSubMesh();
 
    // Define the vertices - each has 3 floats for position, 3 for normal and 2 for the UV mapping.
    const size_t nVertices = vertices.size() / 8;
    
    /* Do not use a color buffer - have textures and UV coordinates.
     * Keep the code for reference.
    RenderSystem* rs = Root::getSingleton().getRenderSystem();
    RGBA colours[nVertices];
    RGBA *pColour = colours;
    // Use render system to convert colour value since colour packing varies
    rs->convertColourValue(ColourValue(1.0,0.0,0.0), pColour++); //0 colour */
    
    msh->sharedVertexData = new VertexData();  // TODO: leak?
    msh->sharedVertexData->vertexCount = nVertices;
 
    // Create declaration (memory format) of vertex data
    VertexDeclaration* decl = msh->sharedVertexData->vertexDeclaration;
    size_t offset = 0;
    // 1st buffer
    decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES);
    offset += VertexElement::getTypeSize(VET_FLOAT2);
       
    // Allocate vertex buffer of the requested number of vertices (vertexCount) 
    // and bytes per vertex (offset)
    HardwareVertexBufferSharedPtr vbuf = 
        HardwareBufferManager::getSingleton().createVertexBuffer(
        offset, msh->sharedVertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    
    // Upload the vertex data to the card
    vbuf->writeData(0, vbuf->getSizeInBytes(), &vertices[0], true);
 
    // Set vertex buffer binding so buffer 0 is bound to our vertex buffer
    VertexBufferBinding* bind = msh->sharedVertexData->vertexBufferBinding; 
    bind->setBinding(0, vbuf);
 
    /* 2nd buffer - originally the color buffer. No more in use, but kept for reference.
    offset = 0;
    decl->addElement(1, offset, VET_COLOUR, VES_DIFFUSE);
    offset += VertexElement::getTypeSize(VET_COLOUR);
    
    // Allocate vertex buffer of the requested number of vertices (vertexCount) 
    // and bytes per vertex (offset)
    vbuf = HardwareBufferManager::getSingleton().createVertexBuffer(
        offset, msh->sharedVertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    
    // Upload the vertex data to the card
    vbuf->writeData(0, vbuf->getSizeInBytes(), colours, true);
 
    // Set vertex buffer binding so buffer 1 is bound to our colour buffer
    bind->setBinding(1, vbuf);*/
    
 
    // Allocate index buffer of the requested number of vertices
    HardwareIndexBufferSharedPtr ibuf = HardwareBufferManager::getSingleton().
        createIndexBuffer(
        HardwareIndexBuffer::IT_32BIT, 
        indices.size(), 
        HardwareBuffer::HBU_STATIC_WRITE_ONLY);
 
    // Upload the index data to the card
    ibuf->writeData(0, ibuf->getSizeInBytes(), &indices[0], true);
 
    // Set parameters of the submesh
    sub->useSharedVertices = true;
    sub->indexData->indexBuffer = ibuf;
    sub->indexData->indexCount = indices.size();
    sub->indexData->indexStart = 0;
 
    // Set bounding information (for culling); should we need to optimize, we can reuse the aabb of the base castle.
    const auto& corners = aabbCorners(vertices);
    const auto& lowCorner = std::get<0>(corners);
    const auto& topCorner = std::get<1>(corners);
    msh->_setBounds(AxisAlignedBox(lowCorner, topCorner));
    msh->_setBoundingSphereRadius(0.5 * (topCorner - lowCorner).length());  // Half the diagonal of the aabb.
 
    // Notify - Mesh object that it has been loaded
    msh->load();
}


void getMeshInformation(const Ogre::Mesh* const mesh,
                        const Ogre::Vector3& position,
                        const Ogre::Quaternion& orientation,
                        const Ogre::Vector3& scale,
                        std::vector<uint32_t>& indices,
                        std::vector<Ogre::Vector3>& vertices)
{
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;
 
    uint32_t vertex_count, index_count;
    vertex_count = index_count = 0;
 
    // Calculate how many vertices and indices we're going to need
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !added_shared )
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }
        // Add the indices
        index_count += submesh->indexData->indexCount;
    }
 
    // Allocate space for the vertices and indices
    vertices.clear();
    indices.clear();
    vertices.resize(vertex_count);
    indices.resize(index_count);
 
    added_shared = false;
 
    // Run through the submeshes again, adding the data into the arrays
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
 
        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
 
        if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }
 
            const Ogre::VertexElement* posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
 
            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
 
            unsigned char* vertex =
                static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
 
            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //Ogre::Real* pReal;
            float* pReal;
 
            for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);
                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
                vertices[current_offset + j] = (orientation * (pt * scale)) + position;
            }
 
            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }
 
        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
 
        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
 
        unsigned long* pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);
 
        size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;
 
        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }
        }
        else
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
                                          static_cast<unsigned long>(offset);
            }
        }
 
        ibuf->unlock();
        current_offset = next_offset;
    }
}


std::tuple<Ogre::Vector3, Ogre::Vector3> aabbCorners(const std::vector<float>& vertices) {
  Ogre::Vector3 lowCorner(0, 0, 0);
  Ogre::Vector3 topCorner(0, 0, 0);
  
  for (size_t vertex = 0; vertex < vertices.size(); vertex += 8){
    const float x = vertices[vertex];
    const float y = vertices[vertex + 1];
    const float z = vertices[vertex + 2];
    
    if (x < lowCorner.x)
      lowCorner.x = x;
    if (y < lowCorner.y)
      lowCorner.y = y;
    if (z < lowCorner.z)
      lowCorner.z = z;
    
    if (x > topCorner.x)
      topCorner.x = x;
    if (y > topCorner.y)
      topCorner.y = y;
    if (z > topCorner.z)
      topCorner.z = z;
  }
  
  
  return std::make_tuple(lowCorner, topCorner);
}


std::tuple<Ogre::Vector3, Ogre::Vector3> aabbCorners(const std::vector<Ogre::Vector3>& vertices) {
  Ogre::Vector3 lowCorner(0, 0, 0);
  Ogre::Vector3 topCorner(0, 0, 0);
  
  for (const Ogre::Vector3& v: vertices) {
    if (v.x < lowCorner.x)
      lowCorner.x = v.x;
    if (v.y < lowCorner.y)
      lowCorner.y = v.y;
    if (v.z < lowCorner.z)
      lowCorner.z = v.z;
    
    if (v.x > topCorner.x)
      topCorner.x = v.x;
    if (v.y > topCorner.y)
      topCorner.y = v.y;
    if (v.z > topCorner.z)
      topCorner.z = v.z;
  }
  
  
  return std::make_tuple(lowCorner, topCorner);
}

    
}}