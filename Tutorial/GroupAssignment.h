#ifndef __GROUP_ASSIGNMENT_h_
#define __GROUP_ASSIGNMENT_h_
 
#include "BaseApplication.h"
#include "TankManager.h"
#include "PowerUpSpawn.h"
#include "SoundPlayer.h"

class GroupAssignment : public BaseApplication
{
public:
    GroupAssignment(void);
    virtual ~GroupAssignment(void);
 
protected:
	virtual bool setup(void);
    virtual void createScene(void);
 
	// Ogre::FrameListener
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
    // OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg );
    // OIS::MouseListener
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
 
	Ogre::Real mRotate;          // The rotate constant
	Ogre::Real mMove;            // The movement constant
 
	Ogre::Vector3 mDirection;     // Value to move in the correct direction

	void createTanks(int);

private:
	int mCurrentState;
	int startNode;
	int goalNode;

	TankManager* tankManager;
	ProjectileManager* projectileManager;
	SoundPlayer* soundPlayer;

	Graph* pathFindingGraph;
	PathFinding mPathFinder;

	void createPath(Ogre::ManualObject* line, float height, std::vector<int>& path, Ogre::ColourValue& colour);

	bool buttonHeldCamera;
	bool ctrlHeld;

	std::vector<Ogre::SceneNode*> boundingBoxes;

	OgreBites::ParamsPanel* mDetailsPanel2;

	Tank* selectedTank; //used for astar, we check if it is null then no tanks is selected
	bool inTankMode;

	//DANNI CODE
	Ogre::OverlaySystem *mOverlaySystem;
	std::vector<PowerUpSpawn*> mPowerUpSpawns;
	Ogre::Overlay* mBulletAnimation;
	int mPowerupCount;
	float mWeaponTimer;
	virtual void createWeather(void); //place in setup
	virtual void controlWeather();			  //place in a case statement for keypress, will toggle weather
	void createReloadOverlay(Ogre::OverlayManager &overlayManager); //place in or around createscene, wherever overlay is created
	void updateReloadOverlay(double timer); //place in framerenderingqueued
	
	void createPowerUpSpawn(Ogre::Vector3 position); //place in createscene
	void updatePowerUps(float deltaTime); //place in framerenderingqueued
	void deletePowerUpSpawns();
	float powerUpTimer;
	std::set<int> spawnLocations;
	
	//NEW DANNI CODE
	Ogre::Overlay* mTextGUI;

	void createTextElement(Ogre::OverlayManager &overlayManager, Ogre::String title, Ogre::String caption, Ogre::Real x, Ogre::Real y);
	void createTextOverlay(Ogre::OverlayManager &overlayManager, Ogre::Real x, Ogre::Real y, Ogre::Real width, Ogre::Real height);
	void updateTextOverlay();

	int testhealth;
	float mPrevHealth;
	Ogre::Overlay* mHealthBar;
	void createHealthOverlay(Ogre::OverlayManager &overlayManager, Ogre::Real x, Ogre::Real y, Ogre::Real width, Ogre::Real height);
	void updateHealthOverlay(float health);
	void createHealthBlock(Ogre::OverlayManager &overlayManager, Ogre::String title, Ogre::Real x, Ogre::Real y, Ogre::Real width, Ogre::Real height);

	
	
	
};
 
#endif // #ifndef __GROUP_ASSIGNMENT_h_