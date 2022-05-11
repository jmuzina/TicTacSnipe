// Tic Tac Snipe Main game script
// Anthony Moore & Joe Muzina
// Credit: Tutorial 2_9/3_9 from Alfred Shaker for environment setup, light, BaseApplication, as well as CEGUI and Bullet tutorials
// Camera movement is locked.

#include "ticTacSnipe.h"
#include "MotionState.h"
#include "Collidable.h"

Collidable* checkBullet = nullptr;
Collidable* checkBlock = nullptr;
bool spaceWasHit = false;
const String BASE_COLOR = "BaseWhite";

const Vector4 redVec = Vector4(1, 0, 0, 0);
const Vector4 blueVec = Vector4(0, 0, 1, 0);
const Vector4 greyVec = Vector4(0.25, 0.25, 0.25, 0);

std::map<int, Vector4> playerColors = {
        { 0, redVec },
        { 1, blueVec }
};

Ogre::MaterialPtr red = Ogre::MaterialPtr();
Ogre::MaterialPtr blue = Ogre::MaterialPtr();
Ogre::MaterialPtr grey = Ogre::MaterialPtr();

void setColor(Ogre::Entity* ent, Vector4 color) {
    if (color != Vector4::ZERO) {
        for (int i = 0; i < ent->getNumSubEntities(); ++i) {
            Ogre::SubEntity* subEntity = ent->getSubEntity(i);
            subEntity->setMaterialName(BASE_COLOR);
            
            if (color == redVec) {
                if (red == Ogre::MaterialPtr()) {
                    auto material_ = subEntity->getMaterial().get()->clone(ent->getName() + std::to_string(i));
                    material_->setDiffuse(color.x, color.y, color.z, color.w);
                    material_->setSpecular(color.x, color.y, color.z, color.w);
                    subEntity->setMaterialName(material_->getName());
                    red = material_;
                }
                else {
                    subEntity->setMaterialName(red.get()->getName());
                }
            }
            else if (color == blueVec) {
                if (blue == Ogre::MaterialPtr()) {
                    auto material_ = subEntity->getMaterial().get()->clone(ent->getName() + std::to_string(i));
                    material_->setDiffuse(color.x, color.y, color.z, color.w);
                    material_->setSpecular(color.x, color.y, color.z, color.w);
                    subEntity->setMaterialName(material_->getName());
                    blue = material_;
                }
                else {
                    subEntity->setMaterialName(blue.get()->getName());
                }
            }
            else if (color == greyVec) {
                if (grey == Ogre::MaterialPtr()) {
                    auto material_ = subEntity->getMaterial().get()->clone(ent->getName() + std::to_string(i));
                    material_->setDiffuse(color.x, color.y, color.z, color.w);
                    material_->setSpecular(color.x, color.y, color.z, color.w);
                    subEntity->setMaterialName(material_->getName());
                    grey = material_;
                }
                else {
                    subEntity->setMaterialName(grey.get()->getName());
                }
            }
        }
    }
    else {
        for (int i = 0; i < ent->getNumSubEntities(); ++i) {
            Ogre::SubEntity* subEntity = ent->getSubEntity(i);
            subEntity->setMaterialName(BASE_COLOR);
        }
    }
}

// Allow overwrite of default object collision behavior
// Credit: https://stackoverflow.com/questions/20300615/bullet-collision-callback-between-2-bodies
struct MyContactResultCallback : public btCollisionWorld::ContactResultCallback
{
    btScalar addSingleResult(btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0Wrap,
        int partId0,
        int index0,
        const btCollisionObjectWrapper* colObj1Wrap,
        int partId1,
        int index1)
    {
        if ((checkBullet != nullptr) && (checkBlock != nullptr)) {
            // Block was not hit by this bullet
            // If this isn't checked, the same bullet may "strike" the same block twice
            if (!checkBlock->alreadyHitBy(checkBullet) && (checkBlock->getOwnerId() == -1) && (!checkBullet->isMarkedForDeletion())) {
                checkBlock->markHit(checkBullet);
                setColor(checkBlock->getEntity(), playerColors.find(checkBullet->getOwnerId())->second);
                checkBullet->markForDeletion();
                spaceWasHit = true;
            }
        }

        return true;
    }
};

