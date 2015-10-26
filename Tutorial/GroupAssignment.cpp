#include "GroupAssignment.h"
 
//-------------------------------------------------------------------------------------
GroupAssignment::GroupAssignment(void)
	: mOverlaySystem(0),
	mPowerupCount(0),
	testhealth(100)
{
}
//-------------------------------------------------------------------------------------
GroupAssignment::~GroupAssignment(void)
{
	if(pathFindingGraph)
		delete pathFindingGraph;

	deletePowerUpSpawns();
	if (mOverlaySystem) 
		delete mOverlaySystem;
	
	if(tankManager)
		delete tankManager;

	if(projectileManager)
		delete projectileManager;

	if (soundPlayer)
		delete soundPlayer;
}

bool GroupAssignment::setup(void)
{
	if (!BaseApplication::setup())
	{
		return false;
	}

	//////////Danni Code
	createWeather();
	////////////
	mTrayMgr->showCursor();

	
	mCurrentState = 0;

	mDirection = Ogre::Vector3::ZERO;

	mRotate = 0.13;
	mMove = 200;
	powerUpTimer = 0;

	ctrlHeld = false;
	buttonHeldCamera = false;
	inTankMode = false;

	// create a params panel for displaying details
	Ogre::StringVector items;
	items.push_back("Camera Pos: ");

	mDetailsPanel2 = mTrayMgr->createParamsPanel(OgreBites::TL_TOPLEFT, "DetailsPanel2", 300, items);


	return true;
};

