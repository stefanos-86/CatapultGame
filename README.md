# CatapultGame
Learn-by-doing project: slow, buggy mix of Ogre 2 and PolyVox

I cobbled this together via [Ogre 2](http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Main+Page+V2) and [PolyVox](http://www.volumesoffun.com/).

## Disclaimer!
This is a learning project, with all the drawbacks implied...
...don't expect to be able to build this one "out of the box", don't expect it to be "state of art", don't expect it to make any sense at all...

### The game
Point the Catapult at Castle Voxel and punch holes into its walls.

![Gameplay image](https://github.com/stefanos-86/CatapultGame/blob/master/Screenshot.png "Looks like this")

![Animation](https://github.com/stefanos-86/CatapultGame/blob/master/theMovie.gif "Animation to show the game")

### What is missing to make complete a build
It may compile "as is" (CMake ., then make)... but you need more stuff to make it work.
You need a "lib" folder with boost, Ogre and PolyVox libraries already built, plus the basic Ogre resource pack from the tutorial application.
Check the file itself to see what you have to add to create a full distribution.

### ...but what is inside?
0. Ogre has all the facilities to load a castle-shaped mesh (anything its command line tools can "digest" - mine is done with Blender) and return its "raw" vertices and triangles. 
0. A "simple" algorithm to tell if a point is inside the mesh. Just pick a ray from the point to... anywhere you want, and count how many triangles it goes through. An odd number? That point is inside. That's how I know how to fill a voxel: is its centre inside the castle?
0. A voxel library, not just to keep all the cubes at bay and "remove" them when the ball hits, but to scan them with the Marching Cubes algorithm and get out a mesh with all the holes to pass back to Ogre.
0. Lots, lots of glue code to setup Ogre for rendering, deal with the input, compute the ball trajectory...

I did not do this alone, but I adapted a lot of what I found. Check the comments in the code to know where the originals are from.