void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img);

TicTacSnipeApplication::TicTacSnipeApplication()
    :
    cameraSpawnPoint(Ogre::Vector3(2224, 212, 462)),
    cameraLookPoint(Ogre::Vector3(2228, 170, 806)),
    debugLabel(0)
{
}

TicTacSnipeApplication::~TicTacSnipeApplication()
{
}

void TicTacSnipeApplication::createScene()
{
    mCamera->setPosition(cameraSpawnPoint);
    mCamera->lookAt(cameraLookPoint);
    mCamera->setNearClipDistance(.1);

    bool infiniteClip =
        mRoot->getRenderSystem()->getCapabilities()->hasCapability(
            Ogre::RSC_INFINITE_FAR_PLANE);

    if (infiniteClip)
        mCamera->setFarClipDistance(0);
    else
        mCamera->setFarClipDistance(50000);

	/*
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

    Ogre::MeshManager::getSingleton().createPlane(
        "ground",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane,
        5000, 5000, 20, 20,
        true,
        1, 5, 5,
        Ogre::Vector3::UNIT_Z);

    Ogre::Entity* groundEntity = mSceneMgr->createEntity("ground");

    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);

    groundEntity->setCastShadows(false);

    // Materials found in env\Ogre10\sdk\media\materials\scripts\Examples.material
    groundEntity->setMaterialName("Examples/GrassFloor");
	*/

    // Lighting
    mSceneMgr->setAmbientLight(Ogre::ColourValue(.4, .4, .4));

    Ogre::Vector3 lightDir(.55, -.3, .75);
    lightDir.normalise();

    Ogre::Light* light = mSceneMgr->createLight("TestLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightDir);
    light->setDiffuseColour(Ogre::ColourValue(0.4, 0.4, 0.4));
    light->setSpecularColour(Ogre::ColourValue(.02, .02, .02));

    // Terrain
    mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();

    mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(
        mSceneMgr,
        Ogre::Terrain::ALIGN_X_Z,
        513, 12000.0);
    mTerrainGroup->setFilenameConvention(Ogre::String("terrain"), Ogre::String("dat"));
    mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);

    configureTerrainDefaults(light);

    for (long x = 0; x <= 0; ++x)
        for (long y = 0; y <= 0; ++y)
            defineTerrain(x, y);

    mTerrainGroup->loadAllTerrains(true);

    if (mTerrainsImported)
    {
        Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();

        while (ti.hasMoreElements())
        {
            Ogre::Terrain* t = ti.getNext()->instance;
            initBlendMaps(t);
        }
    }

    mTerrainGroup->freeTemporaryResources();
    CreateBulletSim();
    TicTacToeBoard* board = new TicTacToeBoard(mSceneMgr, dynamicsWorld, ActiveCollidables_, cameraLookPoint, Quaternion(1,0,1,0), Vector3(0.2, 1, 1));
    boards_.push_back(board);

    // Make all board dividers grey
    std::vector<Entity*> dividers = board->getDividers();
    for (int i = 0; i < dividers.size(); ++i) {
        setColor(dividers[i], greyVec);
    }
}

