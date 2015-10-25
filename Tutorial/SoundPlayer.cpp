#include "stdafx.h"
#include "SoundPlayer.h"

using namespace irrklang;

SoundPlayer::SoundPlayer(void)
{
	engine = createIrrKlangDevice();
	fireSound = engine->addSoundSourceFromFile("media/sounds/fire.flac", ESM_AUTO_DETECT, true);
	explosionSound = engine->addSoundSourceFromFile("media/sounds/explosion.wav", ESM_AUTO_DETECT, true);
	movingTankSound = engine->addSoundSourceFromFile("media/sounds/moving_truck.mp3", ESM_AUTO_DETECT, true);
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

void SoundPlayer::playFireSound(const Ogre::Vector3& position){
	engine->play3D(fireSound, convert(position));

}

void SoundPlayer::playExplosionSound(const Ogre::Vector3& position){
	engine->play3D(explosionSound, convert(position));
	
}

ISound* SoundPlayer::playMovingTankSound(const Ogre::Vector3& position){
	ISound* sound = engine->play3D(movingTankSound, convert(position), false, true, true);
	iSoundSet.insert(sound);
	return sound;
}
