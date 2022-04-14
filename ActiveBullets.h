#ifndef ACTIVE_BULLETS_H
#define ACTIVE_BULLETS_H

#include "ticTacSnipe.h"
#include "Bullet.h"
#include <queue>

class ActiveBullets {
public:
	ActiveBullets(btDiscreteDynamicsWorld* dynamicsWorld) { dynamicsWorld_ = dynamicsWorld; };

	void registerBullet(Ogre::Entity* entity, btRigidBody* rigidBody, btCollisionShape* shape) {
		// Add new bullet to registry
		bullets.push(new Bullet(entity, rigidBody, shape));
		
		// Enforce bullet limit
		if (bullets.size() > MAX_BULLETS) removeOldestBullet();
	}
private:
	void removeOldestBullet() {
		Bullet* toDelete = bullets.front();

		if (toDelete != nullptr) {
			bullets.pop();
			// Remove bullet's rigidbody from the simulation
			dynamicsWorld_->removeRigidBody(toDelete->getRigidBody());
			// Call destructor on all of bullet's constituent pointers
			delete toDelete;
		}
	}

	std::queue<Bullet*> bullets;
	const int MAX_BULLETS = 10;
	btDiscreteDynamicsWorld* dynamicsWorld_;
};

#endif