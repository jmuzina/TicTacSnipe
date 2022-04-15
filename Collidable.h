#ifndef Collidable_H
#define Collidable_H

#include "ticTacSnipe.h"

class Collidable {
public:
    Collidable(Ogre::Entity* entity, btRigidBody* rigidBody, btCollisionShape* shape) {
        entity_ = entity;
        rigidBody_ = rigidBody;
        shape_ = shape;
    }

    ~Collidable() {
        if (entity_ != nullptr) {
            if (entity_->isAttached()) entity_->detachFromParent();
            delete entity_;
        }
        if (rigidBody_ != nullptr) {
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