//-------------------------------------------------------------------------------------
void GroupAssignment::createScene(void)
{
	pathFindingGraph = new Graph;

	soundPlayer = new SoundPlayer;

	projectileManager = new ProjectileManager;
	projectileManager->init(mSceneMgr, soundPlayer);

	tankManager = new TankManager;
	tankManager->init(mSceneMgr, projectileManager, soundPlayer);


	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);


	mCamera->setPosition(Ogre::Vector3(0, 900, 1000));
	mCamera->lookAt(Ogre::Vector3(0, 0, 10));

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.25, 0.25, 0.25));
 
    // create the light
    Ogre::Light *light = mSceneMgr->createLight("Light1");
    light->setType(Ogre::Light::LT_POINT);
    light->setPosition(Ogre::Vector3(250, 150, 250));
    light->setDiffuseColour(Ogre::ColourValue::White);
    light->setSpecularColour(Ogre::ColourValue::White);
 
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
		4500, 4500, 20, 20, true, 1, 2.5, 2.5, Ogre::Vector3::UNIT_Z);

	Ogre::Entity* entGround = mSceneMgr->createEntity("GroundEntity", "ground");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entGround);
	entGround->setMaterialName("Examples/GrassFloor");
	entGround->setCastShadows(false);
	entGround->setQueryFlags(0);

	std::vector<int> walls;
	int rowCheck = 0;
	// go through the graph
	// if a node is blocked, display a cube on that grid location
	for(int nodeNumber=0; nodeNumber<TOTAL_NODES; nodeNumber++)
	{
		int contents = pathFindingGraph->getContent(nodeNumber);

		/*
		if(rowCheck < 35)
		{
			if(rowCheck == 5 || rowCheck == 13 || rowCheck == 23 || rowCheck == 31)
			{
				printf("\t");
			}
			printf("%d", contents);
			rowCheck++;
		}
		else 
		{
			printf("%d\n", contents);
			printf("%d\n", nodeNumber + 1);
			rowCheck = 0;
		}
		*/
		
		if(contents)
		{
			// Create unique name
			std::ostringstream oss;
			oss << nodeNumber;
			std::string entityName = "Cube" + oss.str();

			// Create entity
			Ogre::Entity* cube = mSceneMgr->createEntity(entityName, "cube.mesh");
			cube->setMaterialName("Examples/BumpyMetal");

			// Attach entity to scene node
			Ogre::SceneNode* myNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
			myNode->attachObject(cube);
			myNode->scale(1.25, 1, 1.25);
			
			// Place object at appropriate position
			Ogre::Vector3 position = pathFindingGraph->getPosition(nodeNumber);
			position.y = 50;
			myNode->translate(position);
			walls.push_back(nodeNumber);
		}
		else
		{
			// Create unique name
			std::ostringstream oss;
			oss << nodeNumber;
			std::string entityName = "BoundingBox" + oss.str();

			Ogre::Entity* cube = mSceneMgr->createEntity(entityName, "cube.mesh");
			cube->setQueryFlags(0);

			// Attach entity to scene node
			Ogre::SceneNode* myNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
			myNode->attachObject(cube);
			//myNode->scale(0.1, 0.01, 0.1);
			
			myNode->setVisible(false);
			//myNode->showBoundingBox(true);
			
			// Place object at appropriate position
			Ogre::Vector3 position = pathFindingGraph->getPosition(nodeNumber);
			position.y = 0.5;
			myNode->translate(position);

			boundingBoxes.push_back(myNode);
		}
	}

	//mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

	//WE CREATE THE TANKS HERE 
	std::random_device rd;
	std::mt19937 random(rd());

	std::set<int> randomNumberA;
	int i = 0;
	while(i < 5)
	{
		std::uniform_real_distribution<double> randomGen1(1, 5);
		int tempNumber = randomGen1(random);
		std::uniform_real_distribution<double> randomGen2(0, 36);
		int tempNumber2 = (36 * (int)randomGen2(random)) + (int)tempNumber;
		//printf("&d---%d\n", i, tempNumber2);

		if(randomNumberA.find(tempNumber2) == randomNumberA.end())
		{
			randomNumberA.insert(tempNumber2);
			Ogre::Vector3 position = pathFindingGraph->getPosition(tempNumber2);
			position.y = 0.0;
			tankManager->createTank(position, 1, pathFindingGraph, mPathFinder);
			i++;
		}
	}
	i = 0;
	randomNumberA.clear();
	while(i < 5)
	{
		std::uniform_real_distribution<double> randomGen1(31, 35);
		int tempNumber = randomGen1(random);
		std::uniform_real_distribution<double> randomGen2(0, 36);
		int tempNumber2 = (36 * (int)randomGen2(random)) + (int)tempNumber;
		//printf("%d\n", tempNumber2);

		if(randomNumberA.find(tempNumber2) == randomNumberA.end())
		{
			randomNumberA.insert(tempNumber2);
			Ogre::Vector3 position = pathFindingGraph->getPosition(tempNumber2);
			position.y = 0.0;
			tankManager->createTank(position, 2, pathFindingGraph, mPathFinder);
			i++;
		}
	}

	Ogre::OverlayManager &overlayManager = Ogre::OverlayManager::getSingleton();
	createReloadOverlay(overlayManager);
	createTextOverlay(overlayManager, 0.75, 0.01, 0.24, 0.33);
	createHealthOverlay(overlayManager, 0.74, 0.36, 0.24, 0.04);
	

	/*
	createPowerUpSpawn(pathFindingGraph->getPosition(52)); 
	createPowerUpSpawn(Ogre::Vector3(-100, 10, 0)); 
	createPowerUpSpawn(Ogre::Vector3(-300, 10, 0)); 
	createPowerUpSpawn(Ogre::Vector3(300, 10, 0)); 
	*/
	int factor = 3;
	for(int j = 0; j < 11; j++)
	{
		int tempNode = factor * 36 + 17;
		int tempNode2 = factor * 36 + 18;
		factor += 3;
		
		Ogre::Vector3 firstNodePos = pathFindingGraph->getPosition(tempNode);
		Ogre::Vector3 sndNodePos = pathFindingGraph->getPosition(tempNode2);

		Ogre::Vector3 tempVector = Ogre::Vector3( (firstNodePos.x + sndNodePos.x) / 2, 10, (firstNodePos.z) );

		createPowerUpSpawn(tempVector); 
	}


}

