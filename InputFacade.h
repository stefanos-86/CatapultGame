#ifndef INPUTFACADE_H
#define INPUTFACADE_H

#include <cstdlib>  // size_t

namespace OIS { class Mouse; class Keyboard; class InputManager; }

namespace catapult {

class InputFacade {
  public:
    void initialize(const size_t windowHandle);
    void shutdown();
    
    void setMouseAreaSize(const unsigned int width, const unsigned int height);

    OIS::Mouse* mouse;
    OIS::Keyboard* keyboard;
    
private:
    OIS::InputManager* inputManager;
  };

}

#endif
