#include "stdafx.h"
#include "TankManager.h"

Tank::Tank(Ogre::BillboardSet* healthBar, Ogre::BillboardSet* selectionCircle,
		   Ogre::SceneNode* tankBodyNode, Ogre::SceneNode* tankTurretNode, Ogre::SceneNode* tankBarrelNode, TankManager* tnkMgr,
		   Graph* pathFindingGraph, PathFinding mPathFinder, Ogre::ManualObject* aStarPath, int side, int goal, Ogre::SceneManager* mSceneMgr){
	//wee added 3 lines
	this->pathFindingGraph = pathFindingGraph;
	this->mPathFinder = mPathFinder;
	this->aStarPath = aStarPath;
	this->tankSide = side;
	this->goalNode = goal;
	this->mSceneMgr = mSceneMgr;

	this->tnkMgr = tnkMgr;

	// Create a BillboardSet to represent a health bar and set its properties
	mHealthBar = healthBar;
	mHealthBar->setCastShadows(false);
	mHealthBar->setDefaultDimensions(50, 3);
	mHealthBar->setMaterialName("myMaterial/HealthBar");

	// Create a billboard for the health bar BillboardSet
	mHealthBarBB = mHealthBar->createBillboard(Ogre::Vector3(0, 40, 0));

	// Set it to always draw on top of other objects
	mHealthBar->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);

	// Create a BillboardSet for a selection circle and set its properties
	mSelectionCircle = selectionCircle;
	mSelectionCircle->setCastShadows(false);
	mSelectionCircle->setDefaultDimensions(120, 120);
	mSelectionCircle->setMaterialName("myMaterial/SelectionCircle");
	mSelectionCircle->setBillboardType(Ogre::BillboardType::BBT_PERPENDICULAR_COMMON);
	mSelectionCircle->setCommonDirection(Ogre::Vector3(0, 1, 0));
	mSelectionCircle->setCommonUpVector(Ogre::Vector3(0, 0, -1));

	// Create a billboard for the selection circle BillboardSet
	mSelectionCircleBB = mSelectionCircle->createBillboard(Ogre::Vector3(0, -12.5f, 0)); //offset the tank's body
	mSelectionCircleBB->setTexcoordRect(0.0, 0.0, 1.0, 1.0);

	// Set the health bar to the appropriate level
	mHealthBarBB->setTexcoordRect(0.0, 0.0, 0.5, 1.0);

	mTankBodyNode = tankBodyNode;
	mTankTurretNode = tankTurretNode;
	mTankBarrelNode = tankBarrelNode;

	//attach healthbar and selection circle to tank
	mTankBodyNode->attachObject(mHealthBar);
	mTankBodyNode->attachObject(mSelectionCircle);

	thirdPersonCamNode = mTankBodyNode->createChildSceneNode(Ogre::Vector3(300.f, 150.f, 0.f));
	thirdPersonCamNode->lookAt(mTankBodyNode->getPosition(), Ogre::Node::TransformSpace::TS_LOCAL);

	
	tankSphere.setRadius(62.5f);

	ssq = mSceneMgr->createSphereQuery(tankSphere, Ogre::SceneManager::ENTITY_TYPE_MASK);

	tankSound = tnkMgr->soundPlayer->playMovingTankSound(tankBodyNode->getPosition());
}

void Tank::resetAll(void){
	hp = 1.0f;
	powerUpDuration = 0.f;
	deathTimer = 5.f;
	mDestination = Ogre::Vector3::ZERO;
	mDirection = Ogre::Vector3::ZERO;

	mMove = 0.f;
	bodyRotate = 0.f;
	turretRotation = 0.f;
	barrelRotation = 0.f;
	turretDegree = 0.f;
	barrelDegree = 0.f;

	//wee initialize here
	pathCreated = false;
	mMoveSpd = 70.0; 
	tankStarted = false;
	mDistance = 0;
	currentState = A_STAR;
	sphereSceneTime = 0;

	mHealthBarBB->setTexcoordRect(0.0, 0.0, 0.5, 1.0);

	//wander initialise
	wanderAngle = 0.f;
	ANGLE_CHANGE = 10.f;

	setSelected(false);
	resetAttributes();
}