bool GroupAssignment::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;
 
	if(mShutDown)
		return false;
 
	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();

	mTrayMgr->frameRenderingQueued(evt);

	//used for moving the camera, keeping the y position so that it wont move downwards when moving Z
	Ogre::Vector3 temp = Ogre::Vector3(0, mCamera->getPosition().y, 0); 
	mCamera->moveRelative(mDirection * mMove * evt.timeSinceLastFrame/1000);
	mCamera->setPosition(Ogre::Vector3(mCamera->getPosition().x, temp.y, mCamera->getPosition().z));


	//UPDATING DETAILS PANEL
	if (!mTrayMgr->isDialogVisible())
	{
		mCameraMan->frameRenderingQueued(evt);   // if dialog isn't up, then update the camera

		Ogre::String robotPos = std::to_string((int)mCamera->getPosition().x) + "." + std::to_string((int)mCamera->getPosition().y) + "." + std::to_string((int)mCamera->getPosition().z);
		mDetailsPanel2->setParamValue(0, robotPos);
	}

	//update the tank manager and projectile manager
	tankManager->update(evt.timeSinceLastFrame, mPowerUpSpawns);
	projectileManager->update(evt.timeSinceLastFrame,tankManager);

	///////////////////////////////////////////////////////////////////////////////////////////////

	//also needs a check for if we change which tank is selected
	
	
	if (mTextGUI->isVisible() && selectedTank != NULL)
	{
		updateTextOverlay();
		updateReloadOverlay(selectedTank->weaponTimer);
		if (mPrevHealth != selectedTank->hp)
		{
			mPrevHealth = selectedTank->hp;
			updateHealthOverlay(selectedTank->hp); //pass the health
		}
	}	
	updatePowerUps(evt.timeSinceLastFrame);

	powerUpTimer += evt.timeSinceLastFrame;
	if(powerUpTimer > 20)
	{
		if(spawnLocations.size() > 0)
		{
			for (std::set<int>::iterator it = spawnLocations.begin(); it != spawnLocations.end(); ++it)
			{
				if(mPowerUpSpawns[*it]->getIsPowerUp())
					mPowerUpSpawns[*it]->pickupPowerUp(mSceneMgr);
			}
			spawnLocations.clear();
		}
	}		
	if(powerUpTimer > 25)
	{
		int i = 0;
		while(i < 5)
		{
			int tempRand = rand() % 11;

			if(spawnLocations.find(tempRand) == spawnLocations.end())
			{
				spawnLocations.insert(tempRand);
				mPowerUpSpawns[tempRand]->createPowerUp(mSceneMgr, '?');
				i++;
			}
		}
		powerUpTimer = 0;
	}	
	return true;
}

// OIS::KeyListener
bool GroupAssignment::keyPressed( const OIS::KeyEvent &arg )
{
    switch (arg.key)
	{
		case OIS::KC_ESCAPE: 
			mShutDown = true;
		break;
		//////////////////////////////DANNI CODE
		case OIS::KC_U:
		{
			printf("Health: %f\n", selectedTank->hp); 
		}
		break;
		case OIS::KC_K:
		
		if (!inTankMode && selectedTank != NULL)
		{
			selectedTank->attachCamera(mCamera);
			inTankMode = true;
			mTrayMgr->hideCursor();
			selectedTank->setPossessed(true);

			soundPlayer->playMovingTank();
		}
		break;
		case OIS::KC_P:
			mWindow->writeContentsToTimestampedFile("GroupProject", ".png");
			break;
		case OIS::KC_V:
			controlWeather();
		break;		
		case OIS::KC_L:
		{
			if (mTextGUI->isVisible())
			{
				mTextGUI->hide();
				mBulletAnimation->hide();
				mHealthBar->hide();
			}
			else
			{
				mTextGUI->show();
				mBulletAnimation->show();
				mHealthBar->show();

			}
		}
		break;	

		/////////////////////////////
		case OIS::KC_LSHIFT:
			mTrayMgr->hideCursor();
			buttonHeldCamera = true;
		break;
		case OIS::KC_LCONTROL:
			ctrlHeld = true;
		break;
		case OIS::KC_1: //spawn tanks at A
			createTanks(1);
		break;
		case OIS::KC_2:
			createTanks(2);
		break;
		case OIS::KC_0:
			if(inTankMode)
			{
				selectedTank->detachCamera(mCamera);
				mCamera->setPosition(Ogre::Vector3(0, 900, 1000));
				mCamera->lookAt(Ogre::Vector3(0, 0, 10));
				selectedTank->setSelected(false);
				inTankMode = false;
				mTrayMgr->showCursor();
				selectedTank->setPossessed(false);
				mTextGUI->hide();
				mBulletAnimation->hide();
				mHealthBar->hide();

				soundPlayer->stopSounds();

				//selectedTank = NULL;
			}
		break;
		case OIS::KC_9: //show bounding box
			for(int i = 0; i < boundingBoxes.size(); i++)
			{
				boundingBoxes[i]->showBoundingBox(!boundingBoxes[i]->getShowBoundingBox());
			}
		break;
		default:
			break;
	}

	if(inTankMode){
		switch (arg.key)
		{
			case OIS::KC_W:
				selectedTank->mMove -= 5.0;
				break;

			case OIS::KC_S:
				selectedTank->mMove += 5.0;
				break;

			case OIS::KC_A:
				selectedTank->bodyRotate += 1.0;
				break;

			case OIS::KC_D:
				selectedTank->bodyRotate -= 1.0;
				break;

			case OIS::KC_LEFT:
				selectedTank->turretRotation += 1.0;
				break;
 
			case OIS::KC_RIGHT:
				selectedTank->turretRotation -= 1.0;
				break;

			case OIS::KC_UP:
				selectedTank->barrelRotation = 1.0;
				break;
 
			case OIS::KC_DOWN:
				selectedTank->barrelRotation = -1.0;
				break;
			case OIS::KC_SPACE:
				if (selectedTank->weaponTimer > 4)
				{
					selectedTank->shoot();
					selectedTank->weaponTimer = 0.f;
					soundPlayer->playFireSound();
				}
				break;
			default:
				break;
		}
	}

	return true;
}

