#ifndef BOARD_SPACE_H
#define BOARD_SPACE_H

#include "ticTacSnipe.h"
using namespace Ogre;

class BoardSpace {
public:
	BoardSpace(Ogre::SceneNode* baseBoardNode, SceneManager* mSceneMgr, btDiscreteDynamicsWorld* dynamicsWorld, ActiveCollidables* ActiveCollidables, Vector3 spawnPosition, Quaternion spawnRotation, String entName) {
        SceneNode* cubeNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        Entity* cubeEntity = mSceneMgr->createEntity(entName, "cube.mesh");
        cubeNode->attachObject(cubeEntity);
        cubeEntity->setCastShadows(true);
        cubeNode->scale(SCALE_);
        const btVector3 collidablePosititon = btVector3(spawnPosition.x, spawnPosition.y, spawnPosition.z);

        cubeNode->setPosition(spawnPosition);
        cubeNode->setOrientation(spawnRotation);

        // empty ogre vectors for the cubes size and position
        Ogre::Vector3 size = Ogre::Vector3::ZERO;
        Ogre::Vector3 pos = Ogre::Vector3::ZERO;
        // Convert the bullet physics vector to the ogre vector
        pos.x = collidablePosititon.getX();
        pos.y = collidablePosititon.getY();
        pos.z = collidablePosititon.getZ();

        Ogre::AxisAlignedBox boundingB = cubeEntity->getBoundingBox();

        boundingB.scale(SCALE_);
        size = boundingB.getSize() * 0.95f;
        btTransform Transform;
        Transform.setIdentity();
        Transform.setOrigin(collidablePosititon);
        Transform.setRotation(btQuaternion(spawnRotation.x, spawnRotation.y, spawnRotation.z, spawnRotation.w));
        MotionState* ms = new MotionState(Transform, cubeNode);

        btVector3 HalfExtents(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
        btCollisionShape* Shape = new btBoxShape(HalfExtents);
        btVector3 LocalInertia;
        Shape->calculateLocalInertia(BOARD_MASS, LocalInertia);
        btRigidBody* RigidBody = new btRigidBody(BOARD_MASS, ms, Shape, LocalInertia);

        // Store a pointer to the Ogre Node so we can update it later
        RigidBody->setUserPointer((void*)(cubeNode));

        // Add it to the physics world
        dynamicsWorld->addRigidBody(RigidBody);

        // Disable block gravity
        RigidBody->setGravity(btVector3(0, 0, 0));

        collidable_ = ActiveCollidables->registerCollidable(cubeEntity, RigidBody, Shape);
	}

    Collidable* getCollidable() const {
        return collidable_;
    }

private:
    const int BOARD_MASS = MAXINT;
    Collidable* collidable_ = nullptr;
    const Vector3 SCALE_ = Vector3(0.24, 0.36, 0.36);
};

#endif