#ifndef TICTACTOEBOARD_H
#define TICTACTOEBOARD_H

#include "ticTacSnipe.h"
#include "Collidable.h"
#include "ActiveCollidables.h"
#include "MotionState.h"

using namespace Ogre;

class TicTacToeBoard {
	
public:
	TicTacToeBoard(SceneManager* mSceneMgr, btDiscreteDynamicsWorld* dynamicsWorld, ActiveCollidables* ActiveCollidables, Vector3 spawnPosition, Quaternion spawnRotation, Vector3 scale) {
        mSceneMgr_ = mSceneMgr;
        dynamicsWorld_ = dynamicsWorld;
        ActiveCollidables_ = ActiveCollidables;
		baseBoardNode_ = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		Entity* baseBoard = mSceneMgr->createEntity("cube.mesh");
		baseBoardNode_->attachObject(baseBoard);
        baseBoard->setCastShadows(true);
        baseBoardNode_->scale(Ogre::Vector3(scale.x, scale.y, scale.z));

        const btVector3 collidablePosititon = btVector3(spawnPosition.x, spawnPosition.y, spawnPosition.z);

        // empty ogre vectors for the cubes size and position
        Ogre::Vector3 size = Ogre::Vector3::ZERO;
        Ogre::Vector3 pos = Ogre::Vector3::ZERO;
        // Convert the bullet physics vector to the ogre vector
        pos.x = collidablePosititon.getX();
        pos.y = collidablePosititon.getY();
        pos.z = collidablePosititon.getZ();

        Ogre::AxisAlignedBox boundingB = baseBoard->getBoundingBox();

        boundingB.scale(Ogre::Vector3(scale.x, scale.y, scale.z));
        size = boundingB.getSize() * 0.95f;
        btTransform Transform;
        Transform.setIdentity();
        Transform.setOrigin(collidablePosititon);
        Transform.setRotation(btQuaternion(spawnRotation.x, spawnRotation.y, spawnRotation.z, spawnRotation.w));
        MotionState* ms = new MotionState(Transform, baseBoardNode_);

        btVector3 HalfExtents(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
        btCollisionShape* Shape = new btBoxShape(HalfExtents);
        btVector3 LocalInertia;
        Shape->calculateLocalInertia(DIVIDER_MASS, LocalInertia);
        btRigidBody* RigidBody = new btRigidBody(DIVIDER_MASS, ms, Shape, LocalInertia);

        // Store a pointer to the Ogre Node so we can update it later
        RigidBody->setUserPointer((void*)(baseBoardNode_));

        // Add it to the physics world
        dynamicsWorld_->addRigidBody(RigidBody);
        RigidBody->setGravity(btVector3(0, 0, 0));
        //collisionShapes.push_back(Shape);
        ActiveCollidables_->registerMiscCollidable(baseBoard, RigidBody, Shape);

        constructDivider(mSceneMgr, Vector3(0, 0, OFFSET), Vector3(0, 1, 0), VERTICAL_SCALE);
        constructDivider(mSceneMgr, Vector3(0, 0, -OFFSET), Vector3(0, 1, 0), VERTICAL_SCALE);
        constructDivider(mSceneMgr, Vector3(0, OFFSET, 0), Vector3(0, 0, 1), HORIZONTAL_SCALE);
        constructDivider(mSceneMgr, Vector3(0, -OFFSET, 0), Vector3(0, 0, 1), HORIZONTAL_SCALE);
	}

private:
    void constructDivider(SceneManager* mSceneMgr, Vector3 spawnPosition, Vector3 spawnRotation, Vector3 scale) {
        SceneNode* childBoardNode = baseBoardNode_->createChildSceneNode();
        Entity* board = mSceneMgr->createEntity("cube.mesh");
        childBoardNode->attachObject(board);
        childBoardNode->setScale(scale);
        childBoardNode->setPosition(spawnPosition);
        childBoardNode->rotate(spawnRotation, Ogre::Degree(90));
        boards_.push_back(childBoardNode);
    };
	std::vector<SceneNode*> boards_ = std::vector<SceneNode*>();
	SceneNode* baseBoardNode_;
	const int OFFSET = 20;
    const Vector3 VERTICAL_SCALE = Vector3(0.2, 1, 0.3);
    const Vector3 HORIZONTAL_SCALE = Vector3(0.2, 0.3, 1);
    const int DIVIDER_MASS = MAXINT;
    SceneManager* mSceneMgr_;
    btDiscreteDynamicsWorld* dynamicsWorld_;
    ActiveCollidables* ActiveCollidables_;
};

#endif