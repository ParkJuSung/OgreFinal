#include "TutorialApplication.h"

#include <OgreException.h>
#include <OgreConfigFile.h>
#include <OgreRenderWindow.h>
#include <OgreCamera.h> 
#include <OgreViewport.h> 
#include <OgreSceneManager.h>
#include <OgreEntity.h>
#include <OgreMeshManager.h>

#include<windows.h>
#include<playsoundapi.h>
using namespace Ogre;
#pragma comment(lib, "winmm.lib")

#define MUSIC "C:\\Users\\parkjoosung\\Downloads\\OgreSDK_vc11_v1-9-0\\shotsound.wav"
#define MUSIC2 "D:\\unity\\OrpheusDestiny (2)\\Assets\\Resources\\Sound\\MenuBGM.wav"
#define MUSIC3 "D:\\unity\\OrpheusDestiny (2)\\Assets\\Resources\\3DModel\\BossMonster\\audio\\cavewind1.wav"

#define CAMERA_NAME "SceneCamera"

RobotState robotstate[100];
TutorialApplication::TutorialApplication(void) : 
	mRoot(0),
	mCamera(0),
	mSceneMgr(0),
	mWindow(0),
	mCameraMan(0),
	mMouse(0),
	mKeyboard(0),
	mResourcesCfg(Ogre::StringUtil::BLANK),
	mPluginsCfg(Ogre::StringUtil::BLANK),
	mCamNode(0),
	mPlayerMove(500),
	mPlayerDirection(Vector3::ZERO),
	mMainSceneMgr(0),
	mEndSceneMgr(0),
	mSceneNumber(0),
	mRobotMove(250),
	 mInfoLabel(0),
	 mTrayMgr(0),
	 HP(50),
	 score(0),
	 ninjaEntity(0),
	fireParticle(0)

{
	for(int i=0;i<100;i++)
	{
		robotNode[i] = 0;
		robotEntity[i] = 0;
		mRobotDirection[i] = Vector3::ZERO;
	}
}


TutorialApplication::~TutorialApplication(void)
{
	
	if (mCameraMan) delete mCameraMan;

	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this); 
	windowClosed(mWindow);
	delete mRoot;
}


bool TutorialApplication::configure(void)
{
	if(mRoot->showConfigDialog())
    {
        mWindow = mRoot->initialise(true, "TutorialApplication Render Window");
		return true;
    }
	else
    {
        return false;
    }
}

void TutorialApplication::chooseSceneManager(void)
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC,"play");
	mMainSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "main");
	mEndSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "end");

	    mOverlaySystem = new Ogre::OverlaySystem();
		mSceneMgr->addRenderQueueListener(mOverlaySystem);
		mMainSceneMgr->addRenderQueueListener(mOverlaySystem);
		mEndSceneMgr->addRenderQueueListener(mOverlaySystem);
}

void TutorialApplication::createCamera(void)
{
	mSceneMgr->createCamera(CAMERA_NAME); 
	mMainSceneMgr->createCamera(CAMERA_NAME);
	mEndSceneMgr->createCamera(CAMERA_NAME);
	mCamera = mMainSceneMgr->createCamera("MainCam"); 

	mCamera->lookAt(0, 0, -300);
	mCamera->setNearClipDistance(5);
	mCamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("camNode",Vector3(0,0,80));
	mCamNode->attachObject(mCamera);
	mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}

void TutorialApplication::createFrameListener(void)
{

	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***"); 
	OIS::ParamList pl; 
	size_t windowHnd = 0; 
	std::ostringstream windowHndStr;
	
	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd; 
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	
	mInputMgr = OIS::InputManager::createInputSystem( pl );

	mKeyboard = static_cast<OIS::Keyboard*>(mInputMgr->createInputObject( OIS::OISKeyboard, true )); 
	mMouse = static_cast<OIS::Mouse*>(mInputMgr->createInputObject( OIS::OISMouse, true ));
	
	
	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	// Set initial mouse clipping size 
	windowResized(mWindow);
	
	// Register as a Window listener 
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
	
	// Register as a Frame listener 
	mInputContext.mKeyboard = mKeyboard;
    mInputContext.mMouse = mMouse;
	
	    mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, mInputContext, this);
		mTrayMgr->showFrameStats(OgreBites::TL_TOPLEFT);
		mTrayMgr->showLogo(OgreBites::TL_TOPRIGHT);
	    mTrayMgr->showCursor();

		mInfoLabel = mTrayMgr->createLabel( OgreBites::TL_TOP, // Label location 
		"TerrainInfo", // Label name 
		"Press Space to start.", // Label caption 
		350); // Label width
		mScoreLabel = mTrayMgr->createLabel( OgreBites::TL_TOPRIGHT, // Label location 
		"score", // Label name 
		"Press Space to start.", // Label caption 
		350); // Label width
		mScoreLabel->hide();

	mRoot->addFrameListener(this);
}

