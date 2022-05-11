//TicTacToeBoard.h

#ifndef TICTACTOEBOARD_H
#define TICTACTOEBOARD_H

#include "ticTacSnipe.h"
#include "Collidable.h"
#include "ActiveCollidables.h"
#include "MotionState.h"
#include "boardSpace.h"

using namespace Ogre;

class TicTacToeBoard {

public:
    TicTacToeBoard(SceneManager* mSceneMgr, btDiscreteDynamicsWorld* dynamicsWorld, ActiveCollidables* ActiveCollidables, Vector3 spawnPosition, Quaternion spawnRotation, Vector3 scale) {
        mSceneMgr_ = mSceneMgr;
        dynamicsWorld_ = dynamicsWorld;
        ActiveCollidables_ = ActiveCollidables;
        baseBoardNode_ = mSceneMgr->getRootSceneNode()->createChildSceneNode();

        baseBoardNode_->setPosition(spawnPosition);
        baseBoardNode_->setOrientation(spawnRotation);
        baseBoardNode_->scale(scale);

        for (int i = 0; i < 9; ++i) {
            constructBoardSpace(i, spawnPosition, spawnRotation);
        }

        // Create board dividers
        constructDivider(Vector3(0, 0, OFFSET), Vector3(0, 1, 0), VERTICAL_SCALE);
        constructDivider(Vector3(0, 0, -OFFSET), Vector3(0, 1, 0), VERTICAL_SCALE);
        constructDivider(Vector3(0, OFFSET, 0), Vector3(0, 0, 1), HORIZONTAL_SCALE);
        constructDivider(Vector3(0, -OFFSET, 0), Vector3(0, 0, 1), HORIZONTAL_SCALE);
    }

    int getWinner() const {
        // Check for wins along rows
        for (int rowNum = 0; rowNum < 3; ++rowNum) {
            const int rowWinner = winnerOnRow(rowNum);
            if (rowWinner != -1) return rowWinner;
        }

        // Check for wins along columns
        for (int colNum = 0; colNum < 3; ++colNum) {
            const int columnWinner = winnerOnColumn(colNum);
            if (columnWinner != -1) return columnWinner;
        }

        // Check for wins along the diagonals
        const int diagonalWinner = winnerOnDiagonal();

        if (diagonalWinner == -1) {
            // check for draw
            for (int i = 0; i < 9; ++i) {
                const BoardSpace* space = spaces_[i];
                // there is a tile that has not been hit yet; game still in progress
                if (space->getCollidable()->getOwnerId() == -1) {
                    return -1;
                }
            }
            // All tiles have been hit and no winner found; draw.
            return -2;
        }
        // Winner along diagonal
        else return diagonalWinner;
    }


    std::vector<BoardSpace*> getSpaces() const {
        return spaces_;
    }

    std::vector<Entity*> getDividers() const {
        return dividers_;
    }


private:
    int winnerOnRow(int rowNum) const {
        if (rowNum < 0 || rowNum > 2) return -1;

        const int firstIndex = 3 * rowNum;
        const BoardSpace* firstSpace = spaces_[firstIndex];
        const BoardSpace* secondSpace = spaces_[firstIndex + 1];

        // First and second tiles are owned by different players; there cannot be a winner in this row
        if (firstSpace->getCollidable()->getOwnerId() != secondSpace->getCollidable()->getOwnerId())
            return -1;

        const BoardSpace* thirdSpace = spaces_[firstIndex + 2];

        // If the third tile matches the second tile, and the first tile already matches the second tile, there is a winner
        if (secondSpace->getCollidable()->getOwnerId() == thirdSpace->getCollidable()->getOwnerId())
            return firstSpace->getCollidable()->getOwnerId();

        // Second tile and third tile did not match, so there is no winner in this row
        return -1;
    }

