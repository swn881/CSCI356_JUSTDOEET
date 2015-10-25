#ifndef __TankManager_h_
#define __TankManager_h_
 
#include "stdafx.h"
#include "ProjectileManager.h"
#include <random>
#include "PathFinding.h"
#include "Graph.h"
#include <math.h>
#include "PowerUpSpawn.h"
#include "SoundPlayer.h"

class TankManager;
class ProjectileManager;

struct Tank{
public:
	Tank(Ogre::BillboardSet* healthBar, Ogre::BillboardSet* selectionCircle,
		 Ogre::SceneNode* tankBodyNode, Ogre::SceneNode* tankTurretNode, Ogre::SceneNode* tankBarrelNode, TankManager* tnkMgr,
		 Graph* pathFindingGraph, PathFinding mPathFinder, Ogre::ManualObject* aStarPath, int tankSide, int goal, Ogre::SceneManager* mSceneMgr);
	void resetAll(void);
	void resetAttributes(void);
	Ogre::Vector3 getPosition(void);
	bool checkSceneNode(const Ogre::SceneNode* sceneNode);
	void setSelected(bool isSelected);
	bool isSelected(void);
	void setPath(std::deque<int>& path);
	void shoot(void);
	void takeDamage(const int& dmg);
	void attachCamera(Ogre::Camera* camera);
	void detachCamera(Ogre::Camera* camera);
	void setPossessed(bool possessed);
	Ogre::String getState();

	Ogre::Degree getShootingAngle(const Ogre::Vector3& targetTank);

	void update(const float& deltaTime, std::vector<PowerUpSpawn*> mPowerUpSpawns);

	bool isAlive(void);

	float hp;
	//Mode mode; //the movement mode that the tank is currently in
	float powerUpDurationR, powerUpDurationS, powerUpDurationP;
	float deathTimer;
	
	float ms; //movement speed
	float fireRate;
	int dmg;
	float shootingVelocity;
		
	float mMove;
	float bodyRotate;
	float turretRotation;
	float barrelRotation;

private:
	irrklang::ISound* tankSound;

	float turretDegree;
	float barrelDegree;

	TankManager* tnkMgr;

	Ogre::BillboardSet* mHealthBar;
	Ogre::Billboard* mHealthBarBB;

	Ogre::BillboardSet* mSelectionCircle;
	Ogre::Billboard* mSelectionCircleBB;
	
	Ogre::SceneNode* mTankBodyNode;
	Ogre::SceneNode* mTankTurretNode;
	Ogre::SceneNode* mTankBarrelNode;

	Ogre::SceneNode* thirdPersonCamNode;

	std::deque<int> path;

	//WEE ADDED TANK SPHERE HERE
	Ogre::SceneManager* mSceneMgr;
	Ogre::Sphere tankSphere;
	Ogre::SphereSceneQuery* ssq;
	float sphereSceneTime; 

	//this is used for wander MICHAEL CODE 
	float wanderAngle;
	float ANGLE_CHANGE;

	void decrementPowerups(const float& deltaTime);
	void wander(const float&);
	void seek(const Ogre::Vector3& target, const float& deltaTime);

	//used for movement, wee added stuff here

		Ogre::Vector3 mDestination;
		Ogre::Vector3 mDirection;
		float mDistance;
		//wee added here VARIABLES
		bool pathCreated;
		Ogre::Real mMoveSpd;
		std::deque<Ogre::Vector3> mWalkList;
		int startNode;
		int goalNode;
		Graph* pathFindingGraph;
		PathFinding mPathFinder;
		Ogre::ManualObject* aStarPath;
		bool tankStarted; //used to check if we called astar or not
		int tankSide;

		enum TankBodyStates
		{
			A_STAR, SEEK, WANDER, ESCAPE, STOP, POSSESSED
		};

		TankBodyStates currentState;
		TankBodyStates savedState;

		//wee added here FUNCTIONS 
		void createPath(Ogre::ManualObject* line, float height, std::vector<int>& path, Ogre::ColourValue& colour);
		void aStar();
		bool nextLocation();
		void aStarMovement(const float&);

		//wee added end
	bool selected;
	void rotateTank(void);
};

#pragma once
class TankManager
{
public:
	TankManager(void);
	~TankManager(void);
	void init(Ogre::SceneManager* scnMgr, ProjectileManager* projectileMgr, SoundPlayer* soundPlayer);

	Tank* createTank(const Ogre::Vector3& position, int side, Graph* pathFindingGraph, PathFinding mPathFinder);

	void createProjectile(const Ogre::Vector3& tankPosition, const Ogre::Quaternion& turretOrientation,
						  const Ogre::Degree& angle, const float& velocity, const float& dmg);

	void deselectAll(void);
	Tank* toggleTankSelected(const Ogre::SceneNode* tankSnNode);

	void checkTankExplosion(const Ogre::SceneNode* tankSnNode, const Ogre::Vector3& explosionCtr, const float& dmg);

	Ogre::Vector3 getNearestSelectedTankStartPos(const Ogre::Vector3& targetArea);


	void update(const float& deltaTime, std::vector<PowerUpSpawn*> mPowerUpSpawns);

	std::vector<Ogre::Vector3> getPositionTank(int side);
	
	SoundPlayer* soundPlayer;
private:
	Ogre::SceneManager* mSceneMgr;
	std::set<Tank*> tankSideA;
	std::set<Tank*> tankSideB;
	//vector that represents the exit
	std::vector<int> exitNodesA;
	std::vector<int> exitNodesB;
	std::list<Tank*> selectedTanks;
	int numSelected;
	
	ProjectileManager* projMgr;
};


#endif