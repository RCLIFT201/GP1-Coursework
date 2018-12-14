/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();

/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}

void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);

	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	// Players score
	theScore = 0;

	theAreaClicked = { 0, 0 };


	// Store the sprite and background textures
	textureName = { "Enemy", "bullet_clipart", "explosion", "Turret", "MainMenuBackground","GameOverBackground" };
	texturesToUse = { "Images\\Sprites\\Enemy.png", "Images\\Sprites\\Bullet_ClipArt.png", "Images\\Sprites\\explosion.png", "Images\\Sprites\\Turret.png", "Images\\Bkg\\MainMenuBackground.png", "Images\\Bkg\\GameOverBackground.png" };
	for (int tCount = 0; tCount < (int)textureName.size(); tCount++)
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}

	// Store the button textures
	btnNameList = { "quit_button", "menu_button", "play_button", };
	btnTexturesToUse = { "Buttons/quit_button.png", "Buttons/menu_button.png", "Buttons/play_button.png" };
	btnPos = { { 400, 375 },{ 400, 300 },{ 400, 300 } };
	for (unsigned int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]);
	}
	for (unsigned int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight());
		theButtonMgr->add(btnNameList[bCount], newBtn);
	}

	theGameState = gameState::menu;
	theBtnType = btnTypes::exit;

	// Create textures for Game Dialogue (text)
	fontList = { "retro" };
	fontsToUse = { "Fonts/retro.ttf" };
	for (int fonts = 0; fonts < (int)fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 48);
	}
	gameTextList = { "Robot Defence", "Score : "};
	strScore = gameTextList[1];
	strScore += to_string(theScore).c_str();
	
	theTextureMgr->addTexture("Title", theFontMgr->getFont("retro")->createTextTexture(theRenderer, gameTextList[0], textType::solid, { 255, 0, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("theScore", theFontMgr->getFont("retro")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 255, 0, 0, 255 }, { 0, 0, 0, 0 }));

	// Create text Textures
	gameTextNames = { "gameOver", "SeeYouTxt", "robotsKilled", "moveText", "shootText" };
	gameTextList = { "GAME OVER", "See you again soon!", "Score : ", "- Use the arrow keys to move", "- Use spacebar to shoot the robots" };
	for (unsigned int text = 0; text < gameTextNames.size(); text++)
	{
		if (text == 0 || text == gameTextNames.size() - 1)
		{
			theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("retro")->createTextTexture(theRenderer, gameTextList[text], textType::solid, { 255, 0, 0, 255 }, { 0, 0, 0, 0 }));
		}
		else
		{
			theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("retro")->createTextTexture(theRenderer, gameTextList[text], textType::solid, { 255, 0, 0, 255 }, { 0, 0, 0, 0 }));
		}
	}

	// Load game sounds
	soundList = { "gameTheme", "menuMusic", "laserShot", "explosion", "gameOver" };
	soundTypes = { soundType::music, soundType::music, soundType::sfx, soundType::sfx, soundType::sfx };
	soundsToUse = { "Audio/InGameTheme.wav", "Audio/menuMusic.wav", "Audio/laserShot.wav", "Audio/explosion2.aiff", "Audio/gameOver.wav" };
	for (int sounds = 0; sounds < (int)soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("gameTheme")->play(-1);

	// Set Main Menu Background
	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("MainMenuBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("MainMenuBackground")->getTWidth(), theTextureMgr->getTexture("MainMenuBackground")->getTHeight());

	// Set Game Over Background
	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("GameOverBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("GameOverBackground")->getTWidth(), theTextureMgr->getTexture("GameOverBackground")->getTHeight());

	// Set sprite of the Turret
	theTurret.setSpritePos({ 450, 640 });
	theTurret.setTexture(theTextureMgr->getTexture("Turret"));
	theTurret.setSpriteDimensions(theTextureMgr->getTexture("Turret")->getTWidth(), theTextureMgr->getTexture("Turret")->getTHeight());
	theTurret.setTurretVelocity(75);
	theTurret.setSpriteTranslation({ 50,50 });

	 // Create vector array of textures for robots
	for (int astro = 0; astro < 5; astro++)
	{
		theRobots.push_back(new cRobot);
		theRobots[astro]->setSpritePos({ (80 * (astro + 1) + (60*astro)), -50 }); // Enemies randomly spawn on x-axis, and always on 0 on the y-axis
		theRobots[astro]->setSpriteTranslation({ 0, -120 }); // Moves enemies down
		int randRobot = RobotTextDis(gen);
		theRobots[astro]->setTexture(theTextureMgr->getTexture(textureName[randRobot]));
		theRobots[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randRobot])->getTWidth(), theTextureMgr->getTexture(textureName[randRobot])->getTHeight());
		theRobots[astro]->setRobotVelocity(200);
		theRobots[astro]->setActive(true);
	}

	gameOver = false;
}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	switch (theGameState)
	{

	// Renders Main Menu game state
	case gameState::menu:
	{
		// Render the Background
		spriteBkgd.setTexture(theTextureMgr->getTexture("MainMenuBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("MainMenuBackground")->getTWidth(), theTextureMgr->getTexture("MainMenuBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("Title");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		// Render the Move Text
		tempTextTexture = theTextureMgr->getTexture("moveText");
		pos = { 50, 70, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		// Render the Shoot Text
		tempTextTexture = theTextureMgr->getTexture("shootText");
		pos = { 50, 110, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		// Render Buttons
		theButtonMgr->getBtn("play_button")->setSpritePos({ 400, 375 });
		theButtonMgr->getBtn("play_button")->render(theRenderer, &theButtonMgr->getBtn("play_button")->getSpriteDimensions(), &theButtonMgr->getBtn("play_button")->getSpritePos(), theButtonMgr->getBtn("play_button")->getSpriteScale());
		theButtonMgr->getBtn("quit_button")->setSpritePos({ 400, 475 });
		theButtonMgr->getBtn("quit_button")->render(theRenderer, &theButtonMgr->getBtn("quit_button")->getSpriteDimensions(), &theButtonMgr->getBtn("quit_button")->getSpritePos(), theButtonMgr->getBtn("quit_button")->getSpriteScale());

		gameOver = false;
	}
	break;

	// Renders Playing game state
	case gameState::playing:
	{
		// Render the Background
		spriteBkgd.setTexture(theTextureMgr->getTexture("MainMenuBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("MainMenuBackground")->getTWidth(), theTextureMgr->getTexture("MainMenuBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

		// Render Button
		theButtonMgr->getBtn("quit_button")->setSpritePos({ 20, 640 });
		theButtonMgr->getBtn("quit_button")->render(theRenderer, &theButtonMgr->getBtn("quit_button")->getSpriteDimensions(), &theButtonMgr->getBtn("quit_button")->getSpritePos(), theButtonMgr->getBtn("quit_button")->getSpriteScale());

		// Render each robot in the vector array
		for (int draw = 0; draw < (int)theRobots.size(); draw++)
		{
			theRobots[draw]->render(theRenderer, &theRobots[draw]->getSpriteDimensions(), &theRobots[draw]->getSpritePos(), theRobots[draw]->getSpriteRotAngle(), &theRobots[draw]->getSpriteCentre(), theRobots[draw]->getSpriteScale());
		}

		// Render each bullet in the vector array
		for (int draw = 0; draw < (int)theBullets.size(); draw++)
		{
			theBullets[draw]->render(theRenderer, &theBullets[draw]->getSpriteDimensions(), &theBullets[draw]->getSpritePos(), theBullets[draw]->getSpriteRotAngle(), &theBullets[draw]->getSpriteCentre(), theBullets[draw]->getSpriteScale());
		}

		// Render each explosion in the vector array
		for (int draw = 0; draw < (int)theExplosions.size(); draw++)
		{
			theExplosions[draw]->render(theRenderer, &theExplosions[draw]->getSourceRect(), &theExplosions[draw]->getSpritePos(), theExplosions[draw]->getSpriteScale());
		}

		// Render the Title
		cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
		SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		FPoint scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		// Render the score
		if (updateScore)
		{
			theTextureMgr->deleteTexture("theScore");
			theTextureMgr->addTexture("theScore", theFontMgr->getFont("retro")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 255, 0, 0, 255 }, { 0, 0, 0, 0 }));
			updateScore = false;
		}
		tempTextTexture = theTextureMgr->getTexture("theScore");
		pos = { 700, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		// render the turret
		theTurret.render(theRenderer, &theTurret.getSpriteDimensions(), &theTurret.getSpritePos(), theTurret.getSpriteRotAngle(), &theTurret.getSpriteCentre(), theTurret.getSpriteScale());
	}
	break;

	// Renders Game Over game state
	case gameState::end:
	{
		// Render the background
		spriteBkgd.setTexture(theTextureMgr->getTexture("GameOverBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("GameOverBackground")->getTWidth(), theTextureMgr->getTexture("GameOverBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

		//Render the Title
		cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
		SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		
		// Render Game Over Text
		tempTextTexture = theTextureMgr->getTexture("gameOver");
		pos = { 425, 150, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		
		// Render Buttons
		theButtonMgr->getBtn("menu_button")->setSpritePos({ 500, 375 });
		theButtonMgr->getBtn("menu_button")->render(theRenderer, &theButtonMgr->getBtn("menu_button")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_button")->getSpritePos(), theButtonMgr->getBtn("menu_button")->getSpriteScale());
		theButtonMgr->getBtn("quit_button")->setSpritePos({ 500, 475 });
		theButtonMgr->getBtn("quit_button")->render(theRenderer, &theButtonMgr->getBtn("quit_button")->getSpriteDimensions(), &theButtonMgr->getBtn("quit_button")->getSpritePos(), theButtonMgr->getBtn("quit_button")->getSpriteScale());

		// Render Score on End Screen
		tempTextTexture = theTextureMgr->getTexture("theScore");
		pos = { 700, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	}
	break;

	// Renders Quit game state
	case gameState::quit:
	{
		loop = false;
	}
	break;
	default:
		break;
	}
	SDL_RenderPresent(theRenderer);

}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{
	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{
	// Keeps the player within the boundary of the screen, if they hit the edge then they bounce back
	IPoint turretPos = { (int)theTurret.getSpritePos().x, (int)theTurret.getSpritePos().y };
	if (turretPos.X >= 910 || turretPos.X <= 0)
	{
		theTurret.setSpritePos({ theTurret.getSpritePos().x, theTurret.getSpritePos().y });
		theTurret.setTurretMove(theTurret.getTurretMove()*(-1));
	}

	// Check Button clicked and change state
	if (theGameState == gameState::menu || theGameState == gameState::end)
	{
		theGameState = theButtonMgr->getBtn("quit_button")->update(theGameState, gameState::quit, theAreaClicked);
	}
	else
	{
		theGameState = theButtonMgr->getBtn("quit_button")->update(theGameState, gameState::end, theAreaClicked);
	}

	theGameState = theButtonMgr->getBtn("menu_button")->update(theGameState, gameState::menu, theAreaClicked);

	if (theGameState == gameState::menu)
	{
		theGameState = theButtonMgr->getBtn("play_button")->update(theGameState, gameState::playing, theAreaClicked);
		gameOver = false;

		// If on Main Menu, reset the score
		theScore = 0;
		strScore = gameTextList[2];
		strScore += to_string(theScore).c_str();
		updateScore = true;
	}

	if (theGameState == gameState::end)
	{
		// clears the robots and bullets so that the game can be replayed
		theRobots.clear();
		theBullets.clear();
	}

	if (theGameState == gameState::playing && gameOver == false)
	{
		gameOver = false;

		// Update the visibility and position of each robot
		vector<cRobot*>::iterator robotIterator = theRobots.begin();
		while (robotIterator != theRobots.end())
		{
			if ((*robotIterator)->isActive() == false)
			{
				robotIterator = theRobots.erase(robotIterator);
			}
			else  
			{
				(*robotIterator)->update(deltaTime);
				// If robot hits bottom of the screen, ends the game and changes game state
				if ((*robotIterator)->getSpritePos().y >= 700)
				{
					theSoundMgr->getSnd("gameOver")->play(0);
					theGameState = gameState::end;
					gameOver = true;
					theAreaClicked = { 0,0 };
				}
				++robotIterator;
			}
		}

		// Update the visibility and position of each bullet
		vector<cBullet*>::iterator bulletIterartor = theBullets.begin();
		while (bulletIterartor != theBullets.end())
		{
			if ((*bulletIterartor)->isActive() == false)
			{
				bulletIterartor = theBullets.erase(bulletIterartor);
			}
			else
			{
				(*bulletIterartor)->update(deltaTime);
				++bulletIterartor;
			}
		}

		// Update the visibility and position of each explosion
		vector<cSprite*>::iterator expIterartor = theExplosions.begin();
		while (expIterartor != theExplosions.end())
		{
			if ((*expIterartor)->isActive() == false)
			{
				expIterartor = theExplosions.erase(expIterartor);
			}
			else
			{
				(*expIterartor)->animate(deltaTime);
				++expIterartor;
			}
		}

		/*
		==============================================================
		| Check for collisions
		==============================================================
		*/
		for (vector<cBullet*>::iterator bulletIterartor = theBullets.begin(); bulletIterartor != theBullets.end(); ++bulletIterartor)
		{
			//(*bulletIterartor)->update(deltaTime);
			for (vector<cRobot*>::iterator robotIterator = theRobots.begin(); robotIterator != theRobots.end(); ++robotIterator)
			{

				if ((*robotIterator)->collidedWith(&(*robotIterator)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
				{
					// if a collision set the bullet and robot to false
					(*robotIterator)->setActive(false);
					(*bulletIterartor)->setActive(false);
					theExplosions.push_back(new cSprite);
					int index = theExplosions.size()-1;
					theExplosions[index]->setSpriteTranslation({ 0, 0 });
					theExplosions[index]->setActive(true);
					theExplosions[index]->setNoFrames(16);
					theExplosions[index]->setTexture(theTextureMgr->getTexture("explosion"));
					theExplosions[index]->setSpriteDimensions(theTextureMgr->getTexture("explosion")->getTWidth()/ theExplosions[index]->getNoFrames(), theTextureMgr->getTexture("explosion")->getTHeight());
					theExplosions[index]->setSpritePos({ (*robotIterator)->getSpritePos().x + (int)((*robotIterator)->getSpritePos().w/2), (*robotIterator)->getSpritePos().y + (int)((*robotIterator)->getSpritePos().h / 2) });

					// If a collision, play explosion sound and update score
					theSoundMgr->getSnd("explosion")->play(0);
					theScore += 1;
					strScore = gameTextList[2];
					strScore += to_string(theScore).c_str();
					updateScore = true;
				}
			}
		}
	
		// Update the Turets position
		theTurret.update(deltaTime);

		// Respawn enemies after 100 frames
		if (frames % 100 == 0)
		{
			int currentSize = theRobots.size();
			int end = currentSize + 5;
			int count = 1;
			for (int astro = currentSize; astro < end; astro++)
			{
				theRobots.push_back(new cRobot);
				theRobots[astro]->setSpritePos({ (80 * (count) + (60 * count)), -50 }); // Enemies randomly spawn on x-axis, and always on 0 on the y-axis
				theRobots[astro]->setSpriteTranslation({ 0, -120 }); // Moves enemies down
				int randRobot = RobotTextDis(gen);
				theRobots[astro]->setTexture(theTextureMgr->getTexture(textureName[randRobot]));
				theRobots[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randRobot])->getTWidth(), theTextureMgr->getTexture(textureName[randRobot])->getTHeight());
				theRobots[astro]->setRobotVelocity(200);
				theRobots[astro]->setActive(true);
				count++;
			}
		}

		frames++;
	
 		if (gameOver)
		{
			theGameState = gameState::end;
			updateScore = true;
		}

	}
}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					theAreaClicked = { event.button.x, event.button.y };
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;
				case SDLK_DOWN:
				{
					
				}
				break;

				case SDLK_UP:
				{
					
				}
				break;
				case SDLK_RIGHT:
				{
					theTurret.setTurretMove(-3); // Allows player to move right
				}
				break;

				case SDLK_LEFT:
				{
					theTurret.setTurretMove(3); // Allows player to move left
				}
				break;
				case SDLK_SPACE:
				{
					// bullets can only be shot during Playing game state
					if (theGameState == gameState::playing)
					{
						theBullets.push_back(new cBullet);
						int numBullets = theBullets.size() - 1;
						theBullets[numBullets]->setSpritePos({ theTurret.getBoundingRect().x + theTurret.getSpriteCentre().x, theTurret.getBoundingRect().y + theTurret.getSpriteCentre().y });
						theBullets[numBullets]->setSpriteTranslation({ 50, 50 });
						theBullets[numBullets]->setTexture(theTextureMgr->getTexture("bullet_clipart"));
						theBullets[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("bullet_clipart")->getTWidth(), theTextureMgr->getTexture("bullet_clipart")->getTHeight());
						theBullets[numBullets]->setBulletVelocity(10); // Changed bullet speed so that it is slightly slower
						theBullets[numBullets]->setSpriteRotAngle(theTurret.getSpriteRotAngle());
						theBullets[numBullets]->setActive(true);
						cout << "Bullet added to Vector at position - x: " << theTurret.getBoundingRect().x << " y: " << theTurret.getBoundingRect().y << endl;
						theSoundMgr->getSnd("laserShot")->play(0);
					}
				}
				break;
				default:
					break;
				}
			default:
				break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	//Quit FONT system
	TTF_Quit();

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

