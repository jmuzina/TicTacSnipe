// Container class allowing interface with all collidable objects
// Written by Joe for Tic Tac Snipe project
// ActiveCollidables.h

#ifndef ACTIVE_COLLIDABLES_H
#define ACTIVE_COLLIDABLES_H

#include "ticTacSnipe.h"
#include "Collidable.h"
#include <queue>

class ActiveCollidables {
public:
	ActiveCollidables(btDiscreteDynamicsWorld* dynamicsWorld) { dynamicsWorld_ = dynamicsWorld; };

	Collidable* registerCollidable(Ogre::Entity* entity, btRigidBody* rigidBody, btCollisionShape* shape, int playerId = -1, Vector4 color = Vector4::ZERO) {
		Collidable* newCollidable = new Collidable(entity, rigidBody, shape, playerId, color);

		// Add new collidable to registry
		const bool inserted = all.insert(std::pair<btRigidBody*, Collidable*>(rigidBody, newCollidable)).second;

		if (inserted) return newCollidable;
		else return nullptr;
	}

	// Remove all blocks from the cube
	void clearAllBlocks() {
		std::map<btRigidBody*, Collidable*>::iterator it;
		it = all.begin();
		while (it != all.end()) {
			Collidable* obj = getFromRigidBody(it->second->getRigidBody());
			it = std::next(it);
			remove(obj);
		}
	}

	Collidable* getFromRigidBody(btRigidBody* rb) const {
		auto foundResult = all.find(rb);
		if (foundResult != all.end())
			return foundResult->second;
		else
			return nullptr;
	}

	// Remove unneeded collidables
	void pruneAll(float timePassed = 0) {
		std::map<btRigidBody*, Collidable*>::iterator it;
		it = all.begin();
		while (it != all.end()) {
			Collidable* obj = getFromRigidBody(it->second->getRigidBody());

			// Enforce bullet age restriction
			if (obj->isBullet()) obj->tick(timePassed);

			// Remove all collidables marked for deletion
			if (obj->isMarkedForDeletion()) {
				it = std::next(it);
				remove(obj);
			}
			else {
				it = std::next(it);
			}
		}
	}

private:
	void remove(Collidable* toDelete) {
		if (toDelete != nullptr) {
			std::map<btRigidBody*, Collidable*>::iterator it = all.find(toDelete->getRigidBody());
			if (it != all.end()) {
				all.erase(toDelete->getRigidBody());
				// Remove rigidbody from the simulation
				dynamicsWorld_->removeRigidBody(toDelete->getRigidBody());
				// Call destructor on all constituent pointers
				delete toDelete;
			}
		}
	}

	std::map<btRigidBody*, Collidable*> all;
	btDiscreteDynamicsWorld* dynamicsWorld_;
};

#endif