    int winnerOnColumn(int colNum) const {
        if (colNum < 0 || colNum > 2) return -1;

        const BoardSpace* firstSpace = spaces_[colNum];
        const BoardSpace* secondSpace = spaces_[colNum + 3];

        // First and second tiles are owned by different players; there cannot be a winner in this column
        if (firstSpace->getCollidable()->getOwnerId() != secondSpace->getCollidable()->getOwnerId())
            return -1;

        const BoardSpace* thirdSpace = spaces_[colNum + 6];

        // If the third tile matches the second tile, and the first tile already matches the second tile, there is a winner
        if (secondSpace->getCollidable()->getOwnerId() == thirdSpace->getCollidable()->getOwnerId())
            return firstSpace->getCollidable()->getOwnerId();

        // Second tile and third tile did not match, so there is no winner in this column
        return -1;
    }

    int winnerOnDiagonal() const {
        const BoardSpace* topLeft = spaces_[0];
        const BoardSpace* center = spaces_[4];

        // top left and center are owned by the same player, check the bottom right
        if (topLeft->getCollidable()->getOwnerId() == center->getCollidable()->getOwnerId() && topLeft->getCollidable()->getOwnerId() != -1) {
            const BoardSpace* bottomRight = spaces_[8];

            // Bottom right and center are owned by the same player, so top left also matches. Winner along top left to bottom right diagonal.
            if (center->getCollidable()->getOwnerId() == bottomRight->getCollidable()->getOwnerId())
                return topLeft->getCollidable()->getOwnerId();
        }

        const BoardSpace* topRight = spaces_[2];
        // top right and center are owned by the same player, check the bottom right
        if (topRight->getCollidable()->getOwnerId() == center->getCollidable()->getOwnerId() && topRight->getCollidable()->getOwnerId() != -1) {
            const BoardSpace* bottomLeft = spaces_[6];

            // Bottom left and center are owned by the same player, so top right also matches. Winner along bottom left to top right diagonal
            if (center->getCollidable()->getOwnerId() == bottomLeft->getCollidable()->getOwnerId())
                return bottomLeft->getCollidable()->getOwnerId();
        }


        return -1;
    }

    void constructDivider(Vector3 spawnPosition, Vector3 spawnRotation, Vector3 scale) {
        SceneNode* childBoardNode = baseBoardNode_->createChildSceneNode();
        Entity* board = mSceneMgr_->createEntity("cube.mesh");
        childBoardNode->attachObject(board);
        childBoardNode->setScale(scale);
        childBoardNode->setPosition(spawnPosition);
        childBoardNode->rotate(spawnRotation, Ogre::Degree(90));

        dividers_.push_back(board);

        //boards_.push_back(childBoardNode);
    };

    void constructBoardSpace(int spaceNumber, Vector3 spawnPosition, Quaternion spawnRotation) {

        BoardSpace* newBoardSpace = new BoardSpace(baseBoardNode_, mSceneMgr_, dynamicsWorld_, ActiveCollidables_, spawnPosition + spaceOffsets[spaceNumber], spawnRotation, "boardSpace" + std::to_string(spaceNumber));
        if (newBoardSpace != nullptr) spaces_.push_back(newBoardSpace);

        //Collidable* createdCollidable = ActiveCollidables_->registerCollidable(baseBoard, RigidBody, Shape);
    }

    std::vector<BoardSpace*> spaces_ = std::vector<BoardSpace*>();
    SceneNode* baseBoardNode_;
    const int OFFSET = 20;
    const Vector3 VERTICAL_SCALE = Vector3(0.2, 1.15, 0.3);
    const Vector3 HORIZONTAL_SCALE = Vector3(0.2, 0.3, 1.15);
    const int BOARD_MASS = MAXINT;
    SceneManager* mSceneMgr_;
    btDiscreteDynamicsWorld* dynamicsWorld_;
    ActiveCollidables* ActiveCollidables_;

    std::vector<Entity*> dividers_;

    Vector3 spaceOffsets[9] = {
        Vector3(40, 35, 5),         // top left
        Vector3(0, 35, 5),          // top middle
        Vector3(-40, 35, 5),        // top right
        Vector3(40, -5, 5),         // center left
        Vector3(0, -5, 5),          // center
        Vector3(-40, -5, 5),        // center right
        Vector3(40, -45, 5),        // bottom left
        Vector3(0, -45, 5),         // bottom middle
        Vector3(-40, -45, 5)        // bottom right
    };
};

#endif