void Tank::resetAttributes(void){
	ms = 70.f;
	fireRate = 2.f;
	dmg = 15;
	shootingVelocity = 200.f;
}

Ogre::Vector3 Tank::getPosition(void){
	return mTankBodyNode->getPosition();
}

bool Tank::checkSceneNode(const Ogre::SceneNode* sceneNode){
	return (sceneNode == mTankBodyNode || sceneNode == mTankTurretNode || sceneNode == mTankBarrelNode);
}

void Tank::setSelected(bool isSelected){
	selected = isSelected;
	mHealthBar->setVisible(selected);
	mSelectionCircle->setVisible(selected);
}

bool Tank::isSelected(void){
	return selected;
}

void Tank::setPath(std::deque<int>& path){
	this->path = path;
	//this->path.pop_front(); //get rid of this later
}

void Tank::shoot(void){
	tnkMgr->createProjectile(mTankBodyNode->getPosition(), mTankTurretNode->_getDerivedOrientation(), Ogre::Degree(barrelDegree), shootingVelocity, dmg);
}

void Tank::takeDamage(const int& dmg){
	if (isAlive())
	{
		hp -= (dmg / 100.0f);

		//check if still dead
		if (!isAlive())
		{
			mTankBodyNode->setVisible(false);
		}

		float healthBarAdjuster = (1.0 - hp) / 2;
		mHealthBarBB->setTexcoordRect(0.0 + healthBarAdjuster, 0.0, 0.5 + healthBarAdjuster, 1.0);
	}
}

void Tank::attachCamera(Ogre::Camera* camera){
	thirdPersonCamNode->attachObject(camera);
	camera->setPosition(Ogre::Vector3::ZERO);
	camera->lookAt(mTankBodyNode->getPosition());
}

void Tank::detachCamera(Ogre::Camera* camera){
	thirdPersonCamNode->detachObject(camera);
}

void Tank::setPossessed(bool possessed){
	if(possessed){
		savedState = currentState;
		currentState = POSSESSED;
	} else{
		currentState = savedState;
	}
}

bool Tank::isAlive(void){
	return hp > 0.f;
}

Ogre::Degree Tank::getShootingAngle(const Ogre::Vector3& targetTank){
	float distance = getPosition().distance(targetTank);

	//(gravity * distance) / (velocity ^ 2)
	float inner = (80.f * distance) / (Ogre::Math::Sqr(shootingVelocity));

	return Ogre::Degree(0.5f * Ogre::Math::ASin(inner));
}

