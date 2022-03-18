// Main game script
// Credit: Tutorial 2_9/3_9 from Alfred Shaker for environment setup, flat plane/light, BaseApplication
// basic flat grassy plane with a light.
// Camera movement is locked.

#include "ticTacSnipe.h"

TicTacSnipeApplication::TicTacSnipeApplication()
    :
    cameraSpawnPoint(Ogre::Vector3(0, 400, -1500)),
    cameraLookPoint(Ogre::Vector3(0, 400, 500))
{
}

TicTacSnipeApplication::~TicTacSnipeApplication()
{
}

void TicTacSnipeApplication::createScene()
{
    mCamera->setPosition(cameraSpawnPoint);
    mCamera->lookAt(cameraLookPoint);
    mCamera->setNearClipDistance(.1);

    bool infiniteClip =
        mRoot->getRenderSystem()->getCapabilities()->hasCapability(
            Ogre::RSC_INFINITE_FAR_PLANE);

    if (infiniteClip)
        mCamera->setFarClipDistance(0);
    else
        mCamera->setFarClipDistance(50000);

    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

    Ogre::MeshManager::getSingleton().createPlane(
        "ground",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane,
        5000, 5000, 20, 20,
        true,
        1, 5, 5,
        Ogre::Vector3::UNIT_Z);

    Ogre::Entity* groundEntity = mSceneMgr->createEntity("ground");

    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);

    groundEntity->setCastShadows(false);

    // Materials found in env\Ogre10\sdk\media\materials\scripts\Examples.material
    groundEntity->setMaterialName("Examples/GrassFloor");

    mSceneMgr->setAmbientLight(Ogre::ColourValue(.4, .4, .4));

    Ogre::Vector3 lightDir(.55, -.3, .75);
    lightDir.normalise();

    Ogre::Light* light = mSceneMgr->createLight("TestLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightDir);
    light->setDiffuseColour(Ogre::ColourValue(0.4, 0.4, 0.4));
    light->setSpecularColour(Ogre::ColourValue(.02, .02, .02));
}

void TicTacSnipeApplication::createFrameListener()
{
    BaseApplication::createFrameListener();
}

void TicTacSnipeApplication::destroyScene()
{

}

bool TicTacSnipeApplication::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
    bool ret = BaseApplication::frameRenderingQueued(fe);

    return ret;
}


#if Ogre_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
    int main(int argc, char* argv[])
#endif
    {
        // Create application object
        TicTacSnipeApplication app;

        try {
            app.go();
        }
        catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif