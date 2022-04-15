#ifndef TICTACTOEBOARD_H
#define TICTACTOEBOARD_H

#include "ticTacSnipe.h"
using namespace Ogre;

class TicTacToeBoard {
	
public:
	TicTacToeBoard(SceneManager* mSceneMgr, Vector3 spawnPosition, Vector3 spawnRotation, Vector3 scale) {
		baseBoardNode_ = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		Entity* baseBoard = mSceneMgr->createEntity("cube.mesh");
		baseBoardNode_->attachObject(baseBoard);
		baseBoardNode_->setScale(scale);
		baseBoardNode_->setPosition(spawnPosition);
		//baseBoardNode_->lookAt(spawnRotation, Node::TS_WORLD);

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
};

#endif