#ifndef INPUTBROADCAST
#define INPUTBROADCAST

#include <array>
#include <stdexcept>

#include <OISKeyboard.h>
#include <OISMouse.h>


namespace catapult {
  static const size_t MAX_INPUT_BROADCAST_SUBSCRIBERS = 10;

  /** The main role is to pass the input events to classes that actually do something. */
  class InputBroadcast : public OIS::KeyListener, public OIS::MouseListener
  {
  public:
    InputBroadcast();
    
    virtual bool keyPressed(const OIS::KeyEvent& arg) final;
    virtual bool keyReleased( const OIS::KeyEvent& arg ) final;
    
    virtual bool mouseMoved(const OIS::MouseEvent& arg) final;
    virtual bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id) final;
    virtual bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id) final;
    
    void subscribeToKeyboard(OIS::KeyListener* newListener);
    void subscribeToMouse(OIS::MouseListener* newListener);
    
  private:
    ///The usual publisher/subscriber.
    size_t keyboardSubscribersCount;
    size_t mouseSubscribersCount;
    std::array<OIS::KeyListener*, MAX_INPUT_BROADCAST_SUBSCRIBERS> keyboardSubscribers;
    std::array<OIS::MouseListener*, MAX_INPUT_BROADCAST_SUBSCRIBERS> mouseSubscribers;
  };
  
  class TooManySubscribers : public std::runtime_error {
  public:
    TooManySubscribers() : std::runtime_error("Too much stuff") {}
  };
  
}

#endif
