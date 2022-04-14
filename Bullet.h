#ifndef BULLET_H
#define BULLET_H

#include "ticTacSnipe.h"

class Bullet {
public:
    Bullet(Ogre::Entity* entity, btRigidBody* rigidBody, btCollisionShape* shape) {
        entity_ = entity;
        rigidBody_ = rigidBody;
        shape_ = shape;
    }

    ~Bullet() {
        if (entity_ != nullptr) {
            if (entity_->isAttached()) entity_->detachFromParent();
            delete entity_;
        }
        if (rigidBody_ != nullptr) {
            //dynamicsWorld_->removeRigidBody(RigidBody_);
            delete rigidBody_->getMotionState();
            delete rigidBody_;
        }
        if (shape_ != nullptr) delete shape_;
    }

    btRigidBody* getRigidBody() const { return rigidBody_;  }

private:
    Ogre::Entity* entity_;
    btRigidBody* rigidBody_;
    btCollisionShape* shape_;
};

#endif