void Tank::update(const float& deltaTime, std::vector<PowerUpSpawn*> mPowerUpSpawns){
	
	//this check must be first
	if (!isAlive())
	{
		deathTimer -= deltaTime;

		if (deathTimer <= 0.f)
		{
			deathTimer = 0.f;
			resetAll();
			
			std::random_device rd;
			std::mt19937 random(rd());

			if (tankSide == 1)
			{
				std::uniform_real_distribution<double> randomGen1(1, 5);
				int tempNumber = randomGen1(random);
				std::uniform_real_distribution<double> randomGen2(0, 36);
				int tempNumber2 = (36 * (int)randomGen2(random)) + (int)tempNumber;

				Ogre::Vector3 position = pathFindingGraph->getPosition(tempNumber2);

				mTankBodyNode->setPosition(position);
			} else if(tankSide == 2){
				std::uniform_real_distribution<double> randomGen1(31, 35);
				int tempNumber = randomGen1(random);
				std::uniform_real_distribution<double> randomGen2(0, 36);
				int tempNumber2 = (36 * (int)randomGen2(random)) + (int)tempNumber;
				
				Ogre::Vector3 position = pathFindingGraph->getPosition(tempNumber2);

				mTankBodyNode->setPosition(position);
			}
		}
		return;
	}

	sphereSceneTime += deltaTime;
	if(sphereSceneTime > 0.2)
	{
		sphereSceneTime = 0;

		Ogre::Vector3 tankCenter = mTankBodyNode->getPosition();
		int location;
		bool found = false;

		for(int i = 0; i < mPowerUpSpawns.size(); i++)
		{
			Ogre::Vector3 powerUpLocation = mPowerUpSpawns[i]->spawnNode->getPosition();
			if(tankCenter.distance(powerUpLocation) < 50)
			{
				std::cout << 111 << std::endl;
				found = true;
				location = i;
			}
		}
		if (found)
		{
			if(mPowerUpSpawns[location]->getIsPowerUp())
			{
				char tempType = mPowerUpSpawns[location]->pickupPowerUp(mSceneMgr);

				switch(tempType)
				{
					case 'H': //health
					{
						hp += 0.1f;
						if (hp > 1.0f)
						{
							hp = 1.0f;
						}
					}
					break;
					case 'R': //fire rate
					{
						fireRate = 1.f;
						powerUpDuration = 4.f;
					}
					break;
					case 'S': //speed
					{
						ms = 90.f;
						powerUpDuration = 4.f;
					}
					break;
					case 'P': //damage
					{
						dmg = 30.f;
						powerUpDuration = 4.f;
					}
					break;
				}
			}
		}
	}






/*
		tankSphere.setCenter(tankCenter);
		ssq->setSphere(tankSphere);
		Ogre::SceneQueryResult& result = ssq->execute();

		

		if (result.movables.size() > 0 )
		{
			Ogre::SceneQueryResultMovableList::iterator sqrItr;
			for (sqrItr = result.movables.begin(); sqrItr != result.movables.end(); sqrItr++){
				//if tank, minus health based on the distance from the explosion center
				
				bool check = false;
				int found;
				Ogre::SceneNode * x = (*sqrItr)->getParentSceneNode();
				for(int i = 0; i < mPowerUpSpawns.size() && check == false; i++)
				{
					Ogre::SceneNode * y = mPowerUpSpawns[i]->spawnNode;
					if(x == y )
					{
						found = i;
						check = true;
					}
				}
				if(check)
				{
					if(mPowerUpSpawns[found]->getIsPowerUp())
					{
						char tempType = mPowerUpSpawns[found]->pickupPowerUp(mSceneMgr);

						switch(tempType)
						{
							case 'H': //health
							{
								hp += 0.1f;
								if (hp > 1.0f)
								{
									hp = 1.0f;
								}
							}
							break;
							case 'R': //fire rate
							{
								fireRate = 1.f;
								powerUpDuration = 4.f;
							}
							break;
							case 'S': //speed
							{
								ms = 90.f;
								powerUpDuration = 4.f;
							}
							break;
							case 'P': //damage
							{
								dmg = 30.f;
								powerUpDuration = 4.f;
							}
							break;
						}
					}
						
				}
					
			}
		} 
		sphereSceneTime = 0;
	}
	*/
	if (powerUpDuration > 0.f)
	{
		powerUpDuration -= deltaTime;

		if (powerUpDuration <= 0.f)
		{
			resetAttributes();
		}
	}

	//no movement for body yet
	

	//A_STAR, SEEK, WANDER, ESCAPE, STOP
	switch(currentState)
	{
		case A_STAR:
			//initiate astar
			if(tankStarted == false)
			{
				tankStarted = true;
				aStar(); //creates the path to get out of there
			}
			else
			{
				aStarMovement(deltaTime);
			}
		break;
		case WANDER:
			wander(deltaTime);
		break;
		case SEEK:
			
		break;
		case ESCAPE:
			
		break;
		case STOP:
		
		break;

		case POSSESSED: {
			mTankBodyNode->translate(mMove, 0, 0, Ogre::Node::TransformSpace::TS_LOCAL);
			mTankBodyNode->yaw(Ogre::Degree(bodyRotate));

			// Rotate the tank turret
			mTankTurretNode->yaw(Ogre::Degree(turretRotation));

			turretDegree += turretRotation;

			//to move barrel change mBarrelRotate
			barrelDegree += barrelRotation;

			if(barrelDegree > 30)
				barrelDegree = 30;
			else if(barrelDegree < 0)
				barrelDegree = 0;
			else
				mTankBarrelNode->roll(Ogre::Degree(-barrelRotation));				
		}

		break;
	}

	Ogre::Vector3 setpos = mTankBodyNode->getPosition();
	setpos.y = 13.f;
	mTankBodyNode->setPosition(setpos);

}
//WEE ADDED CODE HERE
void Tank::aStarMovement(const float & deltaTime)
{
	if(pathCreated)
	{
		if (mDirection == Ogre::Vector3::ZERO) 
		{
			nextLocation();
		}
		else
		{

			Ogre::Real move = mMoveSpd * (deltaTime);
			mDistance -= move;
			Ogre::Vector3 src = mTankBodyNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_X;
	
			//http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Quaternion+and+Rotation+Primer
			//this is used for rotation of the tank
			if ((1.0 + src.dotProduct(mDirection)) < 0.0001) 
			{
				mTankBodyNode->yaw(Ogre::Degree(180));
			}
			else
			{
				mDirection.y = 0;
				Ogre::Quaternion quat = src.getRotationTo(mDirection);
				
				Ogre::Quaternion mOrientSrc = mTankBodyNode->getOrientation();
				Ogre::Quaternion mOrientDest = quat * mOrientSrc;
				Ogre::Quaternion delta = Ogre::Quaternion::nlerp(deltaTime * 2.0, mOrientSrc, mOrientDest, true);
				mTankBodyNode->setOrientation(delta);
			}      
			//for movement
			if (mDistance <= 0)
			{
				mTankBodyNode->setPosition(mDestination);
				mDirection = Ogre::Vector3::ZERO;
			}
			else
			{
				mTankBodyNode->translate(move * mDirection);
			}
		}
	}
}
bool Tank::nextLocation()
{
	if(mWalkList.empty())
	{
		aStarPath->clear();
		currentState = WANDER;
		return false;
	}
		
	mDestination = mWalkList.front();
	mWalkList.pop_front();
	mDirection = mDestination - mTankBodyNode->getPosition();

	mDistance = mDirection.normalise();

	return true;
}

