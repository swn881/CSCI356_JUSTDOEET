#include "stdafx.h"
#include "SoundPlayer.h"

using namespace irrklang;

SoundPlayer::SoundPlayer(void)
{
	engine = createIrrKlangDevice();
	/*
	fireSound = engine->addSoundSourceFromFile("media/sounds/fire.wav", ESM_AUTO_DETECT, true);
	explosionSound = engine->addSoundSourceFromFile("media/sounds/explosion.wav", ESM_AUTO_DETECT, true);
	movingTankSound = engine->addSoundSourceFromFile("media/sounds/moving_truck.wav", ESM_AUTO_DETECT, true);
	*/
}

SoundPlayer::~SoundPlayer(void)
{
	if (engine)
	{
		engine->stopAllSounds();

		for (std::set<ISound*>::iterator it = iSoundSet.begin(); it != iSoundSet.end(); it++)
		{
			(*it)->drop();
		}

		engine->removeAllSoundSources();
		engine->drop();
	}
}

void SoundPlayer::playFireSound(){	
	engine->play2D("media/sounds/fire.wav");
}

void SoundPlayer::playExplosionSound(){
	engine->play2D("media/sounds/explosion.wav");

}

void SoundPlayer::playMovingTank(){
	engine->play2D("media/sounds/moving_truck.wav", true);

}

void SoundPlayer::stopSounds()
{
	engine->stopAllSounds();
}

ISound* SoundPlayer::playMovingTankSound(const Ogre::Vector3& position){
	ISound* sound = engine->play3D(movingTankSound, convert(position), true, false, true, false);
	iSoundSet.insert(sound);
	return sound;
}