bool GroupAssignment::keyReleased( const OIS::KeyEvent &arg )
{
	switch (arg.key)
	{
		case OIS::KC_LSHIFT:
			mTrayMgr->showCursor();
			buttonHeldCamera = false;
		break; 
		case OIS::KC_LCONTROL:
			ctrlHeld = false;
		break;
		default:
			break;
	}

	if(inTankMode){
		switch (arg.key)
		{
			case OIS::KC_W:
				selectedTank->mMove = 0;
				break;

			case OIS::KC_S:
				selectedTank->mMove = 0;
				break;

			case OIS::KC_A:
				selectedTank->bodyRotate -= 1.0;
				break;

			case OIS::KC_D:
				selectedTank->bodyRotate += 1.0;
				break;

			case OIS::KC_LEFT:
				selectedTank->turretRotation -= 1.0;
				break;
 
			case OIS::KC_RIGHT:
				selectedTank->turretRotation += 1.0;
				break;

			case OIS::KC_UP:
				selectedTank->barrelRotation -= 1.0;
				break;
 
			case OIS::KC_DOWN:
				selectedTank->barrelRotation += 1.0;
				
				break;

			default:
				break;
		}
	}

	return true;
}

// OIS::MouseListener
bool GroupAssignment::mouseMoved( const OIS::MouseEvent &arg )
{
	if (mTrayMgr->injectMouseMove(arg)) return true;

	if(!inTankMode)
	{
		if(mMouse->getMouseState().X.abs > (mWindow->getWidth() - 20)) 
		{ 
			mDirection.x += mMove; 
			//mDirection.z -= mMove; 
		}
		else if(mMouse->getMouseState().X.abs < (20)) 
		{ 
			mDirection.x -= mMove; 
			//mDirection.z -= mMove; 
		} 
		else
		{
			mDirection.x = 0;
		}

		if(mMouse->getMouseState().Y.abs > (mWindow->getHeight() - 20))
		{
			mDirection.z += mMove;
		}
		else if (mMouse->getMouseState().Y.abs < (20))
		{
			mDirection.z -= mMove;
		}
		else
		{
			mDirection.z = 0;
		}
	
		if(mMouse->getMouseState().Z.rel != 0)
		{
			mCamera->moveRelative(Ogre::Vector3(0, 0, -mMouse->getMouseState().Z.rel * 0.15));
		}

		//used to rotate and look around for the camera
		if(buttonHeldCamera == true)
		{
			mCamera->yaw(Ogre::Degree(-mRotate * arg.state.X.rel));
			mCamera->pitch(Ogre::Degree(-mRotate * arg.state.Y.rel));
		}
	}
	

	return true;
}

