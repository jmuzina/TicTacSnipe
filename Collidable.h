// Interface for collidable entities
// Developed by myself as part of Tic Tac Snipe project

#ifndef Collidable_H
#define Collidable_H

#include "ticTacSnipe.h"
using namespace Ogre;

class Collidable {
public:
    Collidable(Ogre::Entity* entity, btRigidBody* rigidBody, btCollisionShape* shape, int owner, Vector4 color = Vector4::ZERO) {
        entity_ = entity;
        rigidBody_ = rigidBody;
        shape_ = shape;
        markedForDeletion_ = false;
        owner_ = owner;
        isBullet_ = (owner != -1);
        age_ = 0;
        struckBy_ = nullptr;

        if (isBullet_)
            setColor(color);
    }

    void markForDeletion() {
        markedForDeletion_ = true;
    }

    bool isMarkedForDeletion() const {
        return markedForDeletion_;
    }

    bool isBullet() const {
        return isBullet_;
    }

    int getOwnerId() const {
        return owner_;
    }

    Ogre::Entity* getEntity() const {
        return entity_;
    }

    bool isExpiring() const {
        return age_ >= MAX_BULLET_LIFETIME;
    }

    bool alreadyHitBy(Collidable* checkBullet) const {
        if (struckBy_ == nullptr) return false;
        return (struckBy_ == checkBullet);
    }

    void clearHit() {
        owner_ = -1;
        struckBy_ = nullptr;
    }

    void markHit(Collidable* struckBy) {
        owner_ = struckBy->getOwnerId();
        
    }

    // Update the age of the collidable and delete it if necessary
    void tick(float timePassed = 0) {
        age_ += timePassed;

        if (isExpiring())
            markForDeletion();
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

    btRigidBody* getRigidBody() const { return rigidBody_; }

private:
    Ogre::Entity* entity_;
    btRigidBody* rigidBody_;
    btCollisionShape* shape_;
    Collidable* struckBy_;
    bool markedForDeletion_ = false;
    int owner_ = -1;
    bool isBullet_ = false;
    float age_ = 0;
    const float MAX_BULLET_LIFETIME = 5;

    void setColor(Vector4 color) {
        for (int i = 0; i < entity_->getNumSubEntities(); ++i) {
            Ogre::SubEntity* subEntity = entity_->getSubEntity(i);
            //subEntity->setMaterialName(BASE_COLOR);
            auto material_ = subEntity->getMaterial().get()->clone(entity_->getName() + std::to_string(i));
            material_->setDiffuse(color.x, color.y, color.z, color.w);
            material_->setSpecular(color.x, color.y, color.z, color.w);
            subEntity->setMaterialName(material_->getName());
        }
    }
};

#endif