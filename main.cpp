#include <iostream>

#include "CatapultGame.h"


int main(int argc, char *argv[]) {
  catapult::CatapultGame theGame;
  try {
    theGame.go();
  } catch( Ogre::Exception& e ) {
    std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
    return 1;
  }

  return 0;
}
