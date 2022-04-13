#ifndef TICTACSNIPE_H
#define TICTACSNIPE_H

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include "OgreManualObject.h"
#include "../../bullet-2.82/src/btBulletDynamicsCommon.h"
#include "../../bullet-2.82/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BaseApplication.h"
#include <queue>

class TicTacSnipeApplication : public BaseApplication
{
public:
	TicTacSnipeApplication();
	virtual ~TicTacSnipeApplication();

protected:
	virtual void createScene();
	virtual void createFrameListener();
	virtual void destroyScene();
	void CreateBullet(const btVector3& Position, btScalar Mass, const btVector3& scale, char* name);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);
	bool frameStarted(const Ogre::FrameEvent& evt);
	bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	Ogre::ManualObject* CreateBulletMesh(Ogre::String name, Ogre::String matName);

private:
	
	void CreateBulletSim(void); 
	void configureTerrainDefaults(Ogre::Light*);
	void defineTerrain(long x, long y);
	void initBlendMaps(Ogre::Terrain* terrain);
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btCollisionShape* groundShape;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	bool mTerrainsImported;
	Ogre::TerrainGlobalOptions* mTerrainGlobals;
	Ogre::Vector3               cameraSpawnPoint;
	Ogre::Vector3               cameraLookPoint;
	Ogre::TerrainGroup* mTerrainGroup;

	std::queue<Ogre::Entity*> bullets;
	const int MAX_BULLETS = 10;
};

#endif