void TutorialApplication::createViewports(void)
{
	Ogre::Viewport* vp = mWindow->addViewport(mCamera); 
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	mCamera->setAspectRatio( Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void TutorialApplication::setupResources(void)
{
	Ogre::ConfigFile cf; 
	cf.load(mResourcesCfg);
	
	Ogre::String name, locType; 
	Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();

	while (secIt.hasMoreElements()) 
	{ 
		Ogre::ConfigFile::SettingsMultiMap* settings = secIt.getNext(); 
		Ogre::ConfigFile::SettingsMultiMap::iterator it;

		for (it = settings->begin(); it != settings->end(); ++it) 
		{ 
			locType = it->first; 
			name = it->second;
		    
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, locType);
		}
	}
}
void TutorialApplication::createResourceListener(void)
{

}

void TutorialApplication::loadResources(void)
{
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}


void TutorialApplication::go() 
{ 
	#ifdef _DEBUG 
		mResourcesCfg = "resources_d.cfg"; 
		mPluginsCfg = "plugins_d.cfg"; 
	#else 
		mResourcesCfg = "resources.cfg"; 
		mPluginsCfg = "plugins.cfg"; 
	#endif	
		

	
	// The first parameter determines whether or not Ogre will create a RenderWindow for us. 
	

	if(!setup())
		return;
	
	mRoot->startRendering();

	destroyScene();
}

bool TutorialApplication::setup()
{
	mRoot = new Ogre::Root(mPluginsCfg);

	setupResources();

	bool carryOn = configure();
    if (!carryOn) return false;

	chooseSceneManager();
	createCamera();
	createViewports();

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	loadResources();

	createScene();

	createFrameListener();

	return true;
}

bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent& fe) 
{ 
	static float time = 0;

	if (mWindow->isClosed()) 
		return false;
	
	if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) 
		return false;
	
	mCameraMan->frameRenderingQueued(fe);
	 mTrayMgr->frameRenderingQueued(fe);

	mKeyboard->capture(); 
	mMouse->capture();

	if(mSceneNumber==1)
	{

		if(HP<20)
		{
			if(!(fireParticle->isAttached()))
			{
				fireParticleNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Fireworks");
				fireParticleNode->attachObject(fireParticle);
			}
			time += fe.timeSinceLastFrame;
			if(time>=3)
			{
				HP--;
				std::string a = "HP :" + Ogre::StringConverter::toString(HP);
				mInfoLabel->setCaption(a);
				time=0;
			}
		}
		if(fireParticle->isAttached())
		{
				if(HP>=20)
				{
					fireParticleNode->detachObject(fireParticle);
					mSceneMgr->destroyParticleSystem(fireParticle);
					mSceneMgr->destroySceneNode(fireParticleNode);
					fireParticle = mSceneMgr->createParticleSystem("Examples2","Examples/GreenyNimbus");

				}
		}
		gunNode->translate(mPlayerDirection * fe.timeSinceLastFrame,Node::TS_LOCAL);
		mCamera->setPosition(gunNode->getPosition().x+49,gunNode->getPosition().y+500,gunNode->getPosition().z-500);

		for(int i=0;i<100;i++)
		{
			if(robotNode[i] != 0)
			{
				robotMove(i,fe);
				robotNode[i]->translate(mRobotDirection[i]*fe.timeSinceLastFrame,Node::TS_WORLD);
			}

			
		}

		robotAniControl(fe);
		lightrotation(fe);
		if(HP<=0)
		{
			PlaySound(NULL, 0, 0);
			PlaySound(TEXT(MUSIC3),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
			setupViewport(mEndSceneMgr);
			mInfoLabel->setCaption("Game Over");
			mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
			mSceneNumber++;
		}
	}

	return true;
}

