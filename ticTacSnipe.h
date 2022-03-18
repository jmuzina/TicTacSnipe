#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

#include "BaseApplication.h"

class TicTacSnipeApplication : public BaseApplication
{
public:
	TicTacSnipeApplication();
	virtual ~TicTacSnipeApplication();

protected:
	virtual void createScene();
	virtual void createFrameListener();
	virtual void destroyScene();
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);

private:
	Ogre::Vector3               cameraSpawnPoint;
	Ogre::Vector3               cameraLookPoint;
};


