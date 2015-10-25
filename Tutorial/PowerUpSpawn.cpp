#include "stdafx.h"
#include "PowerUpSpawn.h"
#include "GroupAssignment.h"

PowerUpSpawn::PowerUpSpawn(Ogre::SceneNode * spawn, std::string x)
{
	spawnName = x;
	spawnNode = spawn;
	timer = 10;
	entityCounter = 0;
	powerUp = NULL;
	powerNode = spawnNode->createChildSceneNode(Ogre::Vector3(0,2,0)); //annoyingly the position is scaled based on parent's setScale, regardless of inheritscale
	spawnNode->setScale(50, 25, 50);
	powerNode->setInheritScale(false);
	type = -1;
	isPowerUp = false;
}

PowerUpSpawn::~PowerUpSpawn()
{
	
}

bool PowerUpSpawn::getIsPowerUp()
{
	return isPowerUp;
}

void PowerUpSpawn::createPowerUp(Ogre::SceneManager* ScnMgr, char type) //call initially and whenever we want to create a powerup
{
	isPowerUp = true;

	if (type == '?') //random powerup
	{
		int x = rand() % 4;
		if (x == 0)
			type = 'H';
		else if (x == 1)
			type = 'R';		
		else if (x == 2)
			type = 'S';
		else if (x == 3)
			type = 'P';
	}

	std::ostringstream oss;
	oss << ++entityCounter;
	std::string name = spawnName + oss.str();


	switch (type)
	{
		case ('H'): //health pack
		{
			powerUp = ScnMgr->createEntity(name, "healthup.mesh");
			powerNode->attachObject(powerUp);
			powerNode->setScale(500, 100, 80);
			break;
		}
		case ('R'): //projectile firerate
		{
			powerUp = ScnMgr->createEntity(name, "lightning_bolt.mesh");
			powerNode->attachObject(powerUp);
			powerNode->setScale(50, 50, 50);
			break;
		}
		case ('S'): //movement speed
		{
			powerUp = ScnMgr->createEntity(name, "tyre.mesh");
			powerNode->attachObject(powerUp);
			powerNode->setScale(75, 75, 50);
			break;
			break;
		}
		case ('P'): //projectile damage
		{
			powerUp = ScnMgr->createEntity(name, "bullet.mesh");
			powerNode->attachObject(powerUp);
			powerNode->setScale(35, 50, 50);
			break;
			break;
		}
		default:
			break;
	}
}

char PowerUpSpawn::pickupPowerUp(Ogre::SceneManager* ScnMgr)
{
	//destroy powerup entity
	powerUp->detachFromParent();
	ScnMgr->destroyEntity(powerUp->getName());
	powerUp = NULL;

	timer = 10;

	isPowerUp = false;
	//apply buff to tank
	return type;
}
void PowerUpSpawn::update(Ogre::SceneManager* ScnMgr, float deltaTime)
{
	timer -= deltaTime;
	/*
	if (timer <= 0 && powerUp == NULL)
	{
		createPowerUp(ScnMgr, '?');
	}

	///
	if (timer <= -10)
	{
		pickupPowerUp(ScnMgr);
	}
	///
	*/
	rotate(deltaTime);
}

void PowerUpSpawn::rotate(float deltaTime)
{
	powerNode->yaw(Ogre::Degree(-0.13 * deltaTime * 300), Ogre::Node::TS_WORLD);
}