void Tank::createPath(Ogre::ManualObject* line, float height, std::vector<int>& path, Ogre::ColourValue& colour)
{
	pathCreated = true;
	line->clear();

	// Specify the material and rendering type
	line->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);

	// Specify the vertices and vertex colour for the line
	Ogre::Vector3 position;

	for(std::vector<int>::iterator it=path.begin(); it!=path.end(); it++)
	{
		position = pathFindingGraph->getPosition(*it);
		line->position(Ogre::Vector3(position.x, height, position.z));
		
		if(mWalkList.empty())
		{
			Ogre::Vector3 temp = mTankBodyNode->getPosition();
			mWalkList.push_back(Ogre::Vector3(temp.x, 13.0f, temp.z));
		}
		else
		{
			mWalkList.push_back(Ogre::Vector3(position.x, 13.0f, position.z));
		}

		
		//printf("x: %f z: %f\n", position.x, position.z);
		line->colour(colour);
	}
	// Finished defining line
	line->end();
}

void Tank::aStar()
{
	Ogre::Vector3 tankLocation = mTankBodyNode->getPosition();
	startNode = pathFindingGraph->getNode(tankLocation);

	// check that goal node is not the same as start node
	if(goalNode != startNode)
	{
		// try to find path from start to goal node
		std::vector<int> path;
			
		if(mPathFinder.AStar(startNode, goalNode, *pathFindingGraph, path))
		{
			createPath(aStarPath, 13.0f, path, Ogre::ColourValue(1, 0, 0));	
		}
	}
}

//WEE ADDED END

//michael wander code
//code taken and adapted from http://gamedevelopment.tutsplus.com/tutorials/understanding-steering-behaviors-wander--gamedev-1624
//written by Michael Kong
void Tank::wander(const float& deltaTime){
	float circleDistance = 200.f;
	Ogre::Vector3 circleCenter;
	float radius = 20.f;

	std::random_device rd;
	std::mt19937 random(rd());

	Ogre::Quaternion tankOrientation = mTankBodyNode->getOrientation();
	Ogre::Vector3 currentDirection = tankOrientation * Ogre::Vector3::NEGATIVE_UNIT_X; 
	currentDirection.normalise();
	circleCenter = currentDirection * circleDistance;

	Ogre::Vector3 displacement = Ogre::Vector3::UNIT_Z * radius;

	float length = displacement.length();
	displacement.x = cos(wanderAngle) * length;
   	displacement.z = sin(wanderAngle) * length;
	displacement.y = 0;

   	std::uniform_real_distribution<double> randomGen;

   	wanderAngle += randomGen(random) * ANGLE_CHANGE - ANGLE_CHANGE * 0.5f;

   	Ogre::Vector3 steering = circleCenter + displacement;
	steering.normalise();
	steering *= ms;
	
	circleCenter.y = 13.f;

	mTankBodyNode->lookAt(displacement, Ogre::Node::TransformSpace::TS_WORLD);

	mTankBodyNode->translate(steering * deltaTime);
}