void TicTacSnipeApplication::configureTerrainDefaults(Ogre::Light* light)
{
    mTerrainGlobals->setMaxPixelError(8);
    mTerrainGlobals->setCompositeMapDistance(3000);

    mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
    mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
    mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());

    Ogre::Terrain::ImportData& importData = mTerrainGroup->getDefaultImportSettings();
    importData.terrainSize = 513;
    importData.worldSize = 12000.0;
    importData.inputScale = 600;
    importData.minBatchSize = 33;
    importData.maxBatchSize = 65;

    importData.layerList.resize(3);
    importData.layerList[0].worldSize = 100;
    importData.layerList[0].textureNames.push_back(
        "dirt_grayrocky_diffusespecular.dds");
    importData.layerList[0].textureNames.push_back(
        "dirt_grayrocky_normalheight.dds");
    importData.layerList[1].worldSize = 30;
    importData.layerList[1].textureNames.push_back(
        "grass_green-01_diffusespecular.dds");
    importData.layerList[1].textureNames.push_back(
        "grass_green-01_normalheight.dds");
    importData.layerList[2].worldSize = 200;
    importData.layerList[2].textureNames.push_back(
        "growth_weirdfungus-03_diffusespecular.dds");
    importData.layerList[2].textureNames.push_back(
        "growth_weirdfungus-03_normalheight.dds");
}

void TicTacSnipeApplication::defineTerrain(long x, long y)
{
    Ogre::String filename = mTerrainGroup->generateFilename(x, y);

    bool exists =
        Ogre::ResourceGroupManager::getSingleton().resourceExists(
            mTerrainGroup->getResourceGroup(),
            filename);

    if (exists)
        mTerrainGroup->defineTerrain(x, y);
    else
    {
        Ogre::Image img;
        getTerrainImage(x % 2 != 0, y % 2 != 0, img);
        mTerrainGroup->defineTerrain(x, y, &img);

        mTerrainsImported = true;
    }
}

void TicTacSnipeApplication::initBlendMaps(Ogre::Terrain* terrain)
{
    Ogre::Real minHeight0 = 70;
    Ogre::Real fadeDist0 = 40;
    Ogre::Real minHeight1 = 70;
    Ogre::Real fadeDist1 = 15;

    Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
    Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);

    float* pBlend0 = blendMap0->getBlendPointer();
    float* pBlend1 = blendMap1->getBlendPointer();

    for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
    {
        for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
        {
            Ogre::Real tx, ty;

            blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
            Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
            Ogre::Real val = (height - minHeight0) / fadeDist0;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend0++ = val;

            val = (height - minHeight1) / fadeDist1;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend1++ = val;
        }
    }

    blendMap0->dirty();
    blendMap1->dirty();
    blendMap0->update();
    blendMap1->update();

}

bool TicTacSnipeApplication::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id) {
    const Ogre::Vector3 camPos = mCamera->getPosition();

    const int playerId = (id == OIS::MouseButtonID(0) ? 0 : 1);

    // Alternate player turns
    if ((player1Turn && playerId == 0) || (!player1Turn && playerId == 1)) {
        player1Turn = !player1Turn;
        CreateBullet(btVector3(camPos.x, camPos.y + BULLET_SPAWN_Y_OFFSET, camPos.z), playerId);
    }
    return true;
}


void TicTacSnipeApplication::createFrameListener()
{
    BaseApplication::createFrameListener();
    debugLabel = mTrayMgr->createLabel(OgreBites::TL_BOTTOMRIGHT, "CamPos", "", 350);
    debugLabel->show();
}

void TicTacSnipeApplication::checkForWinner() {
    spaceWasHit = false;

    // Check all boards for whether a player has won them
    for (int boardNum = 0; boardNum < boards_.size(); ++boardNum) {
        const TicTacToeBoard* board = boards_[boardNum];
        const int winner = board->getWinner();

        // A player has won on this board
        if (winner != -1) {
            const std::vector<BoardSpace*> spaces = board->getSpaces();
            // Reset all board pieces on this board
            for (int spaceNum = 0; spaceNum < spaces.size(); ++spaceNum) {
                const BoardSpace* space = spaces[spaceNum];
                space->getCollidable()->clearHit();
                setColor(space->getCollidable()->getEntity(), Vector4::ZERO);
            }
        }
    }
}

