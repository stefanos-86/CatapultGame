# CatapultGame
Learn-by-doing project: slow, buggy mix of Ogre 2 and PolyVox

## Disclaimer!
This is a learning project, with all the drawbacks implied...
...don't expect to be able to build this one "out of the box", don't expect it to be "state of art", don't expect it to make any sense at all...

### The game
Point the Catapult at Castle Voxel and punch holes into its walls.

![Gameplay image](https://github.com/stefanos-86/CatapultGame/blob/master/Screenshot.png "Looks like this")

I cobbled this together via [Ogre 2](http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Main+Page+V2) and [PolyVox](http://www.volumesoffun.com/).

### What is missing to make complete a build
It may compile "as is" (CMake ., then make)... but you need more stuff to make it work.
You need a "lib" folder with boost, Ogre and PolyVox libraries already built, plus the basic Ogre resource pack from the tutorial application.
Check the file itself to see what you have to add to create a full distribution.


![Animation](https://github.com/stefanos-86/CatapultGame/blob/master/theMovie.gif "Animation to show the game")

