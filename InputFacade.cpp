#include "InputFacade.h"

#include <sstream>

#include <OISKeyboard.h>
#include <OISMouse.h>
#include <OISInputManager.h>


namespace catapult {

void InputFacade::initialize(const size_t windowHandle) {
    std::ostringstream windowHndStr;   
    windowHndStr << windowHandle;
  
    OIS::ParamList pl;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    
    inputManager = OIS::InputManager::createInputSystem( pl );

    keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
    mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
}


void InputFacade::shutdown() {
  if (!inputManager)
    return;
    
  inputManager->destroyInputObject(mouse);
  inputManager->destroyInputObject(keyboard);

  OIS::InputManager::destroyInputSystem(inputManager);
}


void InputFacade::setMouseAreaSize(const unsigned int width, const unsigned int height) {
    const OIS::MouseState &ms = mouse->getMouseState();
    ms.width = width;
    ms.height = height;
}


}

