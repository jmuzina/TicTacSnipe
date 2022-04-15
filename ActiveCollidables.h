#ifndef ACTIVE_COLLIDABLES_H
#define ACTIVE_COLLIDABLES_H

#include "ticTacSnipe.h"
#include "Collidable.h"
#include <queue>

class ActiveCollidables {
public:
	ActiveCollidables(btDiscreteDynamicsWorld* dynamicsWorld) { dynamicsWorld_ = dynamicsWorld; };

	void registerBullet(Ogre::Entity* entity, btRigidBody* rigidBody, btCollisionShape* shape) {
		// Add new collidable to registry
		bullets.push(new Collidable(entity, rigidBody, shape));
		
		// Enforce bullet limit
		if (bullets.size() > MAX_BULLETS) removeOldestBullet();
	}

	void registerMiscCollidable(Ogre::Entity* entity, btRigidBody* rigidBody, btCollisionShape* shape) {
		// Add new collidable to registry
		misc.push(new Collidable(entity, rigidBody, shape));
	}

	std::queue<Collidable*> getMisc() const { return misc; };
private:
	void removeOldestBullet() {
		Collidable* toDelete = bullets.front();

		if (toDelete != nullptr) {
			bullets.pop();
			// Remove rigidbody from the simulation
			dynamicsWorld_->removeRigidBody(toDelete->getRigidBody());
			// Call destructor on all constituent pointers
			delete toDelete;
		}
	}

	std::queue<Collidable*> bullets;
	std::queue<Collidable*> misc;
	const int MAX_BULLETS = 10;
	btDiscreteDynamicsWorld* dynamicsWorld_;
};

#endif