bool GroupAssignment::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	switch (id)
	{
		case OIS::MB_Left:
			{
				if(!inTankMode)
				{
						// Get the mouse ray, i.e. ray from the mouse cursor 'into' the screen 
					Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(
						static_cast<float>(mMouse->getMouseState().X.abs)/mMouse->getMouseState().width, 
						static_cast<float>(mMouse->getMouseState().Y.abs)/mMouse->getMouseState().height);
					Ogre::RaySceneQuery* mRaySceneQuery = mSceneMgr->createRayQuery(mouseRay, Ogre::SceneManager::ENTITY_TYPE_MASK);
					mRaySceneQuery->setSortByDistance(true);
					// Ray-cast and get first hit
					Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
					Ogre::RaySceneQueryResult::iterator itr = result.begin();
					if(itr != result.end() && itr->movable)
					{
						if (itr->movable != mSceneMgr->getEntity("GroundEntity"))
						{
							Ogre::SceneNode* tankNode = itr->movable->getParentSceneNode();
							selectedTank = tankManager->toggleTankSelected(tankNode);

							if(selectedTank != NULL)
							{
								//GUI stuff
								mTextGUI->show();
								mBulletAnimation->show();
								mHealthBar->show();
								mPrevHealth = selectedTank->hp;
								updateHealthOverlay(selectedTank->hp);
								updateTextOverlay();

							}
						}
					}
					mSceneMgr->destroyQuery(mRaySceneQuery);
				}
			}
			break;
		default:
			break;
	}

	return true;
}

void GroupAssignment::createPath(Ogre::ManualObject* line, float height, std::vector<int>& path, Ogre::ColourValue& colour)
{
	line->clear();

	// Specify the material and rendering type
	line->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);

	// Specify the vertices and vertex colour for the line
	Ogre::Vector3 position;

	for(std::vector<int>::iterator it=path.begin(); it!=path.end(); it++)
	{
		position = pathFindingGraph->getPosition(*it);
		line->position(Ogre::Vector3(position.x, height, position.z));
		line->colour(colour);
	}

	// Finished defining line
	line->end();
}

void GroupAssignment::createTanks(int side)
{
	std::random_device rd;
	std::mt19937 random(rd());

	int tempNumber = -1;
	int tempNumber2 = -1;
	bool check = false;
	std::vector<Ogre::Vector3> tankLocations = tankManager->getPositionTank(side); //returns all the position of the tanks in the scene 

	while(check == false)
	{
		if(side == 1)
		{
			std::uniform_real_distribution<double> randomGen1(0, 5);
			tempNumber = randomGen1(random);
			std::uniform_real_distribution<double> randomGen2(0, 36);
			tempNumber2 = (36 * (int)randomGen2(random)) + (int)tempNumber;

			for(int i = 0; i < tankLocations.size() && check == false; i++)
			{
				if (tempNumber2 != pathFindingGraph->getNode(tankLocations[i]))
				{
					check = true;

					Ogre::Vector3 position = pathFindingGraph->getPosition(tempNumber2);
					position.y = 0.0;
					tankManager->createTank(position, side, pathFindingGraph, mPathFinder);
				}
			}
		}
		else if(side == 2)
		{
			std::uniform_real_distribution<double> randomGen1(31, 36);
			tempNumber = randomGen1(random);
			std::uniform_real_distribution<double> randomGen2(0, 36);
			tempNumber2 = (36 * (int)randomGen2(random)) + (int)tempNumber;

			for(int i = 0; i < tankLocations.size() && check == false; i++)
			{
				if (tempNumber2 != pathFindingGraph->getNode(tankLocations[i]))
				{
					check = true;

					Ogre::Vector3 position = pathFindingGraph->getPosition(tempNumber2);
					position.y = 0.0;
					tankManager->createTank(position, side, pathFindingGraph, mPathFinder);
				}
			}
		}
	}
}

void GroupAssignment::createWeather()
{
	// Create a rainstorm
	
	Ogre::ParticleSystem * snowPSys = mSceneMgr->createParticleSystem("snow", "myParticles/Snow");
	Ogre::ParticleSystem * rainPSys = mSceneMgr->createParticleSystem("rain", "myParticles/Rain");

	Ogre::SceneNode* snowNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("snownode");
	snowNode->translate(0,500,0);
	snowNode->attachObject(snowPSys);



	Ogre::SceneNode* rainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("rainnode");
	rainNode->translate(0,500,0);
	rainNode->attachObject(rainPSys);
	rainPSys->fastForward(5);

	snowNode->setVisible(false);
	rainNode->setVisible(false);
}

