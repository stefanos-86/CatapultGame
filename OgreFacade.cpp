#include "OgreFacade.h"

#include <Compositor/OgreCompositorManager2.h>
#include <OgreConfigFile.h>
#include <OgreTextureManager.h>


namespace catapult {

OgreFacade::OgreFacade() :
  root(new Ogre::Root("plugins.cfg")),
  overlay(new Ogre::OverlaySystem())  // TODO Is this stuff leaking???
{
  setupResources();

  const size_t numThreads = 4; // I have this many cores. I am not looking to optimize in all platforms.
  
  scene = root->createSceneManager(Ogre::ST_GENERIC, numThreads,
                                   Ogre::INSTANCING_CULLING_THREADED,
                                   "TheOgreSceneManager");
  
  scene->addRenderQueueListener(overlay);  
}


OgreFacade::~OgreFacade() {
  Ogre::WindowEventUtilities::removeWindowEventListener(window, this);
  windowClosed(window);
}


bool OgreFacade::configureWindow() {
    bool restoredConfig = root->restoreConfig();  // Configuration saved on file.
    if(restoredConfig || root->showConfigDialog())
    {
        // True means "make a default rendering window".
        window = root->initialise(true, "Catapult");
    }
    Ogre::WindowEventUtilities::addWindowEventListener(window, this);
    return (window != nullptr);
}


OgreBites::InputContext OgreFacade::prepareInputSystem(OIS::KeyListener* keys, OIS::MouseListener* mice) {
  size_t windowHnd = 0;
  window->getCustomAttribute("WINDOW", &windowHnd);
  inputSystem.initialize(windowHnd);
  
  OgreBites::InputContext inputContext;
  inputContext.mMouse = inputSystem.mouse; 
  inputContext.mKeyboard = inputSystem.keyboard;
    
  windowResized(window); // Initialize mouse clipping area.
  
  inputSystem.mouse->setEventCallback(mice);
  inputSystem.keyboard->setEventCallback(keys);
  
  return inputContext;
}


void OgreFacade::finalizeResources() {
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);  
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}


void OgreFacade::createViewports(Ogre::Camera* camera) {
   Ogre::CompositorManager2* compositorManager = root->getCompositorManager2();
   const Ogre::String workspaceName = "TheOnlyWorkspaceIUse";
   
   if (!compositorManager->hasWorkspaceDefinition(workspaceName)) {
    const Ogre::ColourValue skyColor(0.05f, 0.25f, 0.4f); // Won't use a fully-fledged skybox for this demo.
    compositorManager->createBasicWorkspaceDef(workspaceName, skyColor, Ogre::IdString());
   }
   
   compositorManager->addWorkspace(scene, window, camera, workspaceName, true);
}


void OgreFacade::recaptureInputDevices() {
    inputSystem.keyboard->capture();
    inputSystem.mouse->capture();
}


void OgreFacade::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);
    inputSystem.setMouseAreaSize(width, height);
}


void OgreFacade::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the only one I have... not sure if Ogre can call with other windows).
    if( rw == window )
      inputSystem.shutdown();
}


void OgreFacade::setupResources(void)
{
    const Ogre::String resourceConfigurationFile = "resources.cfg";
    
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(resourceConfigurationFile);

    // Go through all sections and settings in the file.
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}

}