bool TutorialApplication::keyPressed(const OIS::KeyEvent& ke)
{
	std::string a = Ogre::StringConverter::toString(mCamera->getPosition());
	switch (ke.key)
	{
	case OIS::KC_W:
		mPlayerDirection.z = -mPlayerMove;
		break;
	case OIS::KC_S:
		mPlayerDirection.z = mPlayerMove;
		break;
	case OIS::KC_A:
		mPlayerDirection.x = -mPlayerMove;
		break;
	case OIS::KC_D:
		mPlayerDirection.x = mPlayerMove;
		break;
	case OIS::KC_SPACE:
		if(mSceneNumber==0)
		{
			PlaySound(NULL, 0, 0);
			mSceneNumber++;
			setupViewport(mSceneMgr);
		}
		if(mSceneNumber==1)
		{
			std::string a = "HP : " + Ogre::StringConverter::toString(HP);
			mInfoLabel->setCaption(a);
			mScoreLabel->show();
			mScoreLabel->setCaption("Score : 0");
			mTrayMgr->hideLogo();
		}
		break;
	case OIS::KC_V:
		MessageBox(NULL,a.c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		break;
	case OIS::KC_1:
		gunNode->detachAllObjects();
		gunNode->setScale(1800,1500,3500);
		gunNode->attachObject(lazerGunNodeEntity);
		break;
	case OIS::KC_2:
		gunNode->detachAllObjects();
		gunNode->setScale(1500,1500,2000);
		gunNode->attachObject(machineGunNodeEntity);
		break;
	case OIS::KC_3:
		gunNode->detachAllObjects();
		gunNode->setScale(3000,1500,6500);
		gunNode->attachObject(pistolGunNodeEntity);
		break;
	}
	mCameraMan->injectKeyDown(ke);

	return true;
}

bool TutorialApplication::keyReleased(const OIS::KeyEvent &ke)
{
	switch (ke.key)
	{
	case OIS::KC_W:
		mPlayerDirection.z = 0;
		break;
	case OIS::KC_S:
		mPlayerDirection.z =0;
		break;
	case OIS::KC_A:
		mPlayerDirection.x = 0; 
		break;
	case OIS::KC_D:
		mPlayerDirection.x = 0;
		break;
	}
mCameraMan->injectKeyUp(ke);
    return true;
}

bool TutorialApplication::mouseMoved(const OIS::MouseEvent& me) 
{
    mCameraMan->injectMouseMove(me);
	if(mSceneNumber==1)
	{
		gunNode->yaw(Ogre::Degree(-me.state.X.rel *0.15));

		 mMouseX = (Real)me.state.X.abs / (Real)me.state.width;
		mMouseY = (Real)me.state.Y.abs / (Real)me.state.height;

		mTrayMgr->getCursorContainer()->setPosition(mWindow->getWidth()/2 - mMouseX-0.042f, mWindow->getHeight()/2 - mMouseY+0.004f);
	}
	return true;
}


bool TutorialApplication::mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	if(mSceneNumber==1)
	{
		static int robotCount = 4;
		Ray ray = mTrayMgr->getCursorRay(mCamera);
		Ogre::RaySceneQuery *mRaySceneQuery;
		mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
		for(int i=0;i<100;i++)
		{
			std::pair<bool,Real> isRobotCol = ray.intersects(robotNode[i]->_getWorldAABB());
			if(isRobotCol.first)
			{
				robotstate[i] = die;
				score++; 
				std::string a = "Score : " + Ogre::StringConverter::toString(score);
				mScoreLabel->setCaption(a);
				if(score%5==0)
				{

					if(!(ninjaEntity->isAttached()))
					{

						int	rnd = rand() %2;
						ninjaEntity->setCastShadows(false);
						switch (rnd)
						{
							case 0:
								ninjaNode->setPosition(-746,0,30);
								ninjaNode->attachObject(ninjaEntity);
								break;
							case 1:
								ninjaNode->setPosition(900,0,65);
								ninjaNode->attachObject(ninjaEntity);
								break;
						}
					}
					if(robotCount<100)
					{
						int rnd = rand() %4;
						switch (rnd)
						{
							case 0:
								robotNode[robotCount]->setPosition(1500,0,1550);
								robotNode[robotCount]->attachObject(robotEntity[robotCount]);
								break;
							case 1:
								robotNode[robotCount]->setPosition(1700,0,-1400);
								robotNode[robotCount]->attachObject(robotEntity[robotCount]);
								break;
							case 2:
								robotNode[robotCount]->setPosition(-1180,0,-1750);
								robotNode[robotCount]->attachObject(robotEntity[robotCount]);
								break;
							case 3:
								robotNode[robotCount]->setPosition(-1500,0,1180);
								robotNode[robotCount]->attachObject(robotEntity[robotCount]);
								break;
						}
						robotCount++;
					}
				}
				break;
			}
			if(ninjaEntity->isAttached())
			{
				std::pair<bool,Real> isItemCol = ray.intersects(ninjaNode->_getWorldAABB());
				if(isItemCol.first)
				{
					ninjaNode->detachObject(ninjaEntity);
					mSceneMgr->destroyEntity(ninjaEntity);
					mSceneMgr->destroySceneNode(ninjaNode);
					ninjaEntity = mSceneMgr->createEntity("ninja.mesh");
					ninjaNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("fixItem");

					if(HP>=35)
						HP=50;
					else
						HP+=15;

					std::string a = "HP :" + Ogre::StringConverter::toString(HP);
					mInfoLabel->setCaption(a);
				}
			}
		}

		mSceneMgr->destroyQuery(mRaySceneQuery);
		//PlaySound(NULL,0,0);
		PlaySound(TEXT(MUSIC),NULL,SND_ASYNC);
	}
	return true;
}