void GroupAssignment::controlWeather()
{
	Ogre::SceneNode* snowNode = mSceneMgr->getSceneNode("snownode");
	Ogre::SceneNode* rainNode = mSceneMgr->getSceneNode("rainnode");

	//6 is rain, 5 is snow
				
	if(rainNode->getAttachedObject("rain")->isVisible())
	{
		rainNode->setVisible(false);
	}
	else if (snowNode->getAttachedObject("snow")->isVisible())
	{
		snowNode->setVisible(false);
		rainNode->setVisible(true);

		//remove fog
		mWindow->getViewport(0)->setBackgroundColour(Ogre::ColourValue(0,0,0));
		mSceneMgr->setFog(Ogre::FOG_NONE);
	}
	else
	{
		snowNode->setVisible(true);

		//creates some fog to add to snow illusion
		Ogre::ColourValue fadeColour(0.9, 0.9, 0.9);
		mWindow->getViewport(0)->setBackgroundColour(fadeColour); 
		mSceneMgr->setFog(Ogre::FOG_EXP, fadeColour, 0.0005);

		//materials that use scene_blend add (with a background colour that matches the viewport background colour)
		//instead of simply using scene_blend alpha_blend with a transparent background
		//will render the fog colour over the material's background
		//this can lead to strange looking particles

	}

		//alternative approach is to do something like
		//weatherNode->detachObject("rain");
		//weatherNode->attachObject(mParticleSystem[6]);
}

void GroupAssignment::createReloadOverlay(Ogre::OverlayManager &overlayManager)
{
	mBulletAnimation = overlayManager.create( "BulletAnimation" );

	Ogre::Real xstart = 0.80;
	Ogre::Real xwidth = 0.025;
	Ogre::Real ystart = 0.4275;
	Ogre::Real yheight = 0.05;

	Ogre::OverlayContainer * panel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "BulletBorder" ) );
    panel->setPosition( xstart, ystart );
    panel->setDimensions( xwidth*5 + 0.015, yheight+0.005 );
    panel->setMaterialName( "myBorder/BulletBorder" );
    mBulletAnimation->add2D( panel );

	//panel is Ogre::OverlayContainer* panel
	panel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "Bullet1" ) );
    panel->setPosition( xstart + 0.01, ystart+0.0035 );
    panel->setDimensions( xwidth, yheight);
    panel->setMaterialName( "myMaterial/Bullet1" );
	panel->hide();
    mBulletAnimation->add2D( panel );

	panel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "Bullet2" ) );
    panel->setPosition( xstart + xwidth + 0.01, ystart + 0.0025 );
    panel->setDimensions( xwidth, yheight);
    panel->setMaterialName( "myMaterial/Bullet2" );
	panel->hide();
    mBulletAnimation->add2D( panel );

	panel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "Bullet3" ) );
    panel->setPosition( xstart + xwidth *2 + 0.01, ystart + 0.0025 );
    panel->setDimensions( xwidth, yheight);
    panel->setMaterialName( "myMaterial/Bullet3" );
	panel->hide();
    mBulletAnimation->add2D( panel );

	panel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "Bullet4" ) );
    panel->setPosition( xstart + xwidth *3 + 0.01, ystart + 0.0025 );
    panel->setDimensions( xwidth, yheight);
    panel->setMaterialName( "myMaterial/Bullet4" );
	panel->hide();
    mBulletAnimation->add2D( panel );

	panel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "Bullet5" ) );
    panel->setPosition( xstart + xwidth *4 + 0.01, ystart + 0.0025 );
    panel->setDimensions( xwidth, yheight);
    panel->setMaterialName( "myMaterial/Bullet5" );
	panel->hide();
    mBulletAnimation->add2D( panel );

    // Hide the overlay
    mBulletAnimation->hide();
}

void GroupAssignment::updateReloadOverlay(double time)
{
	if (time < 0.8) //if we change tank selected we should also hide all these as well to make a clean slate
	{
		mBulletAnimation->getChild("Bullet1")->hide();
		mBulletAnimation->getChild("Bullet2")->hide();
		mBulletAnimation->getChild("Bullet3")->hide();
		mBulletAnimation->getChild("Bullet4")->hide();
		mBulletAnimation->getChild("Bullet5")->hide();
	}
	else if (time > 0.8)
	{
		mBulletAnimation->getChild("Bullet1")->show();
		if (time > 1.6)
		{
			mBulletAnimation->getChild("Bullet2")->show();
			if (time > 2.4)
			{
				mBulletAnimation->getChild("Bullet3")->show();
				if (time > 3.2)
				{
					mBulletAnimation->getChild("Bullet4")->show();
					if (time > 4)
					{
						mBulletAnimation->getChild("Bullet5")->show();
					}
				}
			}
		}
	}

}