bool TicTacSnipeApplication::frameStarted(const Ogre::FrameEvent& evt)
{
    if (boards_.size() > 0) {
        ActiveCollidables_->pruneAll(evt.timeSinceLastFrame);
        dynamicsWorld->stepSimulation(evt.timeSinceLastFrame);

        const btCollisionObjectArray colliders = dynamicsWorld->getCollisionObjectArray();
        for (int i = 0; i < colliders.size(); i++) {
            btCollisionObject* objA = colliders[i];
            btRigidBody* bodyA = btRigidBody::upcast(objA);

            if (bodyA && bodyA->getMotionState()) {
                btTransform trans;
                bodyA->getMotionState()->getWorldTransform(trans);

                void* userPointer = bodyA->getUserPointer();
                if (userPointer) {
                    btQuaternion orientation = trans.getRotation();
                    Ogre::SceneNode* sceneNode = static_cast<Ogre::SceneNode*>(userPointer);
                    checkBullet = ActiveCollidables_->getFromRigidBody(bodyA);
                    if (checkBullet->isBullet()) {
                        // check each other collider object
                        for (int j = 0; j < colliders.size(); ++j) {
                            if (i == j) continue; // ignore identical colliders

                            btCollisionObject* objB = colliders[j];
                            btRigidBody* bodyB = btRigidBody::upcast(objB);
                            checkBlock = ActiveCollidables_->getFromRigidBody(bodyB);

                            // If object A is a bullet and object B is a block, check if they collide.
                            if ((checkBullet != checkBlock) && (checkBlock != nullptr) && (!checkBlock->isBullet())) {
                                // callback will be called if A and B collide.
                                MyContactResultCallback callback;
                                dynamicsWorld->contactPairTest(bodyA, bodyB, callback);
                            }
                        }
                    }
                }
            }
        }
        // Check whether a space has been hit, and handle it if so
        if (spaceWasHit) checkForWinner();
    }
    return true;
}


void TicTacSnipeApplication::destroyScene()
{
    OGRE_DELETE mTerrainGroup;
    OGRE_DELETE mTerrainGlobals;
}

bool TicTacSnipeApplication::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
    bool ret = BaseApplication::frameRenderingQueued(fe);
    const Quaternion orientation = mCamera->getOrientation();
    //debugLabel->setCaption("Orientation: " + std::to_string(orientation.x) + ", " + std::to_string(orientation.y) + ", " + std::to_string(orientation.z));
    debugLabel->setCaption("CamPos: " + std::to_string(static_cast<int>(std::ceil(mCamera->getPosition().x))) + ", " + std::to_string(static_cast<int>(std::ceil(mCamera->getPosition().y))) + ", " + std::to_string(static_cast<int>(std::ceil(mCamera->getPosition().z))));

    return ret;
}

void TicTacSnipeApplication::CreateBulletSim(void) {
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    dispatcher = new   btCollisionDispatcher(collisionConfiguration);

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    overlappingPairCache = new btDbvtBroadphase();

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    solver = new btSequentialImpulseConstraintSolver;

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -100, 0));

    ///create a few basic rigid bodies
    // start with ground plane, 1500, 1500
    Ogre::Terrain* pTerrain = mTerrainGroup->getTerrain(0, 0);
    float* terrainHeightData = pTerrain->getHeightData();
    Ogre::Vector3 terrainPosition = pTerrain->getPosition();
    float* pDataConvert = new float[pTerrain->getSize() * pTerrain->getSize()];
    for (int i = 0; i < pTerrain->getSize(); i++)
        memcpy(
            pDataConvert + pTerrain->getSize() * i, // source
            terrainHeightData + pTerrain->getSize() * (pTerrain->getSize() - i - 1), // target
            sizeof(float) * (pTerrain->getSize()) // size
        );

    float metersBetweenVertices = pTerrain->getWorldSize() / (pTerrain->getSize() - 1); //edit: fixed 0 -> 1 on 2010-08-13
    btVector3 localScaling(metersBetweenVertices, 1, metersBetweenVertices);

    btHeightfieldTerrainShape* groundShape = new btHeightfieldTerrainShape(
        pTerrain->getSize(),
        pTerrain->getSize(),
        pDataConvert,
        1/*ignore*/,
        pTerrain->getMinHeight(),
        pTerrain->getMaxHeight(),
        1,
        PHY_FLOAT,
        true);

    groundShape->setUseDiamondSubdivision(true);
    groundShape->setLocalScaling(localScaling);

    mGroundBody = new btRigidBody(0, new btDefaultMotionState(), groundShape);

    mGroundBody->getWorldTransform().setOrigin(
        btVector3(
            terrainPosition.x,
            terrainPosition.y + (pTerrain->getMaxHeight() - pTerrain->getMinHeight()) / 2,
            terrainPosition.z));

    mGroundBody->getWorldTransform().setRotation(
        btQuaternion(
            Ogre::Quaternion::IDENTITY.x,
            Ogre::Quaternion::IDENTITY.y,
            Ogre::Quaternion::IDENTITY.z,
            Ogre::Quaternion::IDENTITY.w));

    dynamicsWorld->addRigidBody(mGroundBody);
    collisionShapes.push_back(groundShape);
    ActiveCollidables_ = new ActiveCollidables(dynamicsWorld);
}