//code taken and adapted from http://natureofcode.com/book/chapter-6-autonomous-agents/
//written by Michael Kong
void Tank::seek(const Ogre::Vector3& target, const float& deltaTime){

	Ogre::Vector3 seekTarget = target;
	seekTarget.y = 0.f;

	Ogre::Quaternion tankOrientation = mTankBodyNode->getOrientation();
	Ogre::Vector3 currentDirection = tankOrientation * Ogre::Vector3::NEGATIVE_UNIT_X; 
	currentDirection.normalise();

	Ogre::Vector3 desired = seekTarget - mTankBodyNode->getPosition();
	desired.normalise(); //get unit vector
	desired *= ms;

	Ogre::Vector3 src = tankOrientation * Ogre::Vector3::NEGATIVE_UNIT_X; 
	Ogre::Quaternion quat = src.getRotationTo(seekTarget);
/*
	Ogre::Quaternion mOrientSrc = mTankBodyNode->getOrientation();
	Ogre::Quaternion mOrientDest = quat * mOrientSrc;
*/
	mTankBodyNode->setOrientation(quat * deltaTime);

	mTankBodyNode->translate(desired * deltaTime);
}

void Tank::rotateTank(void){
	Ogre::Vector3 src = mTankBodyNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_X;
				
	if ((1.0 + src.dotProduct(mDirection)) < 0.0001) 
	{
		mTankBodyNode->yaw(Ogre::Degree(180));
	}
	else
	{
		Ogre::Quaternion quat = src.getRotationTo(mDirection);
		mTankBodyNode->rotate(quat);
	}   
}

TankManager::TankManager(void)
{
}

TankManager::~TankManager(void)
{
}

void TankManager::init(Ogre::SceneManager* scnMgr, ProjectileManager* projectileMgr, SoundPlayer* soundPlayer){
	mSceneMgr = scnMgr;
	numSelected = 0;
	
	projMgr = projectileMgr;

	this->soundPlayer = soundPlayer;

	exitNodesA.push_back(121);
	exitNodesA.push_back(337);
	exitNodesA.push_back(517);
	exitNodesA.push_back(769);
	exitNodesA.push_back(1201);

	exitNodesB.push_back(94);
	exitNodesB.push_back(418);
	exitNodesB.push_back(598);
	exitNodesB.push_back(922);
	exitNodesB.push_back(1246);
}

