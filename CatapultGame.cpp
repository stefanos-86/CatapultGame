#include "CatapultGame.h"

#include "CameraMan.h"
#include "Light.h"

using namespace catapult;

namespace catapult {

CatapultGame::CatapultGame(void) :
    haltMainLoop(false),
    lastTime(0),
    catapult(nullptr),
    castle(nullptr),
    camera(nullptr),
    mainLight(nullptr)
{ }


CatapultGame::~CatapultGame(void) {
  //Makes the unique pointers work.
}

void CatapultGame::createFrameListener(void)
{   
    auto ic = ogre.prepareInputSystem(&inputBus, &inputBus);
    debugPanel.initialize(ogre.window, ic);
    camera->debugPanel = &debugPanel;

    inputBus.subscribeToKeyboard(&debugPanel);
    inputBus.subscribeToKeyboard(camera.get());
    inputBus.subscribeToMouse(camera.get());
    inputBus.subscribeToKeyboard(this);
    inputBus.subscribeToKeyboard(castle.get());

    ogre.root->addFrameListener(this);
}


void CatapultGame::go(void) {
  if (!setup())
      return;
  ogre.root->renderOneFrame();

  lastTime = ogre.root->getTimer()->getMicroseconds();
  while(!haltMainLoop) {
    ogre.window->swapBuffers();
    Ogre::WindowEventUtilities::messagePump();
    ogre.root->renderOneFrame();

    unsigned long timeNow = ogre.root->getTimer()->getMicroseconds();
    unsigned long elapsedTime = timeNow - lastTime;
    lastTime = timeNow;
    
    advanceSimulation(elapsedTime);
  }
  
  // May need to clean up leaking Ogre stuff in the scene.
}

bool CatapultGame::keyPressed(const OIS::KeyEvent& arg) {
  if (arg.key == OIS::KC_ESCAPE)
    haltMainLoop = true;
  
  return true;
}



bool CatapultGame::setup(void)
{
    if (! ogre.configureWindow()) 
      return false;

    ogre.finalizeResources();
    
    camera.reset(new CameraMan(ogre.scene));
    
    mainLight.reset(new Light(ogre, *camera));
    
    
    ogre.createViewports(camera->camera);

    createScene();

    createFrameListener();

    return true;
};



bool CatapultGame::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if (haltMainLoop || ogre.window->isClosed())
        return false;
    
    ogre.recaptureInputDevices();

    debugPanel.trayManager->frameRenderingQueued(evt);
    camera->cameraMan->frameRenderingQueued(evt);
    
    debugPanel.readCameraPosition(camera->camera); // Auto-skip if invisible.

    return true;
}



void CatapultGame::createScene()
{
    castle.reset(new Castle(*ogre.scene));
    catapult.reset(new Catapult(*ogre.scene));
    inputBus.subscribeToKeyboard(catapult.get());
}


void CatapultGame::advanceSimulation(const unsigned long elapsedMicroseconds) {
   mainLight->moveTo(*camera);

   const float elapsedSeconds = elapsedMicroseconds / 1000000.0;
   catapult->advanceSimulation(elapsedSeconds);
   
   if (catapult->inFligthStone)
    castle->advanceSimulation(*ogre.scene, *(catapult->inFligthStone)); 
}


}