void TicTacSnipeApplication::CreateBullet(const btVector3& collidablePosition, int playerId) {
    // empty ogre vectors for the cubes size and position
    Ogre::Vector3 size = Ogre::Vector3::ZERO;
    Ogre::Vector3 pos = Ogre::Vector3::ZERO;
    Ogre::SceneNode* boxNode;
    Ogre::Entity* boxentity;
    // Convert the bullet physics vector to the ogre vector
    pos.x = collidablePosition.getX();
    pos.y = collidablePosition.getY();
    pos.z = collidablePosition.getZ();
    boxentity = mSceneMgr->createEntity("sphere.mesh");
    
    //boxentity->setScale(Vector3(scale.x,scale.y,scale.z));
    boxentity->setCastShadows(true);

    boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    boxNode->attachObject(boxentity);
    boxNode->scale(Ogre::Vector3(BULLET_SIZE.getX(), BULLET_SIZE.getY(), BULLET_SIZE.getZ()));

    Ogre::AxisAlignedBox boundingB = boxentity->getBoundingBox();

    boundingB.scale(Ogre::Vector3(BULLET_SIZE.getX(), BULLET_SIZE.getY(), BULLET_SIZE.getZ()));
    size = boundingB.getSize() * 0.95f;
    btTransform Transform;
    Transform.setIdentity();
    Transform.setOrigin(collidablePosition);
    MotionState* ms = new MotionState(Transform, boxNode);

    btVector3 HalfExtents(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
    btCollisionShape* Shape = new btBoxShape(HalfExtents);
    btVector3 LocalInertia;
    Shape->calculateLocalInertia(BULLET_MASS, LocalInertia);
    btRigidBody* RigidBody = new btRigidBody(BULLET_MASS, ms, Shape, LocalInertia);

    // Store a pointer to the Ogre Node so we can update it later
    RigidBody->setUserPointer((void*)(boxNode));

    // Add it to the physics world
    dynamicsWorld->addRigidBody(RigidBody);
    collisionShapes.push_back(Shape);
    Collidable* createdBullet = ActiveCollidables_->registerCollidable(boxentity, RigidBody, Shape, playerId, playerColors.find(playerId)->second);

    const Vector3 forward = getForwardAngle();
    const btVector3 btForward = btVector3(forward.x, forward.y, forward.z);

    RigidBody->applyImpulse(btForward * BULLET_FORCE, btVector3(0, 0, 0));
    RigidBody->applyTorqueImpulse(btForward * BULLET_SPAWN_Y_OFFSET);
}

void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
    img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
}



#if Ogre_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
    int main(int argc, char* argv[])
#endif
    {
        // Create application object
        TicTacSnipeApplication app;

        try {
            app.go();
        }
        catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif