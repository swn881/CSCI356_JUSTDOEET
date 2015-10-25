#include "stdafx.h"
#include "ProjectileManager.h"

Explosion::Explosion(Ogre::SceneNode* sn, const float& limit){
	snExplosion = sn;
	timeLimit = limit;
	existedTime = 0;
}

Explosion::~Explosion(void){}

Projectile::Projectile(const Ogre::Vector3& origin, Ogre::SceneNode* particle, const Ogre::Degree& angle, const float& velocity, const float& dmg){
	this->origin = origin;
	particleSn = particle;
	this->angle = angle;
	this->velocity = velocity;
	timeExisted = 0;
	this->dmg = dmg;
}

Projectile::~Projectile(void)
{	
}

void Projectile::update(const float& delta, const float& gravity){
	timeExisted += delta;

	Ogre::Vector3 nextPos = Ogre::Vector3::ZERO;

	// x(t) = (v cos phi) t
	nextPos.x = -(velocity * Ogre::Math::Cos(angle) * timeExisted) + origin.x;

	//y(t) = (v sin phi) t - ((gt^2) / 2)
	nextPos.y = (velocity * Ogre::Math::Sin(angle) * timeExisted) - ((gravity * Ogre::Math::Sqr(timeExisted)) / 2) + origin.y;

	//no need to rotate because the SceneNode is already rotated according to the firing Tank's SceneNode

	particleSn->lookAt(nextPos, Ogre::Node::TransformSpace::TS_LOCAL, Ogre::Vector3::UNIT_Z);
	//particleSn->roll(Ogre::Degree(-90.f)); //since PurpleFountain particle system is flying forward, need to rotate backwards
	particleSn->setPosition(nextPos);

}

ProjectileManager::ProjectileManager(void)
{
}


ProjectileManager::~ProjectileManager(void)
{
}

void ProjectileManager::init(Ogre::SceneManager* scnMgr, SoundPlayer* soundPlayer){
	mSceneMgr = scnMgr;

	this->soundPlayer = soundPlayer;

	projectiles.clear();

	gravityValue = 80.f;
	explosionSphere.setCenter(Ogre::Vector3::ZERO);
	explosionSphere.setRadius(15.f);

	counter = 0;
}

void ProjectileManager::createProjectile(const Ogre::Vector3& tankPosition, const Ogre::Quaternion& turretOrientation, 
										 const Ogre::Degree& angle, const float& velocity, const float& dmg){
	
	std::ostringstream oss;
	oss << "Projectile" << time(0) << projectiles.size() << counter++;										 
											 
	Ogre::ParticleSystem* particleSystem = mSceneMgr->createParticleSystem(oss.str(), "Examples/PurpleFountain");

	scaleBy(1.f, particleSystem);

	Ogre::SceneNode* parentParticleSn = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	Ogre::SceneNode* particleSn = parentParticleSn->createChildSceneNode();

	parentParticleSn->yaw(turretOrientation.getYaw());

	particleSn->attachObject(particleSystem);

	particleSn->roll(Ogre::Degree(-90.f));

	particleSn->scale(Ogre::Vector3(0.1f));

	Ogre::Vector3 start = tankPosition;
	
	start.x += 55.f;
	start.y += 6.f;
	start.z += 1600.f;

	particleSn->setPosition(Ogre::Vector3::ZERO);
	parentParticleSn->setPosition(start);

	projectiles.insert(new Projectile(start, particleSn, angle, velocity, dmg));
}

void ProjectileManager::destroyProjectile(Projectile* projectile){
	Ogre::SceneNode* snParticle = projectile->particleSn;

	Ogre::ParticleSystem* particleSys = static_cast<Ogre::ParticleSystem*>(snParticle->getAttachedObject(0));

	// Detach the entity from the scene node
	snParticle->detachObject(particleSys);

	// Delete the entity and the scene node
	mSceneMgr->destroyParticleSystem(particleSys);
	mSceneMgr->destroySceneNode(snParticle);

	delete projectile;
}

