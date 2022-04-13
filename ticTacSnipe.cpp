// Tic Tac Snipe Main game script
// Anthony Moore & Joe Muzina
// Credit: Tutorial 2_9/3_9 from Alfred Shaker for environment setup, flat plane/light, BaseApplication
// basic flat grassy plane with a light.
// Camera movement is locked.

#include "ticTacSnipe.h"
#include "MotionState.h"

void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img);

TicTacSnipeApplication::TicTacSnipeApplication()
    :
    cameraSpawnPoint(Ogre::Vector3(0, 400, -1500)),
    cameraLookPoint(Ogre::Vector3(0, 400, 500))
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
    createBulletSim();
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

    CreateBullet(btVector3(camPos.x, camPos.y, camPos.z), 1.0f, btVector3(0.2, 0.2, 0.2), "Bullet");
    return true;
}


void TicTacSnipeApplication::createFrameListener()
{
    BaseApplication::createFrameListener();
}

void TicTacSnipeApplication::destroyScene()
{

}

bool TicTacSnipeApplication::frameRenderingQueued(const Ogre::FrameEvent& fe)
{
    bool ret = BaseApplication::frameRenderingQueued(fe);

    return ret;
}

void TicTacSnipeApplication::createBulletSim(void) {
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
    {
        
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
            1,
            pTerrain->getMinHeight(),
            pTerrain->getMaxHeight(),
            1,
            PHY_FLOAT,
            true);

        groundShape->setUseDiamondSubdivision(true);
        groundShape->setLocalScaling(localScaling);

        btRigidBody* mGroundBody = new btRigidBody(0, new btDefaultMotionState(), groundShape);

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

        CreateBullet(btVector3(2623, 500, 750), 1.0f, btVector3(0.3, 0.3, 0.3), "Cube0");
        //CreateCube(btVector3(2263, 150, 1200), 1.0f, btVector3(0.2, 0.2, 0.2), "Cube1");
        //CreateCube(btVector3(2253, 100, 1210), 1.0f, btVector3(0.2, 0.2, 0.2), "Cube2");
        //CreateCube(btVector3(2253, 200, 1210), 1.0f, btVector3(0.2, 0.2, 0.2), "Cube3");
        //CreateCube(btVector3(2253, 250, 1210), 1.0f, btVector3(0.2, 0.2, 0.2), "Cube4");
        //CreateCube(btVector3(1963, 150, 1660),1.0f,btVector3(0.2,0.2,0.2),"Cube1");

        
    }
}

void TicTacSnipeApplication::CreateBullet(const btVector3& Position, btScalar Mass, const btVector3& scale, char* name) {
    // empty ogre vectors for the cubes size and position
    Ogre::Vector3 size = Ogre::Vector3::ZERO;
    Ogre::Vector3 pos = Ogre::Vector3::ZERO;
    Ogre::SceneNode* boxNode;
    Ogre::Entity* bulletEntity;
    // Convert the bullet physics vector to the ogre vector
    pos.x = Position.getX();
    pos.y = Position.getY();
    pos.z = Position.getZ();

    bulletEntity = mSceneMgr->createEntity(name, Ogre::SceneManager::PT_SPHERE);
    //bulletEntity->setScale(Vector3(scale.x,scale.y,scale.z));
    bulletEntity->setCastShadows(true);
    boxNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    boxNode->attachObject(bulletEntity);
    boxNode->scale(Ogre::Vector3(scale.getX(), scale.getY(), scale.getZ()));
    //boxNode->setScale(Vector3(0.1,0.1,0.1));
    Ogre::AxisAlignedBox boundingB = bulletEntity->getBoundingBox();
    //Ogre::AxisAlignedBox boundingB = boxNode->_getWorldAABB();
    boundingB.scale(Ogre::Vector3(scale.getX(), scale.getY(), scale.getZ()));
    size = boundingB.getSize() * 0.95f;
    btTransform Transform;
    Transform.setIdentity();
    Transform.setOrigin(Position);
    MotionState* ms = new MotionState(Transform, boxNode);
    //Give the rigid body half the size
    // of our cube and tell it to create a BoxShape (cube)
    btVector3 HalfExtents(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
    btCollisionShape* Shape = new btBoxShape(HalfExtents);
    btVector3 LocalInertia;
    Shape->calculateLocalInertia(Mass, LocalInertia);
    btRigidBody* RigidBody = new btRigidBody(Mass, ms, Shape, LocalInertia);

    // Store a pointer to the Ogre Node so we can update it later
    RigidBody->setUserPointer((void*)(boxNode));

    // Add it to the physics world
    dynamicsWorld->addRigidBody(RigidBody);
    collisionShapes.push_back(Shape);
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