bool TutorialApplication::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	//MessageBox(NULL,a.c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	return true;
}
void TutorialApplication::createScene(void)
{

mSceneMgr->setAmbientLight(Ogre::ColourValue(.2, .2, .2));

Ogre::Plane plane(Vector3::UNIT_Y, -10);
Ogre::MeshManager::getSingleton().createPlane("plane",
ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
4000,4000,200,200,true,1,5,5,Vector3::UNIT_Z);


 ent = mSceneMgr->createEntity("LightPlaneEntity", "plane");
mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
ent->setMaterialName("Examples/BeachStones");
planeNode = mSceneMgr->createSceneNode("PlaneNode");
mSceneMgr->getRootSceneNode()->addChild(planeNode);

Ogre::Entity* tudorEntity = mSceneMgr->createEntity("tudorhouse.mesh");
tudorNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "TudorNode"); 
tudorNode->setPosition(0,600,0);
tudorNode->showBoundingBox(true);
tudorEntity->setCastShadows(false);
tudorNode->attachObject(tudorEntity);

machineGunNodeEntity = mSceneMgr->createEntity("machine_gun.mesh");
machineGunNodeEntity->setCastShadows(true);
gunNode = mSceneMgr->getRootSceneNode() ->createChildSceneNode("gunNode"); 
gunNode->setScale(1500, 1500, 1500);
gunNode->yaw(Degree(180));
gunNode->setPosition(0,0,1500);
gunNode->attachObject(machineGunNodeEntity);

lazerGunNodeEntity = mSceneMgr->createEntity("lazer.mesh");
pistolGunNodeEntity = mSceneMgr->createEntity("pistol.mesh");

Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
pointLight->setType(Ogre::Light::LT_POINT);
pointLight->setDiffuseColour(0.2, 0.2, 0.2);
pointLight->setPosition(Ogre::Vector3(0, 500, -1500));
pointLight->setSpotlightOuterAngle(Ogre::Degree(10.0f));

mCamera->yaw(Degree(180));

mSceneMgr->setSkyBox(true, "Examples/StormySkyBox", 10000,false); 

snowParticle = mSceneMgr->createParticleSystem("Examples","Snow");
snowParticleNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("SnowParticle");
snowParticleNode->setPosition(0,500,0);
snowParticleNode->attachObject(snowParticle);


for(int i=0;i<100;i++)
	robotstate[i] = idle;

createLight(planeNode);
setupCreateEnemy();
mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

