#ifndef __SoundPlayer_h_
#define __SoundPlayer_h_

#pragma once

#include "stdafx.h"
#include "irrKlang.h"

#pragma comment(lib, "irrKlang.lib")

inline irrklang::vec3df convert(const Ogre::Vector3& vec3)
{
	return irrklang::vec3df(vec3.x, vec3.y, vec3.z);
}

class SoundPlayer
{
public:
	SoundPlayer(void);
	~SoundPlayer(void);

	void playFireSound();
	void playExplosionSound();
	void playMovingTank();
	void stopSounds();
	irrklang::ISound* playMovingTankSound(const Ogre::Vector3& position);
private:
	irrklang::ISoundEngine* engine;
	irrklang::ISoundSource* fireSound;
	irrklang::ISoundSource* explosionSound;
	irrklang::ISoundSource* movingTankSound;

	std::set<irrklang::ISound*> iSoundSet;
};

#endif