void GroupAssignment::createPowerUpSpawn(Ogre::Vector3 position)
{
	std::ostringstream oss;
	oss << ++mPowerupCount;
	std::string name = "PowerUpSpawn" + oss.str();
	std::string namePodium = "Podium" + oss.str();
	std::cout << namePodium << std::endl;

	Ogre::SceneNode * spawnPointNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(position);
	//attach an entity to represent a point where powerups spawn
	Ogre::Entity * spawnPointEnt = mSceneMgr->createEntity(namePodium, "podium.mesh");
	spawnPointNode->attachObject(spawnPointEnt);

	PowerUpSpawn * spawn = new PowerUpSpawn(spawnPointNode, name);

	mPowerUpSpawns.push_back(spawn);

}

void GroupAssignment::updatePowerUps(float deltaTime)
{
	std::vector<PowerUpSpawn *>::iterator it;
	for (it = mPowerUpSpawns.begin(); it != mPowerUpSpawns.end(); it++)
	{
		(*it)->update(mSceneMgr, deltaTime);
	}
}

void GroupAssignment::deletePowerUpSpawns()
{
	std::vector<PowerUpSpawn *>::iterator it = mPowerUpSpawns.begin();
	while (it != mPowerUpSpawns.end())
	{
		delete (*it);
		it++;
	}

}

void GroupAssignment::createTextElement(Ogre::OverlayManager &overlayManager, Ogre::String title, Ogre::String caption, Ogre::Real x, Ogre::Real y)
{
	Ogre::OverlayContainer * panel2 = static_cast<Ogre::OverlayContainer*>(overlayManager.createOverlayElement("Panel",title + "C"));
	panel2->setMetricsMode(Ogre::GMM_RELATIVE);    
	panel2->setDimensions(0.2, 0.2);
    panel2->setPosition(x, y);
     
	Ogre::TextAreaOverlayElement *textArea2 =static_cast<Ogre::TextAreaOverlayElement*>(overlayManager.createOverlayElement("TextArea", title + "E"));
	textArea2->setMetricsMode(Ogre::GMM_RELATIVE);
    textArea2->setPosition(0, 0);
    textArea2->setDimensions(1, 1);
	textArea2->setCaption(caption);
    textArea2->setFontName("BlueHighway");
    textArea2->setCharHeight(0.02f);
	panel2->addChild(textArea2);
	mTextGUI->add2D(panel2);

}

void GroupAssignment::createTextOverlay(Ogre::OverlayManager &overlayManager, Ogre::Real x, Ogre::Real y, Ogre::Real width, Ogre::Real height)
{
	mTextGUI = overlayManager.create( "TextArea" );	

	Ogre::OverlayContainer * panela = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "TextBorder" ) );
    panela->setPosition( x, y ); //0.75 , 0.01
    panela->setDimensions( width, height ); //0.24, 0.35
    panela->setMaterialName( "myBorder/TextBorder" );
    mTextGUI->add2D( panela );



////////////////////////////////////////////////////////
	Ogre::Real x1 = 0.78;
	Ogre::Real x2 = 0.86;
	Ogre::Real x3 = x2 + 0.03;
	Ogre::Real y1 = 0.05;
	Ogre::Real ySep = 0.04;

	createTextElement(overlayManager, "Dummy", "", 0.8, 0.1); //first doesnt show, wp ogre
	
	createTextElement(overlayManager, "Name		:", "Name", x1, y1);
	createTextElement(overlayManager, "State	:", "State", x1, y1 + ySep);
	createTextElement(overlayManager, "Position	:", "Position", x1, y1 + 2 * ySep);
	createTextElement(overlayManager, "Powerups	:", "Powerups", x1, y1 + 3 * ySep);
	
	createTextElement(overlayManager, "NameV", "Bob the tank", x2, y1);
	createTextElement(overlayManager, "StateV", "Idle", x2, y1 + ySep);
	createTextElement(overlayManager, "PositionV", "0,0,0", x2, y1 + 2 * ySep);
	createTextElement(overlayManager, "RPowerupsV", "None", x3, y1 + 3 * ySep);
	createTextElement(overlayManager, "SPowerupsV", "None", x3, y1 + 4 * ySep);
	createTextElement(overlayManager, "PPowerupsV", "None", x3, y1 + 5 * ySep);

	panela = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "BoltIcon" ) );
    panela->setPosition( x2, y1 + 3 * ySep);
    panela->setDimensions( 0.02, 0.02 );
    panela->setMaterialName( "myIcon/Bolt" );
    mTextGUI->add2D( panela );

	panela = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "SpeedIcon" ) );
    panela->setPosition( x2, y1 + 4 * ySep);
    panela->setDimensions( 0.02, 0.02 );
    panela->setMaterialName( "myIcon/Wheel" );
    mTextGUI->add2D( panela );


  	panela = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "NukeIcon" ) );
    panela->setPosition( x2, y1 + 5 * ySep);
    panela->setDimensions( 0.02, 0.02 );
    panela->setMaterialName( "myIcon/Nuke" );
    mTextGUI->add2D( panela );

	mTextGUI->hide();
	
}