mMainSceneMgr->setSkyBox(true,"Examples/SpaceSkyBox"); 
mEndSceneMgr->setSkyBox(true,"Examples/EarlyMorningSkyBox");
PlaySound(TEXT(MUSIC2),NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
fireParticle = mSceneMgr->createParticleSystem("Examples2","Fire");

itemMachineGunNodeEntity = mSceneMgr->createEntity("machine_gun.mesh");

itemPistolGunNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("itemPistolGunNode");

itemLazerGunNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("itemLazerGunNode");


	ninjaEntity = mSceneMgr->createEntity("ninja.mesh");
	ninjaNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("fixItem");

	for(int i=0;i<10;i++)
	{
		treeEntiy[i] =mSceneMgr->createEntity("tree.mesh");
		leavesEntiy[i] = mSceneMgr->createEntity("LowPolyTree.mesh");
		leavesEntiy[i]->setMaterialName("Tree_Leaf");
		treeEntiy[i]->setMaterialName("Tree_Bark");
		treeEntiy[i]->setCastShadows(false);
		leavesEntiy[i]->setCastShadows(false);
		treeNode[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("treeNode" + Ogre::StringConverter::toString(i));
		leavesNode[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("LowPolyTreeNode" + Ogre::StringConverter::toString(i));
	}
		treeNode[0]->setPosition(-1650,0,-180);
		treeNode[0]->setScale(40,50,40);
		treeNode[0]->pitch(Ogre::Degree(-90));
		treeNode[0]->attachObject(treeEntiy[0]);
		leavesNode[0]->setPosition(-1650,500,-380);
		leavesNode[0]->setScale(100,100,60);
		leavesNode[0]->attachObject(leavesEntiy[0]);

		treeNode[1]->setPosition(-1650,0,-800);
		treeNode[1]->setScale(40,50,40);
		treeNode[1]->pitch(Ogre::Degree(-90));
		treeNode[1]->attachObject(treeEntiy[1]);
		leavesNode[1]->setPosition(-1650,500,-1000);
		leavesNode[1]->setScale(100,100,60);
		leavesNode[1]->attachObject(leavesEntiy[1]);

		treeNode[2]->setPosition(-1650,0,320);
		treeNode[2]->setScale(40,50,40);
		treeNode[2]->pitch(Ogre::Degree(-90));
		treeNode[2]->attachObject(treeEntiy[2]);
		leavesNode[2]->setPosition(-1650,500,120);
		leavesNode[2]->setScale(100,100,60);
		leavesNode[2]->attachObject(leavesEntiy[2]);

		treeNode[3]->setPosition(-1650,0,720);
		treeNode[3]->setScale(40,50,40);
		treeNode[3]->pitch(Ogre::Degree(-90));
		treeNode[3]->attachObject(treeEntiy[3]);
		leavesNode[3]->setPosition(-1650,500,520);
		leavesNode[3]->setScale(100,100,60);
		leavesNode[3]->attachObject(leavesEntiy[3]);


		treeNode[4]->setPosition(1600,0,-34);
		treeNode[4]->setScale(40,50,40);
		treeNode[4]->pitch(Ogre::Degree(-90));
		treeNode[4]->attachObject(treeEntiy[4]);
		leavesNode[4]->setPosition(1600,500,-234);
		leavesNode[4]->setScale(60,100,60);
		leavesNode[4]->attachObject(leavesEntiy[4]);

		treeNode[5]->setPosition(1600,0,-500);
		treeNode[5]->setScale(40,50,40);
		treeNode[5]->pitch(Ogre::Degree(-90));
		treeNode[5]->attachObject(treeEntiy[5]);
		leavesNode[5]->setPosition(1600,500,-700);
		leavesNode[5]->setScale(100,100,60);
		leavesNode[5]->attachObject(leavesEntiy[5]);

		treeNode[6]->setPosition(1600,0,-800);
		treeNode[6]->setScale(40,50,40);
		treeNode[6]->pitch(Ogre::Degree(-90));
		treeNode[6]->attachObject(treeEntiy[6]);
		leavesNode[6]->setPosition(1600,500,-1000);
		leavesNode[6]->setScale(100,100,60);
		leavesNode[6]->attachObject(leavesEntiy[6]);

		treeNode[7]->setPosition(1600,0,200);
		treeNode[7]->setScale(40,50,40);
		treeNode[7]->pitch(Ogre::Degree(-90));
		treeNode[7]->attachObject(treeEntiy[7]);
		leavesNode[7]->setPosition(1600,500,0);
		leavesNode[7]->setScale(100,100,60);
		leavesNode[7]->attachObject(leavesEntiy[7]);
return;

}
void TutorialApplication::setupCreateEnemy()
{
	for(int i=0;i<100;i++)
	{
		std::string name = "Robot" + Ogre::StringConverter::toString(i);
		robotEntity[i] = mSceneMgr->createEntity(name, "robot.mesh");
		std::string name2 = "RobotNode" + Ogre::StringConverter::toString(i);
		robotNode[i] = mSceneMgr->getRootSceneNode() ->createChildSceneNode(name2); 
	}

	robotNode[0]->setScale(3,3,3);
	robotNode[0]->setPosition(1500,0,1550);
	robotNode[0]->lookAt(planeNode->getPosition(),Node::TS_WORLD,Vector3::UNIT_X);
	robotEntity[0]->setCastShadows(true);
	robotNode[0]->showBoundingBox(true); 
	robotNode[0]->attachObject(robotEntity[0]);
	

	robotNode[1]->setScale(3,3,3);
	robotNode[1]->setPosition(1700,0,-1400);
	robotNode[1]->lookAt(planeNode->getPosition(),Node::TS_WORLD,Vector3::UNIT_X);
	robotEntity[1]->setCastShadows(true);
	robotNode[1]->showBoundingBox(true); 
	robotNode[1]->attachObject(robotEntity[1]);
	

	robotNode[2]->setScale(3,3,3);
	robotNode[2]->setPosition(-1180,0,-1750);
	robotNode[2]->lookAt(planeNode->getPosition(),Node::TS_WORLD,Vector3::UNIT_X);
	robotEntity[2]->setCastShadows(true);
	robotNode[2]->showBoundingBox(true); 
	robotNode[2]->attachObject(robotEntity[2]);

	robotNode[3]->setScale(3,3,3);
	robotNode[3]->setPosition(-1500,0,1180);
	robotNode[3]->lookAt(planeNode->getPosition(),Node::TS_WORLD,Vector3::UNIT_X);
	robotEntity[3]->setCastShadows(true);
	robotNode[3]->showBoundingBox(true); 
	robotNode[3]->attachObject(robotEntity[3]);


}
void TutorialApplication::createLight(Ogre::SceneNode* planeNode)
{
	
	light1 = mSceneMgr->createLight("Light1"); 
	light1->setType(Ogre::Light::LT_POINT);
	light1->setPosition(1500, 50,1550); 
	light1->setDiffuseColour(0.0f, 0, 1);
	light1->setSpotlightOuterAngle(Ogre::Degree(10.0f));

	Ogre::Entity* Light1Ent = mSceneMgr->createEntity("Light1Ent","sphere.mesh");
	light1Node = planeNode->createChildSceneNode("light1Node"); 
	light1Node->setScale(0.2f, 0.2f, 0.2f); 
	light1Node->setPosition(1500, 50, 1550); 
	light1Node->attachObject(Light1Ent);
	// 1500,0,1550ÀÇ light



	Ogre::Entity* rotationLight1Ent = mSceneMgr->createEntity("rotationLight1Ent","sphere.mesh");
	rotationLight1Node = planeNode->createChildSceneNode("rotationLight1Node"); 
	rotationLight1Node->setScale(0.2f, 0.2f, 0.2f); 
	rotationLight1Node->setPosition(1500, 50, 1550); 
	rotationLight1Ent->setCastShadows(false);
	rotationLight1Node->attachObject(rotationLight1Ent);
	
	light2 = mSceneMgr->createLight("Light2"); 
	light2->setType(Ogre::Light::LT_POINT);
	light2->setPosition(1700, 50,-1400); 
	light2->setDiffuseColour(0.0f, 1, 0);
	light2->setSpotlightOuterAngle(Ogre::Degree(10.0f));


	Ogre::Entity* rotationLight2Ent = mSceneMgr->createEntity("rotationLight2Ent","sphere.mesh");
	rotationLight2Node = planeNode->createChildSceneNode("rotationLight2Node"); 
	rotationLight2Node->setScale(0.2f, 0.2f, 0.2f); 
	rotationLight2Node->setPosition(1700, 50,-1400); 
	rotationLight2Ent->setCastShadows(false);
	rotationLight2Node->attachObject(rotationLight2Ent);

	Ogre::Entity* Light2Ent = mSceneMgr->createEntity("Light2Ent","sphere.mesh");
	light2Node = planeNode->createChildSceneNode("light2Node"); 
	light2Node->setScale(0.2f, 0.2f, 0.2f); 
	light2Node->setPosition(1700,50,-1400); 
	light2Node->attachObject(Light2Ent);
	// 1700,0,-1400ÀÇ light

	light3 = mSceneMgr->createLight("Light3"); 
	light3->setType(Ogre::Light::LT_POINT);
	light3->setPosition(-1180,50,-1750); 
	light3->setDiffuseColour(1.0f, 0, 0);
	light3->setSpotlightOuterAngle(Ogre::Degree(10.0f));



	Ogre::Entity* rotationLight3Ent = mSceneMgr->createEntity("rotationLight3Ent","sphere.mesh");
	rotationLight3Node = planeNode->createChildSceneNode("rotationLight3Node"); 
	rotationLight3Node->setScale(0.2f, 0.2f, 0.2f); 
	rotationLight3Node->setPosition(-1180,50,-1750); 
	rotationLight3Ent->setCastShadows(false);
	rotationLight3Node->attachObject(rotationLight3Ent);

	Ogre::Entity* Light3Ent = mSceneMgr->createEntity("Light3Ent","sphere.mesh");
	light3Node = planeNode->createChildSceneNode("light3Node"); 
	light3Node->setScale(0.2f, 0.2f, 0.2f); 
	light3Node->setPosition(-1180,50,-1750); 
	light3Node->attachObject(Light3Ent);
	//-1180,0,-1750ÀÇ light

	light4 = mSceneMgr->createLight("Light4"); 
	light4->setType(Ogre::Light::LT_POINT);
	light4->setPosition(-1500,50,1180); 
	light4->setDiffuseColour(1.0f, 1, 1);
	light4->setSpotlightOuterAngle(Ogre::Degree(10.0f));

	Ogre::Entity* rotationLight4Ent = mSceneMgr->createEntity("rotationLight4Ent","sphere.mesh");
	rotationLight4Node = planeNode->createChildSceneNode("rotationLight4Node"); 
	rotationLight4Node->setScale(0.2f, 0.2f, 0.2f); 
	rotationLight4Node->setPosition(-1500,50,1180); 
	rotationLight4Ent->setCastShadows(false);
	rotationLight4Node->attachObject(rotationLight4Ent);

	Ogre::Entity* Light4Ent = mSceneMgr->createEntity("Light4Ent","sphere.mesh");
	light4Node = planeNode->createChildSceneNode("light4Node"); 
	light4Node->setScale(0.2f, 0.2f, 0.2f); 
	light4Node->setPosition(-1500,50,1180); 
	light4Node->attachObject(Light4Ent);

	light5 = mSceneMgr->createLight("Light5"); 
	light5->setType(Ogre::Light::LT_POINT);
	light5->setPosition(0,100,0); 
	light5->setDiffuseColour(1.0f, 1.0f, 1.0f);
	light5->setSpotlightOuterAngle(Ogre::Degree(90.0f));

}

void TutorialApplication::destroyScene(void)
{

}

void TutorialApplication::lightrotation(const Ogre::FrameEvent& fe)
{
	static float angle = 0;
	float r = angle * 3.14 /180;
	rotationLight1Node->setPosition(light1Node->getPosition().x+50.0f + (cos(r) * 100.0f), 50.0f, light1Node->getPosition().z+sin(r)*100.0f);
	rotationLight2Node->setPosition(light2Node->getPosition().x+50.0f + (cos(r) * 100.0f), 50.0f, light2Node->getPosition().z+sin(r)*100.0f);
	rotationLight3Node->setPosition(light3Node->getPosition().x+50.0f + (cos(r) * 100.0f), 50.0f, light3Node->getPosition().z+sin(r)*100.0f);
	rotationLight4Node->setPosition(light4Node->getPosition().x+50.0f + (cos(r) * 100.0f), 50.0f, light4Node->getPosition().z+sin(r)*100.0f);

	angle=angle + 60*fe.timeSinceLastFrame;
}
void TutorialApplication::robotMove(int index,const Ogre::FrameEvent& fe)
{
	static float time[10];
	std::string name;
		if(robotNode[index] !=0 && robotEntity[index]->isAttached())
		{
			if(robotstate[index] != shoot && robotstate[index] != die )
			{
				if(tudorNode->getPosition().x >= robotNode[index]->getPosition().x+501)
				{
					mRobotDirection[index].x = mRobotMove;
					robotstate[index] = walk;
				}
				else if(tudorNode->getPosition().x <= robotNode[index]->getPosition().x-501)
				{
					mRobotDirection[index].x = -mRobotMove;
					robotstate[index] = walk;
				}

				if(tudorNode->getPosition().z >= robotNode[index]->getPosition().z+501)
				{
					mRobotDirection[index].z = mRobotMove;
					robotstate[index] = walk;
				}
				else if(tudorNode->getPosition().z <= robotNode[index]->getPosition().z-501)
				{
					mRobotDirection[index].z = -mRobotMove;
					robotstate[index] = walk;
				}
			}

			if(abs(tudorNode->getPosition().x - robotNode[index]->getPosition().x) <=500)
			{
				mRobotDirection[index].x = 0;
			}
			if(abs(tudorNode->getPosition().z - robotNode[index]->getPosition().z) <=500)
			{
				mRobotDirection[index].z = 0;
			}

			if(robotstate[index] == die)
			{
				mRobotDirection[index].x = 0;
				mRobotDirection[index].z = 0;
			}

			if(abs(tudorNode->getPosition().x - robotNode[index]->getPosition().x) <=500 && 
				abs(tudorNode->getPosition().z - robotNode[index]->getPosition().z) <=500)
			{
				time[index]+=fe.timeSinceLastFrame;
				if(RorotAni[index]->getLength()+0.5f<time[index] || robotstate[index] ==walk)
				{
					std::string name = "PowerRobot" + Ogre::StringConverter::toString(index);
					if(robotstate[index] == shoot)
					{
						if(robotNode[index]->getName() ==name)
							HP-=2;
						else
							HP--;
						std::string a = "HP :" + Ogre::StringConverter::toString( HP);
						mInfoLabel->setCaption(a);
						robotstate[index] = idle;
					}
					else if(robotstate[index] == idle ||robotstate[index] == walk )
					{
						robotstate[index] = shoot;
						time[index] = 0;
					}
				}
			}
		if(robotstate[index] == die)
		{
			time[index]+=fe.timeSinceLastFrame;
			if(RorotAni[index]->getLength()+0.3f<time[index])
			{
				robotNode[index]->detachObject(robotEntity[index]);
				mSceneMgr->destroyEntity(robotEntity[index]);
				mSceneMgr->destroySceneNode(robotNode[index]);
				int rnd = rand() %100+1;
				if(rnd>=90)
				{
					name = "SpeedRobot" + Ogre::StringConverter::toString(index);
					robotEntity[index] = mSceneMgr->createEntity(name, "robot.mesh");
					robotNode[index] = mSceneMgr->getRootSceneNode() ->createChildSceneNode(name);
					robotNode[index]->setScale(1,1,1);
				}
				else if(rnd>=70)
				{
					name = "PowerRobot" + Ogre::StringConverter::toString(index);
					robotEntity[index] = mSceneMgr->createEntity(name, "robot.mesh");
					robotNode[index] = mSceneMgr->getRootSceneNode() ->createChildSceneNode(name); 
					robotNode[index]->setScale(3.0f,3.0f,3.0f);
				}
				else
				{
					name = "Robot" + Ogre::StringConverter::toString(index);
					robotEntity[index] = mSceneMgr->createEntity(name, "robot.mesh");
					robotNode[index] = mSceneMgr->getRootSceneNode() ->createChildSceneNode(name);
					robotNode[index]->setScale(5,5,5);
				}
				rnd = rand() %4;
				switch (rnd)
				{
					case 0:
						robotNode[index]->setPosition(1500,0,1550);
						robotNode[index]->attachObject(robotEntity[index]);
						break;
					case 1:
						robotNode[index]->setPosition(1700,0,-1400);
						robotNode[index]->attachObject(robotEntity[index]);
						break;
					case 2:
						robotNode[index]->setPosition(-1180,0,-1750);
						robotNode[index]->attachObject(robotEntity[index]);
						break;
					case 3:
						robotNode[index]->setPosition(-1500,0,1180);
						robotNode[index]->attachObject(robotEntity[index]);
						break;
				}
				robotNode[index]->lookAt(planeNode->getPosition(),Node::TS_WORLD,Vector3::UNIT_X);
				time[index]=0;
				robotstate[index] = idle;
			}
		}
		}
}

void TutorialApplication::robotAniControl(const Ogre::FrameEvent& fe)
{
	for(int i=0;i<100;i++)
	{
		if(robotEntity[i]->isAttached())
		{
			if(robotstate[i] == idle)
			{
				RorotAni[i] = robotEntity[i]->getAnimationState("Idle");
				RorotAni[i]->setLoop(true);
				RorotAni[i]->setEnabled(true);
			}
			if(robotstate[i] == walk)
			{
				RorotAni[i] = robotEntity[i]->getAnimationState("Walk");
				RorotAni[i]->setLoop(true);
				RorotAni[i]->setEnabled(true);
			}
			if(robotstate[i] == shoot)
			{
				RorotAni[i] = robotEntity[i]->getAnimationState("Shoot");
				RorotAni[i]->setLoop(true);
				RorotAni[i]->setEnabled(true);
			}
			if(robotstate[i] == die)
			{
				RorotAni[i] = robotEntity[i]->getAnimationState("Die");
				RorotAni[i]->setLoop(false);
				RorotAni[i]->setEnabled(true);
			}
			RorotAni[i]->addTime(fe.timeSinceLastFrame);
		}
	}
}

void TutorialApplication::windowResized(Ogre::RenderWindow* rw) 
{ 
	int left, top; 
	unsigned int width, height, depth;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState& ms = mMouse->getMouseState(); 
	ms.width = width; 
	ms.height = height;
}

void TutorialApplication::windowClosed(Ogre::RenderWindow* rw) 
{ 
	if(rw == mWindow) 
	{ 
		 if(mInputMgr) 
		 { 
			mInputMgr->destroyInputObject(mMouse); 
			mInputMgr->destroyInputObject(mKeyboard);
			
			OIS::InputManager::destroyInputSystem(mInputMgr); 
			mInputMgr = 0;
		 }
	}
}

void TutorialApplication::setupViewport(Ogre::SceneManager *curr) 
{ 
	mWindow->removeAllViewports();
	
	mCamera = curr->getCamera(CAMERA_NAME); // The Camera 
	Ogre::Viewport *vp = mWindow->addViewport(mCamera);   // Our Viewport linked to the camera 
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
	mCamera->yaw(Degree(180));
	mCamera->pitch(Degree(-20));

	mCamera->lookAt(0, 0, 0);
	mCamera->setNearClipDistance(5);
	mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}





#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
#define WIN32_LEAN_AND_MEAN 
#include "windows.h" 
#endif

#ifdef __cplusplus 
extern "C" 
{ 
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
	INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT ) 
#else 
	int main(int argc, char *argv[]) 
#endif 
{ 
	TutorialApplication app;
  
	try 
	{
	 app.go();
	}

	catch(Ogre::Exception& e) 
	{ 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
	MessageBox( 
		NULL, 
		e.getFullDescription().c_str(), 
		"An exception has occured!", 
		MB_OK | MB_ICONERROR | MB_TASKMODAL); 
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