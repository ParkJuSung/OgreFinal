#pragma once

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>
#include <OISEvents.h> 
#include <OISInputManager.h> 
#include <OISKeyboard.h> 
#include <OISMouse.h>
#include <SdkCameraMan.h>
#include <SdkTrays.h>
#include <OgreAnimationState.h>
#include <OgreSceneQuery.h>

enum RobotState
{
	idle,
	walk,
	shoot,
	die
};

class TutorialApplication : public Ogre::WindowEventListener, public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener,OgreBites::SdkTrayListener
{	
	
private: 
	Ogre::Root* mRoot; 
	Ogre::String mResourcesCfg; 
	Ogre::String mPluginsCfg;
	Ogre::RenderWindow* mWindow;
	Ogre::SceneManager* mSceneMgr;
	Ogre::SceneManager* mMainSceneMgr;
	Ogre::SceneManager* mEndSceneMgr;
	int mSceneNumber;

	Ogre::SceneNode* mCamNode;
	Ogre::SceneNode *robotNode[100];
	Ogre::SceneNode* tudorNode;
	Ogre::SceneNode* snowParticleNode;
	Ogre::SceneNode* fireParticleNode;
	Ogre::SceneNode* planeNode;
	Ogre::SceneNode* gunNode;
	Ogre::SceneNode* itemMachineGunNode;
	Ogre::SceneNode* itemPistolGunNode;
	Ogre::SceneNode* itemLazerGunNode;
	Ogre::SceneNode* ninjaNode;
	Ogre::SceneNode* treeNode[10];
	Ogre::SceneNode* leavesNode[10];

	Ogre::Camera* mCamera;


	OgreBites::SdkCameraMan*    mCameraMan;

	Ogre::Entity* robotEntity[100];
	Ogre::Entity* ent;
	Ogre::Entity* machineGunNodeEntity;
	Ogre::Entity* lazerGunNodeEntity;
	Ogre::Entity* pistolGunNodeEntity;
	Ogre::Entity* itemMachineGunNodeEntity;
	Ogre::Entity* ninjaEntity;
	Ogre::Entity* treeEntiy[10];
	Ogre::Entity* leavesEntiy[10];

	OIS::InputManager* mInputMgr; 
	OIS::Mouse* mMouse; 
	OIS::Keyboard* mKeyboard;

	Ogre::Vector3 mPlayerDirection;
	Ogre::Vector3 mRobotDirection[100];

	

	Ogre::Real mPlayerMove; 
	Ogre::Real mRobotMove;

	Ogre::ParticleSystem* snowParticle;
	Ogre::ParticleSystem* fireParticle;

	Ogre::Light* light1;
	Ogre::Light* light2;
	Ogre::Light* light3;
	Ogre::Light* light4;
	Ogre::Light* light5;


	Ogre::SceneNode* light1Node;
	Ogre::SceneNode* light2Node;
	Ogre::SceneNode* light3Node;
	Ogre::SceneNode* light4Node;

	Ogre::SceneNode* rotationLight1Node;
	Ogre::SceneNode* rotationLight2Node;
	Ogre::SceneNode* rotationLight3Node;
	Ogre::SceneNode* rotationLight4Node;

	Ogre::AnimationState* RorotAni[100];

	Ogre::OverlaySystem*        mOverlaySystem;

	virtual void setupViewport(Ogre::SceneManager *curr); 

	Ogre::Real mMouseX;
    Ogre::Real mMouseY;

	OgreBites::Label* mInfoLabel;
	OgreBites::Label* mScoreLabel;

	OgreBites::SdkTrayManager*	mTrayMgr;
	OgreBites::InputContext   mInputContext;
	int HP;
	int score;
protected:
	virtual bool configure(void);
	virtual void chooseSceneManager(void);
	virtual void createCamera(void);
	virtual void createFrameListener(void);
	virtual void createViewports(void);
	virtual void setupResources(void);
	virtual void createResourceListener(void);
	virtual void loadResources(void);
	virtual bool setup();
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);
	virtual bool keyPressed(const OIS::KeyEvent& ke);
	virtual bool keyReleased(const OIS::KeyEvent& ke);
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id); 
	virtual void createScene(void);
	virtual void destroyScene(void);
	virtual void windowResized(Ogre::RenderWindow* rw); 
	virtual void windowClosed(Ogre::RenderWindow* rw);
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
public:
	TutorialApplication(void);
	virtual ~TutorialApplication(void);
	virtual void go();
	void createLight(Ogre::SceneNode* planeNode);
	void setupCreateEnemy();
	void createEnemy();
	void robotMove(int index,const Ogre::FrameEvent& fe);
	void robotAniControl(const Ogre::FrameEvent& fe);
	void lightrotation(const Ogre::FrameEvent& fe);
	
};