void GroupAssignment::createHealthBlock(Ogre::OverlayManager &overlayManager, Ogre::String title, Ogre::Real x, Ogre::Real y, Ogre::Real width, Ogre::Real height)
{
	Ogre::OverlayContainer * panel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", title ) );
    panel->setPosition( x, y );
    panel->setDimensions( width, height);
    panel->setMaterialName( "myMaterial/HealthBlock" );
    
	mHealthBar->add2D( panel );
}

void GroupAssignment::createHealthOverlay(Ogre::OverlayManager &overlayManager, Ogre::Real x, Ogre::Real y, Ogre::Real width, Ogre::Real height)
{
	mHealthBar = overlayManager.create( "HealthBar" );	

	Ogre::Real xstart = x; //0.74
	Ogre::Real ystart = y; //0.38
	Ogre::Real xwidth = (width-0.005)/20; //0.01175 = (0.24-0.005)/20
	Ogre::Real yheight = height; //0.04

	Ogre::OverlayContainer * panela = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "HealthBorder" ) );
    panela->setPosition( xstart + xwidth - 0.0025, ystart - 0.005 );
    panela->setDimensions( xwidth*20 + 0.005, yheight + 0.01 );
    panela->setMaterialName( "myBorder/HealthBorder" );
    mHealthBar->add2D( panela );



	for (int i = 1; i < 21; i++)
	{
		std::ostringstream oss;
		oss << i; 

		createHealthBlock(overlayManager, "Health" + oss.str(), xstart + i * xwidth, ystart, xwidth, yheight);
	}

	mHealthBar->hide();
}

void GroupAssignment::updateTextOverlay()
{
	//Name
	mTextGUI->getChild("NameVC")->getChild("NameVE")->setCaption("Test");
	//State
	mTextGUI->getChild("StateVC")->getChild("StateVE")->setCaption(	selectedTank->getState());

	//Position
	std::ostringstream oss;
	Ogre::Vector3 pos = selectedTank->getPosition();
	oss << '(' << (int)pos.x << ',' << (int)pos.y << ',' << (int)pos.z << ')';
	mTextGUI->getChild("PositionVC")->getChild("PositionVE")->setCaption(oss.str());

	//Powerups
	oss.str("");
	oss.clear();
	
	oss << selectedTank->powerUpDurationR;
	mTextGUI->getChild("RPowerupsVC")->getChild("RPowerupsVE")->setCaption(oss.str());
	oss.str("");
	oss.clear();
	
	oss << selectedTank->powerUpDurationS;
	mTextGUI->getChild("SPowerupsVC")->getChild("SPowerupsVE")->setCaption(oss.str());

	oss.str("");
	oss.clear();
	
	oss << selectedTank->powerUpDurationP;
	mTextGUI->getChild("PPowerupsVC")->getChild("PPowerupsVE")->setCaption(oss.str());

		
}

void GroupAssignment::updateHealthOverlay(float health)
{
	int x = health * 200;
	int visHealth; //how many health blocks to show

	visHealth = 1 + (x-1)/10;

	for (int i = 1; i < visHealth + 1; i++)
	{
		std::ostringstream oss;
		oss << i; 
		mHealthBar->getChild("Health" + oss.str())->show();
	}

	for (int i = visHealth + 1; i < 21; i++)
	{
		std::ostringstream oss;
		oss << i; 
		mHealthBar->getChild("Health" + oss.str())->hide();
	}
}






 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        GroupAssignment app;
 
        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }
 
        return 0;
    }
 
#ifdef __cplusplus
}
#endif