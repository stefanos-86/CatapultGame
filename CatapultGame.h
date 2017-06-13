#ifndef CATAPULTGAME_H
#define CATAPULTGAME_H

#include <memory>

#include <OISKeyboard.h>
#include <OgreFrameListener.h>

#include "Catapult.h"
#include "Castle.h"
#include "DebugPanel.h"
#include "InputBroadcast.h"
#include "OgreFacade.h"


namespace catapult {
  class CameraMan;
  class Light;

  /** Main class that coordinates all the others to make the game.
      Derived from the Ogre tutorials base application. */
  class CatapultGame : public Ogre::FrameListener, public OIS::KeyListener
  {
  public:
      CatapultGame(void);
      ~CatapultGame(void);
      void go(void);
      
      // public OIS::KeyListener
        bool keyPressed(const OIS::KeyEvent& arg) final;
        bool keyReleased(const OIS::KeyEvent& arg) final { return true; }

  private:
      bool haltMainLoop;
      
      unsigned long lastTime;
      
      InputBroadcast inputBus;
      OgreFacade ogre;
      DebugPanel debugPanel; // Order of the variables matters for destruction. Keep as is.
      
      std::unique_ptr<Catapult> catapult;
      std::unique_ptr<Castle> castle;
      std::unique_ptr<CameraMan> camera; 
      std::unique_ptr<Light> mainLight; 
    
      bool setup();
      void createFrameListener(void);
      void createScene();

      // Ogre::FrameListener
      bool frameRenderingQueued(const Ogre::FrameEvent& evt) final;
      
      /** Simulation "master clock" that calls all the elements that need to refresh at each step. 
          It passes them the time in seconds (makes for easier thinking in the other parts of the 
          simulation.*/
      void advanceSimulation(const unsigned long elapsedMicroseconds);
  };

}



#endif
