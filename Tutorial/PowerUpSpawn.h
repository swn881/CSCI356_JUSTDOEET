#ifndef __POWERUPSPAWN_H
#define __POWERUPSPAWN_H

#include "stdafx.h"

class PowerUpSpawn
{
private:
	Ogre::SceneNode * spawnNode;
	Ogre::SceneNode * powerNode;
	Ogre::Entity * powerUp;
	float timer;
	std::string spawnName;
	int entityCounter;
	char type;
	bool isPowerUp;
	
	void rotate(float deltaTime);

public:
	PowerUpSpawn(Ogre::SceneNode * spawn, std::string name);
	~PowerUpSpawn();
	void update(Ogre::SceneManager* ScnMgr, float deltaTime);
	char pickupPowerUp(Ogre::SceneManager* ScnMgr); //will need to pass the appropriate "tank" to this function to modify it

	void createPowerUp(Ogre::SceneManager* ScnMgr, char type);	//call initially and whenever we want to create a powerup, make private //changed to public
	bool getIsPowerUp();
};


#endif