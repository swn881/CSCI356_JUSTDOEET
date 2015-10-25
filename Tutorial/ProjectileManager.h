#ifndef __ProjectileManager_h_
#define __ProjectileManager_h_

#include "stdafx.h"
#include "TankManager.h"
#include "SoundPlayer.h"

#pragma once

class TankManager;

struct Explosion{
	Explosion(Ogre::SceneNode*, const float&);
	~Explosion(void);

	Ogre::SceneNode* snExplosion;
	float existedTime;
	float timeLimit;
};

struct Projectile{ //doesn't need a rigid body
	Projectile(const Ogre::Vector3& origin, Ogre::SceneNode* particle, const Ogre::Degree& angle, const float& velocity, const float& dmg);
	~Projectile(void);
	void update(const float& delta, const float& gravity);

	Ogre::Vector3 origin;
	Ogre::SceneNode* particleSn;
	Ogre::Degree angle;
	float velocity;
	float timeExisted;
	float dmg;
};

class ProjectileManager
{
public:
	ProjectileManager(void);
	~ProjectileManager(void);
	void init(Ogre::SceneManager* scnMgr, SoundPlayer* soundPlayer);
	void createProjectile(const Ogre::Vector3& tankPosition, const Ogre::Quaternion& turretOrientation,
						  const Ogre::Degree& angle, const float& velocity, const float& dmg);
	void createExplosion(const Ogre::Vector3& location);
	void update(const float& delta, TankManager* tankMgr);

private:
	std::set<Projectile*> projectiles;
	std::set<Explosion*> explosions;
	Ogre::SceneManager* mSceneMgr;
	float gravityValue;

	Ogre::Sphere explosionSphere;

	SoundPlayer* soundPlayer;

	void destroyProjectile(Projectile* projectile);
	void destroyExplosion(Explosion* explosion);
	void scaleBy(Ogre::Real factor, Ogre::ParticleSystem* pSys_);

	int counter;
};

#endif