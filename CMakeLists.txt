# Derived from the tutorial application file.
# Use as cmake -DCMAKE_BUILD_TYPE=Release . (because Ogre was built in release mode).

cmake_minimum_required(VERSION 2.6)
 
project(CatapultGame)
 
if( NOT UNIX )
  message( FATAL_ERROR "This game is only for linux." )
endif()

if(EXISTS "/usr/local/lib/OGRE/cmake")

  set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
  set(OGRE_SAMPLES_INCLUDEPATH "/usr/local/share/OGRE/samples/Common/include/") # We could just *assume* that developers uses this basepath : /usr/local

elseif(EXISTS "/usr/lib/OGRE/cmake")

  set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
  set(OGRE_SAMPLES_INCLUDEPATH "/usr/share/OGRE/samples/Common/include/") # Otherwise, this one

else ()
  message(SEND_ERROR "Failed to find module path.")
endif(EXISTS "/usr/local/lib/OGRE/cmake")
  
 
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")
 
find_package(OGRE REQUIRED) 
find_package(OIS REQUIRED)
 
# Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
set(Boost_ADDITIONAL_VERSIONS "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40" "1.39.0" "1.39" "1.38.0" "1.38" "1.37.0" "1.37" )
# Components that need linking (NB does not include header-only components like bind)
set(OGRE_BOOST_COMPONENTS thread date_time)
find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
  if (NOT Boost_FOUND)
      # Try again with the other type of libs
      set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
      find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
  endif()
find_package(Boost QUIET)
 
# Set up referencing of Boost
include_directories(${Boost_INCLUDE_DIR})
add_definitions(-DBOOST_ALL_NO_LIB)
set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})

add_definitions(-std=c++11) # Requested by PolyVox, at a minimum.
add_definitions(-Wall -O3)

set(HDRS
    CatapultGame.h
    CameraMan.h
    Catapult.h
    Castle.h
    InputBroadcast.h
    InputFacade.h
    DebugPanel.h
    Light.h
    MeshConversions.h
    OgreFacade.h
    Stone.h
    VolumeObject.h
)
 
set(SRCS 
    main.cpp
    CatapultGame.cpp
    CameraMan.cpp
    Catapult.cpp
    Castle.cpp
    DebugPanel.cpp
    InputBroadcast.cpp
    InputFacade.cpp
    Light.cpp
    MeshConversions.cpp
    OgreFacade.cpp
    Stone.cpp
    VolumeObject.cpp
)
 
include_directories( 
    ${OIS_INCLUDE_DIRS}
    ${OGRE_INCLUDE_DIRS}
    ${OGRE_SAMPLES_INCLUDEPATH}
    ${OGRE_Overlay_INCLUDE_DIRS}
    /usr/local/include/PolyVoxCore
)
 
add_executable(CatapultGame ${HDRS} ${SRCS})
 
set_target_properties(CatapultGame PROPERTIES DEBUG_POSTFIX _d)
 
target_link_libraries(CatapultGame ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${OGRE_Overlay_LIBRARIES}, PolyVoxCore)
 
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/bin)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/media)
 
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/dist/bin) 


# Bit of an abuse here: make install creates a redistributable release, does not install
# the game.
install(TARGETS CatapultGame
    RUNTIME DESTINATION bin
    CONFIGURATIONS All)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/dist/media
    DESTINATION ./
    CONFIGURATIONS Release RelWithDebInfo Debug
)

install(FILES 
    ${CMAKE_SOURCE_DIR}/plugins.cfg
    ${CMAKE_SOURCE_DIR}/resources.cfg
    DESTINATION bin
    CONFIGURATIONS Release RelWithDebInfo Debug
)

install(FILES 
    ${CMAKE_SOURCE_DIR}/plugins.cfg
    ${CMAKE_SOURCE_DIR}/resources.cfg
    DESTINATION bin
    CONFIGURATIONS Release RelWithDebInfo Debug
)

# Copy the pre-compiled Ogre parts - this seems to be sufficient to distribute the program.
install(FILES 
    libs/libboost_system.so.1.58.0
    libs/libOgreMain.so.2.0.0
    libs/libOgreOverlay.so.2.0.0
    libs/libOIS-1.3.0.so
    libs/libPolyVoxCore.so.0
    libs/Plugin_CgProgramManager.so
    libs/Plugin_CgProgramManager.so.2.0.0
    libs/Plugin_ParticleFX.so
    libs/Plugin_ParticleFX.so.2.0.0
    libs/RenderSystem_GL.so
    libs/RenderSystem_GL.so.2.0.0
    DESTINATION libs
    CONFIGURATIONS Release RelWithDebInfo Debug
)

# Script that sets the library path.
install(FILES CatapultGame.sh DESTINATION .)

install(FILES ../ogre/Samples/Media/packs/SdkTrays.zip DESTINATION ${CMAKE_SOURCE_DIR}/dist/media/packs)

install(FILES models/Catapult.mesh DESTINATION ${CMAKE_SOURCE_DIR}/dist/media/models)
install(FILES models/Wood.material DESTINATION ${CMAKE_SOURCE_DIR}/dist/media/materials/scripts)

install(FILES models/Castle.mesh DESTINATION ${CMAKE_SOURCE_DIR}/dist/media/models)
install(FILES models/Walls.material DESTINATION ${CMAKE_SOURCE_DIR}/dist/media/materials/scripts)
install(FILES models/wall-578204_960_720.jpg DESTINATION ${CMAKE_SOURCE_DIR}/dist/media/materials/textures)

install(FILES models/Terrain.mesh DESTINATION ${CMAKE_SOURCE_DIR}/dist/media/models)
install(FILES models/Terrain.material DESTINATION ${CMAKE_SOURCE_DIR}/dist/media/materials/scripts)
install(FILES models/meadow-253616_960_720.jpg DESTINATION ${CMAKE_SOURCE_DIR}/dist/media/materials/textures)
 