void ProjectileManager::destroyExplosion(Explosion* explosion){
	Ogre::SceneNode* snParticle = explosion->snExplosion;

	Ogre::ParticleSystem* particleSys = static_cast<Ogre::ParticleSystem*>(snParticle->getAttachedObject(0));

	// Detach the entity from the scene node
	snParticle->detachObject(particleSys);

	// Delete the entity and the scene node
	mSceneMgr->destroyParticleSystem(particleSys);
	mSceneMgr->destroySceneNode(snParticle);

	delete explosion;
}

/*
Code for this method taken from http://www.ogre3d.org/forums/viewtopic.php?p=176603&sid=ffef6c25bddf82213f3b1e0daf228657#p176603
Adapted for use in this program by Michael Y.M. Kong
*/
void ProjectileManager::scaleBy(Ogre::Real factor, Ogre::ParticleSystem* pSys_)
{
   Ogre::Real temp;
   temp = pSys_->getDefaultWidth();
   temp *= factor;
   pSys_->setDefaultWidth( temp );
   temp = pSys_->getDefaultHeight();
   temp *= factor;
   pSys_->setDefaultHeight( temp );

   for(unsigned short i=0; i<pSys_->getNumEmitters(); ++i)
   {
      Ogre::ParticleEmitter* emitt = pSys_->getEmitter(i);
	  Ogre::Vector3 pos = emitt->getPosition();
      Ogre::Real particleVelocity = emitt->getParticleVelocity();
      Ogre::Real minParticleVelocity = emitt->getMinParticleVelocity();
      Ogre::Real maxParticleVelocity = emitt->getMaxParticleVelocity();

	  pos *= factor;
	  particleVelocity *= factor;
	  minParticleVelocity *= factor;
	  maxParticleVelocity *= factor;

      emitt->setPosition(pos);
      emitt->setParticleVelocity(particleVelocity);
      emitt->setMinParticleVelocity(minParticleVelocity);
      emitt->setMaxParticleVelocity(maxParticleVelocity);
   }
}

void ProjectileManager::createExplosion(const Ogre::Vector3& location){
	std::ostringstream oss;
	oss << "Explosion" << time(0) << explosions.size();	

	Ogre::ParticleSystem* ps = mSceneMgr->createParticleSystem(oss.str(), "explosionTemplate");
	ps->fastForward(1.0f);

	//scaleBy(0.5f, ps);

	Ogre::Vector3 loc = location;

	loc.y += 10.f;

	Ogre::SceneNode* explosionSn = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	explosionSn->attachObject(ps);
	explosionSn->setPosition(loc);
	//explosionSn->scale(Ogre::Vector3(0.1f));
	
	explosions.insert(new Explosion(explosionSn, 1.8f));

	soundPlayer->playExplosionSound(location);
}

void ProjectileManager::update(const float& delta, TankManager* tankMgr){
	for (std::set<Projectile*>::iterator it = projectiles.begin();it != projectiles.end();)
	{
		(*it)->update(delta, gravityValue);
		
		//check if Projectile hit anything
		Ogre::Vector3 explosionCtr = (*it)->particleSn->_getDerivedPosition();
		explosionSphere.setCenter(explosionCtr);
		Ogre::SphereSceneQuery* ssq = mSceneMgr->createSphereQuery(explosionSphere, Ogre::SceneManager::ENTITY_TYPE_MASK);
		Ogre::SceneQueryResult& result = ssq->execute();
		
		//projectile hit something
		if (result.movables.size() > 0 || explosionCtr.y < 0.5f) //if projectile hit some tank or is just above ground
		{
			Ogre::SceneQueryResultMovableList::iterator sqrItr;
			for (sqrItr = result.movables.begin(); sqrItr != result.movables.end(); ++sqrItr){
				//if tank, minus health based on the distance from the explosion center
				tankMgr->checkTankExplosion((*sqrItr)->getParentSceneNode(), explosionCtr, (*it)->dmg);
			}

			createExplosion(explosionCtr);

			destroyProjectile(*it);
			it = projectiles.erase(it); //erase projectile and get new iterator
		} else{
			++it;
		}
		
		mSceneMgr->destroyQuery(ssq);	
	}

	for (std::set<Explosion*>::iterator it = explosions.begin(); it != explosions.end();)
	{
		(*it)->existedTime += delta;

		if ((*it)->existedTime > (*it)->timeLimit)
		{
			destroyExplosion((*it));
			it = explosions.erase(it);
		} else{
			it++;
		}
	}
}