Tank* TankManager::createTank(const Ogre::Vector3& position, int side, Graph* pathFindingGraph, PathFinding mPathFinder){
	int tankNumber = tankSideA.size() + tankSideB.size();
	std::ostringstream oss1;
	oss1 << "tankbody" << tankNumber;

	Ogre::Entity* tankBody = mSceneMgr->createEntity(oss1.str(), "lpbody.mesh");
	tankBody->setCastShadows(true);
	tankBody->setMaterialName("lp_tank_material");

	std::ostringstream oss2;
	oss2 << "tankturret" << tankNumber;

	// Create tank turret entity
	Ogre::Entity* tankTurret = mSceneMgr->createEntity(oss2.str(), "lpturret.mesh");
	tankTurret->setCastShadows(true);
	tankTurret->setMaterialName("lp_tank_material");

	std::ostringstream oss3;
	oss3 << "tankbarrel" << tankNumber;

	// Create tank barrel entity
	Ogre::Entity* tankBarrel = mSceneMgr->createEntity(oss3.str(), "lpbarrel.mesh");
	tankBarrel->setCastShadows(true);
	tankBarrel->setMaterialName("lp_tank_material");

	// Create a child scene node and attach tank body to it
	Ogre::SceneNode* mTankBodyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mTankBodyNode->attachObject(tankBody);
	// Move it above the ground
	mTankBodyNode->translate(position.x, position.y + 13.f, position.z);

	int goalNode = -1;
	float distance = 0.f;

	if(side == 1){
		mTankBodyNode->yaw(Ogre::Degree(180.f));
		
		for (std::vector<int>::iterator it = exitNodesA.begin(); it != exitNodesA.end(); it++)
		{
			Ogre::Vector3 pos = pathFindingGraph->getPosition((*it));

			float curDistance = position.squaredDistance(pos);

			if (distance == 0.f) //distance has not been set
			{
				distance = curDistance;
				goalNode = (*it);
			}
			else if (curDistance < distance)
			{
				distance = curDistance;
				goalNode = (*it);
			}
		}
		
		/*
		Ogre::Real shortestDistance = 999999999999999;
		for(int i = 0; i < exitNodesA.size(); i++)
		{
			Ogre::Vector3 temp = pathFindingGraph->getPosition(exitNodesA[i]);
			Ogre::Real temp2 = temp.distance(mTankBodyNode->getPosition());
			float temp3 = temp2;
			printf("%f", temp3);
			if(shortestDistance > temp2)
			{
				shortestDistance = temp2;
				goalNode = exitNodesA[i];
			}
		}
		*/
	}
	else if(side == 2)
	{
		
		for (std::vector<int>::iterator it = exitNodesB.begin(); it != exitNodesB.end(); it++)
		{
			Ogre::Vector3 pos = pathFindingGraph->getPosition((*it));
			
			float curDistance = position.squaredDistance(pos);

			if (distance == 0.f) //distance has not been set
			{
				distance = curDistance;
				goalNode = (*it);
			}
			else if (curDistance < distance)
			{
				distance = curDistance;
				goalNode = (*it);
			}
		}
		/*
		Ogre::Real shortestDistance = 999999999999999;
		for(int i = 0; i < exitNodesB.size(); i++)
		{
			Ogre::Vector3 temp = pathFindingGraph->getPosition(exitNodesB[i]);
			Ogre::Real temp2 = temp.distance(mTankBodyNode->getPosition());
			float temp3 = temp2;
			printf("%f", temp3);
			if(shortestDistance > temp2)
			{
				shortestDistance = temp2;
				goalNode = exitNodesB[i];
			}
		}*/
	}

	// Create a child scene node from tank body's scene node and attach the tank turret to it
	Ogre::SceneNode* mTankTurretNode = mTankBodyNode->createChildSceneNode();
	mTankTurretNode->attachObject(tankTurret);
	// Move it above tank body
	mTankTurretNode->translate(0.f, 3.f, 0.f);

	// Create a child scene node from tank turret's scene node and attach the tank barrel to it
	Ogre::SceneNode* mTankBarrelNode = mTankTurretNode->createChildSceneNode();
	mTankBarrelNode->attachObject(tankBarrel);
	// Move it to the appropriate position on the turret
	mTankBarrelNode->translate(-30.f, 10.f, -1.5f);

	//WEE ADDED HERE TO MAKE THE MANUAL OBJECT
		std::string pathName = "AStarPath" + std::to_string(tankNumber);
		Ogre::ManualObject* aStarPath = mSceneMgr->createManualObject(pathName);
		aStarPath->clear();
		aStarPath->setQueryFlags(0);
		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(aStarPath);

	Tank* newTank = new Tank(mSceneMgr->createBillboardSet(), mSceneMgr->createBillboardSet(),
		mTankBodyNode, mTankTurretNode, mTankBarrelNode, this, pathFindingGraph, mPathFinder, aStarPath, side, goalNode, mSceneMgr);

	newTank->resetAll();
	
	if (side == 1)
	{
		tankSideA.insert(newTank);
	} else if(side == 2){
		tankSideB.insert(newTank);
	}

	return newTank;
}

void TankManager::createProjectile(const Ogre::Vector3& tankPosition, const Ogre::Quaternion& turretOrientation,
					  const Ogre::Degree& angle, const float& velocity, const float& dmg){
	soundPlayer->playFireSound(tankPosition);
	projMgr->createProjectile(tankPosition, turretOrientation, angle, velocity, dmg);
}

void TankManager::deselectAll(void){
	for (std::set<Tank*>::iterator it = tankSideA.begin(); it != tankSideA.end(); it++)
	{
		if ((*it)->isSelected())
		{
			(*it)->setSelected(false);
		}
	}

	for (std::set<Tank*>::iterator it = tankSideB.begin(); it != tankSideB.end(); it++)
	{
		if ((*it)->isSelected())
		{
			(*it)->setSelected(false);
		}
	}
}

