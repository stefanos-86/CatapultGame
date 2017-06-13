#include "InputBroadcast.h"

namespace catapult {
  
InputBroadcast::InputBroadcast() :
  keyboardSubscribersCount(0),
  mouseSubscribersCount(0)
{ }

  
bool InputBroadcast::keyPressed(const OIS::KeyEvent& arg) {
  for (size_t i = 0; i < keyboardSubscribersCount; ++i)
    keyboardSubscribers.at(i)->keyPressed(arg);

  return true;
}

  
bool InputBroadcast::keyReleased(const OIS::KeyEvent& arg) {
  for (size_t i = 0; i < keyboardSubscribersCount; ++i)
    keyboardSubscribers.at(i)->keyReleased(arg);
  
  return true;
}


bool InputBroadcast::mouseMoved(const OIS::MouseEvent& arg) {
  for (size_t i = 0; i < mouseSubscribersCount; ++i)
    mouseSubscribers.at(i)->mouseMoved(arg);
  
  return true;
};

bool InputBroadcast::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id) {
  for (size_t i = 0; i < mouseSubscribersCount; ++i)
    mouseSubscribers.at(i)->mousePressed(arg, id);
  
  return true;
}

bool InputBroadcast::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id) {
  for (size_t i = 0; i < mouseSubscribersCount; ++i)
    mouseSubscribers.at(i)->mouseReleased(arg, id);
  
  return true;
}

void InputBroadcast::subscribeToKeyboard(OIS::KeyListener* newListener) {
  if (keyboardSubscribersCount == keyboardSubscribers.size())
    throw catapult::TooManySubscribers();

  keyboardSubscribers.at(keyboardSubscribersCount++) = newListener;
}

void InputBroadcast::subscribeToMouse(OIS::MouseListener* newListener) {
  if (mouseSubscribersCount == mouseSubscribers.size())
    throw catapult::TooManySubscribers();

  mouseSubscribers.at(mouseSubscribersCount++) = newListener;
}

  
}
