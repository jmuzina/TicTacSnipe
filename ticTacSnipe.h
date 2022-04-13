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
	void CreateBullet(const btVector3& Position, btScalar Mass, const btVector3& scale, char* name);
	bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);

private:
	
	void createBulletSim(void); 
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btCollisionShape* groundShape;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	
	Ogre::Vector3               cameraSpawnPoint;
	Ogre::Vector3               cameraLookPoint;
	//Ogre::TerrainGroup* mTerrainGroup;
};

#endif