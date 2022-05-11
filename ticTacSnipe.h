// ticTacSnipe.h

#ifndef TICTACSNIPE_H
#define TICTACSNIPE_H

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include "OgreManualObject.h"
#include <btBulletDynamicsCommon.h>
#include <btHeightfieldTerrainShape.h>
#include "BaseApplication.h"
#include <queue>
#include "Collidable.h"
#include "ActiveCollidables.h"
#include "TicTacToeBoard.h"

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

using namespace Ogre;

class TicTacSnipeApplication : public BaseApplication
{
public:
	TicTacSnipeApplication();
	virtual ~TicTacSnipeApplication();

protected:
	virtual void createScene();
	virtual void createFrameListener();
	virtual void destroyScene();
	void CreateBullet(const btVector3& collidablePosition, int playerId);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);
	bool frameStarted(const Ogre::FrameEvent& evt);
	bool mouseMoved(const OIS::MouseEvent& arg);
	bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	bool keyPressed(const OIS::KeyEvent& arg);
	bool keyReleased(const OIS::KeyEvent& arg);
	//CEGUI
	CEGUI::OgreRenderer* mRenderer;
	//when true will show cursor and stop moving camera orientation
	bool inMenu;
	//dismisses start window
	bool Start(const CEGUI::EventArgs&);
	//dismisses winner button and calls reset to clean the board
	bool DisplayWinner(const CEGUI::EventArgs&);
	//resets the board
	bool ResetBoard();
	//lets me hide and display menus during runtime
	CEGUI::Window* sheet;
	//tracks who shot the winning boards to display winner and update scores
	int Winner;
	//used to change camera fov for zoom
	Ogre::Real Zoom;
	
private:
	void CreateBulletSim(void); 
	void configureTerrainDefaults(Ogre::Light*);
	void defineTerrain(long x, long y);
	void initBlendMaps(Ogre::Terrain* terrain);
	void checkForWinner();
	void setZoomState(bool);
	Vector3 getForwardAngle() const {
		return mCamera->getOrientation() * Vector3(0, 0, -1);
	}
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btCollisionShape* groundShape;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	btRigidBody* mGroundBody;

	bool mTerrainsImported;
	Ogre::TerrainGlobalOptions* mTerrainGlobals;
	Ogre::Vector3               cameraSpawnPoint;
	Ogre::Vector3               cameraLookPoint;
	Ogre::TerrainGroup* mTerrainGroup;
	ActiveCollidables* ActiveCollidables_;
	std::vector<TicTacToeBoard*> boards_ = std::vector<TicTacToeBoard*>();

	OgreBites::Label* debugLabel;

	const int BULLET_FORCE = 1200;
	const btVector3 BULLET_SIZE = btVector3(0.01, 0.01, 0.01);
	const btScalar BULLET_MASS = 1.25;
	const int BULLET_SPIN = 2000;
	const float BULLET_SPAWN_Y_OFFSET = -5;

	//tracks who is next to fire
	bool player1Turn;
	//tracks if the camera is zoomed or not
	bool zoomed_ = false;
	//tracks player wins
	int scores_[2] = { 0, 0 };
};

#endif