Tank* TankManager::toggleTankSelected(const Ogre::SceneNode* tankSnNode){
	Tank* tank = NULL;

	for (std::set<Tank*>::iterator it = tankSideA.begin(); it != tankSideA.end(); it++)
	{
		if ((*it)->isSelected())
		{
			(*it)->setSelected(false);
			numSelected--;
		}

		if ((*it)->checkSceneNode(tankSnNode))
		{
			(*it)->setSelected(!(*it)->isSelected());
			(*it)->isSelected() ? numSelected++ : numSelected--;

			tank = (*it);
		}
	}

	for (std::set<Tank*>::iterator it = tankSideB.begin(); it != tankSideB.end(); it++)
	{
		if ((*it)->isSelected())
		{
			(*it)->setSelected(false);
			numSelected--;
		}

		if ((*it)->checkSceneNode(tankSnNode))
		{
			(*it)->setSelected(!(*it)->isSelected());
			(*it)->isSelected() ? numSelected++ : numSelected--;

			tank = (*it);
		}
	}

	return tank;

}

void TankManager::checkTankExplosion(const Ogre::SceneNode* tankSnNode, const Ogre::Vector3& explosionCtr, const float& dmg){
	
	for (std::set<Tank*>::iterator it = tankSideA.begin(); it != tankSideA.end(); it++)
	{
		if ((*it)->isAlive() && (*it)->checkSceneNode(tankSnNode))
		{
			float distance = (*it)->getPosition().distance(explosionCtr);

			if (distance <= 10.f)
			{
				(*it)->takeDamage(dmg);
			}
			else if (distance <= 20.f)
			{
				(*it)->takeDamage(dmg - 5);
			}
			else if (distance <= 30.f)
			{
				(*it)->takeDamage(dmg - 10);
			}

		}
	}

	for (std::set<Tank*>::iterator it = tankSideB.begin(); it != tankSideB.end(); it++)
	{
		if ((*it)->isAlive() && (*it)->checkSceneNode(tankSnNode))
		{
			float distance = (*it)->getPosition().distance(explosionCtr);

			if (distance <= 10.f)
			{
				(*it)->takeDamage(15);
			}
			else if (distance <= 20.f)
			{
				(*it)->takeDamage(10);
			}
			else if (distance <= 30.f)
			{
				(*it)->takeDamage(5);
			}

		}
	}
}

Ogre::Vector3 TankManager::getNearestSelectedTankStartPos(const Ogre::Vector3& targetArea){
	for (std::set<Tank*>::iterator it = tankSideA.begin(); it != tankSideA.end(); it++)
	{
		if ((*it)->isSelected())
		{
			return (*it)->getPosition();
		}
	}

	for (std::set<Tank*>::iterator it = tankSideB.begin(); it != tankSideB.end(); it++)
	{
		if ((*it)->isSelected())
		{
			return (*it)->getPosition();
		}
	}

	return Ogre::Vector3(-1.f);
}

void TankManager::update(const float& deltaTime, std::vector<PowerUpSpawn*> mPowerUpSpawns){

	for (std::set<Tank*>::iterator it = tankSideA.begin(); it != tankSideA.end(); it++)
	{
		(*it)->update(deltaTime, mPowerUpSpawns);
	}

	for (std::set<Tank*>::iterator it = tankSideB.begin(); it != tankSideB.end(); it++)
	{
		(*it)->update(deltaTime, mPowerUpSpawns);
	}

}

std::vector<Ogre::Vector3> TankManager::getPositionTank(int side)
{
	std::vector<Ogre::Vector3> tankPos;

	if (side == 1)
	{
		for (std::set<Tank*>::iterator it = tankSideA.begin(); it != tankSideA.end(); it++)
		{
			tankPos.push_back((*it)->getPosition());
		}

	}
	else if (side == 2)
	{
		for (std::set<Tank*>::iterator it = tankSideB.begin(); it != tankSideB.end(); it++)
		{
			tankPos.push_back((*it)->getPosition());
		}
	}
	return tankPos;
}