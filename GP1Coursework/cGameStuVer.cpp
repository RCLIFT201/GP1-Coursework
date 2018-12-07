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
	
	/* Let the computer pick a random number */
	random_device rd;    // non-deterministic engine 
	mt19937 gen{ rd() }; // deterministic engine. For most common uses, std::mersenne_twister_engine, fast and high-quality.
	uniform_int_distribution<> AsteroidDis{ 0, 10 };
	uniform_int_distribution<> AsteroidTextDis{ 0, 0 };

	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();
	theScore = 0;

	theAreaClicked = { 0, 0 };
	// Store the textures
	textureName = { "Enemy", "bullet_clipart", "explosion", "Tank", "MainMenuBackground", };
	texturesToUse = { "Images\\Sprites\\Enemy.png", "Images\\Sprites\\Bullet_ClipArt.png", "Images\\Sprites\\explosion.png", "Images\\Sprites\\Tank.png", "Images\\Bkg\\MainMenuBackground.png", };
	for (int tCount = 0; tCount < (int)textureName.size(); tCount++)
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}

	// Store the textures
	btnNameList = { "quit_button", "instructions_btn", "load_btn", "menu_btn", "play_button", "save_btn", "settings_btn", "hs_button" };
	btnTexturesToUse = { "Buttons/quit_button.png", "Buttons/button_instructions.png", "Buttons/button_load.png", "Buttons/button_menu.png", "Buttons/play_button.png", "Buttons/button_save.png", "Buttons/button_settings.png", "Buttons/hs_button.png" };
	btnPos = { { 400, 375 },{ 400, 300 },{ 400, 300 },{ 500, 500 },{ 400, 300 },{ 740, 500 },{ 400, 300 },{ 400, 500 } };
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
	theTextureMgr->addTexture("theScore", theFontMgr->getFont("retro")->createTextTexture(theRenderer, gameTextList[1], textType::solid, { 255, 0, 0, 255 }, { 0, 0, 0, 0 }));

	// Create text Textures
	gameTextNames = { "TitleTxt", "ObjectiveTxt", "InstructTxt", "ThanksTxt", "SeeYouTxt","BottleCount","HSTable","HScore" };
	gameTextList = { "Robot Defence", "Kill all the invading robots!", "Use the arrow keys to move left or right.", "Thanks for playing!", "See you again soon!", "Score: ", "", "High Score" };
	for (unsigned int text = 0; text < gameTextNames.size(); text++)
	{
		if (text == 0 || text == gameTextNames.size() - 1)
		{
			theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("retro")->createTextTexture(theRenderer, gameTextList[text], textType::solid, { 44, 203, 112, 255 }, { 0, 0, 0, 0 }));
		}
		else
		{
			theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("retro")->createTextTexture(theRenderer, gameTextList[text], textType::solid, { 44, 203, 112, 255 }, { 0, 0, 0, 0 }));
		}
	}

	// Load game sounds
	soundList = { "gameTheme", "menuMusic", "laserShot", "explosion" };
	soundTypes = { soundType::music, soundType::music, soundType::sfx, soundType::sfx };
	soundsToUse = { "Audio/InGameTheme.wav", "Audio/menuMusic.wav", "Audio/laserShot.wav", "Audio/explosion2.aiff" };
	for (int sounds = 0; sounds < (int)soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("gameTheme")->play(-1);

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("MainMenuBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("MainMenuBackground")->getTWidth(), theTextureMgr->getTexture("MainMenuBackground")->getTHeight());

	theRocket.setSpritePos({ 500, 640 });
	theRocket.setTexture(theTextureMgr->getTexture("Tank"));
	theRocket.setSpriteDimensions(theTextureMgr->getTexture("Tank")->getTWidth(), theTextureMgr->getTexture("Tank")->getTHeight());
	theRocket.setRocketVelocity(75);
	theRocket.setSpriteTranslation({ 50,50 });


	 // Create vector array of textures
	for (int astro = 0; astro < 10; astro++)
	{
		theAsteroids.push_back(new cAsteroid);
		theAsteroids[astro]->setSpritePos({ 90 * AsteroidDis(gen), 0 }); // Enemies randomly spawn on x-axis, and always on 0 on the y-axis
		theAsteroids[astro]->setSpriteTranslation({ 0, -60 }); // Moves enemies down
		int randAsteroid = AsteroidTextDis(gen);
		theAsteroids[astro]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
		theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
		theAsteroids[astro]->setAsteroidVelocity(10);
		theAsteroids[astro]->setActive(true);
	}

	robotsDestroyed = 0;
	strScore = gameTextList[5];
	strScore += to_string(robotsDestroyed).c_str();

	numTableItems = 0;
	theHSTable.loadFromFile("Data/HighScore.dat");

	theHighScoreTable = gameTextList[6];

	theHSTableSize = theHSTable.getTableSize();
	highScoreTextures = { "score1","score2","score3","score4","score5","score6","score7","score8","score9","score10" };

	for (int item = 0; item < theHSTableSize; item++)
	{
		string entry = "";
		entry += theHSTable.getItem(item).Name + " " + to_string(theHSTable.getItem(item).score);
		theTextureMgr->addTexture(highScoreTextures[item], theFontMgr->getFont("retro")->createTextTexture(theRenderer, entry.c_str(), textType::solid, { 44, 203, 112, 255 }, { 0, 0, 0, 0 }));
	}

	gameOver = false;
}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	bool loop = true;

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
	SDL_RenderClear(theRenderer);
	spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

	// Render each asteroid in the vector array
	for (int draw = 0; draw < (int)theAsteroids.size(); draw++)
	{
		theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
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

	// Render updated score value
	
	// Lab 7 code goes here
	tempTextTexture = theTextureMgr->getTexture("theScore");
	pos = { 700, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	scale = { 1, 1 };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	
	// render the rocket
	theRocket.render(theRenderer, &theRocket.getSpriteDimensions(), &theRocket.getSpritePos(), theRocket.getSpriteRotAngle(), &theRocket.getSpriteCentre(), theRocket.getSpriteScale());
	SDL_RenderPresent(theRenderer);

	switch (theGameState)
	{
	case gameState::menu:
	{
		spriteBkgd.setTexture(theTextureMgr->getTexture("MainMenuBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("MainMenuBackground")->getTWidth(), theTextureMgr->getTexture("MainMenuBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		// Render Button
		theButtonMgr->getBtn("play_button")->setSpritePos({ 400, 375 });
		theButtonMgr->getBtn("play_button")->render(theRenderer, &theButtonMgr->getBtn("play_button")->getSpriteDimensions(), &theButtonMgr->getBtn("play_button")->getSpritePos(), theButtonMgr->getBtn("play_button")->getSpriteScale());
		theButtonMgr->getBtn("hs_button")->setSpritePos({ 400, 425 });
		theButtonMgr->getBtn("hs_button")->render(theRenderer, &theButtonMgr->getBtn("hs_button")->getSpriteDimensions(), &theButtonMgr->getBtn("hs_button")->getSpritePos(), theButtonMgr->getBtn("hs_button")->getSpriteScale());
		theButtonMgr->getBtn("quit_button")->setSpritePos({ 400, 475 });
		theButtonMgr->getBtn("quit_button")->render(theRenderer, &theButtonMgr->getBtn("quit_button")->getSpriteDimensions(), &theButtonMgr->getBtn("quit_button")->getSpritePos(), theButtonMgr->getBtn("quit_button")->getSpriteScale());
	}
	break;
	case gameState::playing:
	{
		// Render each asteroid in the vector array
		for (int draw = 0; draw < (int)theAsteroids.size(); draw++)
		{
			theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
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

		// Render updated score value

		// Lab 7 code goes here
		tempTextTexture = theTextureMgr->getTexture("theScore");
		pos = { 700, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		// render the rocket
		theRocket.render(theRenderer, &theRocket.getSpriteDimensions(), &theRocket.getSpritePos(), theRocket.getSpriteRotAngle(), &theRocket.getSpriteCentre(), theRocket.getSpriteScale());
		SDL_RenderPresent(theRenderer);
	}
	case gameState::end:
	{
		spriteBkgd.setTexture(theTextureMgr->getTexture("GameOverBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("GameOverBackground")->getTWidth(), theTextureMgr->getTexture("GameOverBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		cTexture* tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("ThanksTxt");
		pos = { 50, 100, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("SeeYouTxt");
		pos = { 50, 175, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 500, 500 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("quit_button")->setSpritePos({ 500, 575 });
		theButtonMgr->getBtn("quit_button")->render(theRenderer, &theButtonMgr->getBtn("quit_button")->getSpriteDimensions(), &theButtonMgr->getBtn("quit_button")->getSpritePos(), theButtonMgr->getBtn("quit_button")->getSpriteScale());
	}
	break;
	case gameState::highscore:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("HScore");
		pos = { 220, 125, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		pos = { 220, 200, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };

		for (int item = 0; item < theHSTableSize; item++)
		{
			tempTextTexture = theTextureMgr->getTexture(highScoreTextures[item]);
			tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
			pos = { 220, 200 + (50 * item), tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		}

		// Render Button
		theButtonMgr->getBtn("play_btn")->setSpritePos({ 900, 375 });
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 900, 425 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 900, 475 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case gameState::quit:
	{
		loop = false;
	}
	break;
	default:
		break;
	}

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
	IPoint rocketPos = { (int)theRocket.getSpritePos().x, (int)theRocket.getSpritePos().y };
	if (rocketPos.X >= 910 || rocketPos.X <= 0)
	{
		theRocket.setSpritePos({ theRocket.getSpritePos().x, theRocket.getSpritePos().y });
		theRocket.setRocketMove(theRocket.getRocketMove()*(-1));
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
	if (theGameState == gameState::highscore)
	{
		spriteBkgd.setTexture(theTextureMgr->getTexture("MainMenuBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("MainMenuBackground")->getTWidth(), theTextureMgr->getTexture("MainMenuBackground")->getTHeight());
		theGameState = theButtonMgr->getBtn("play_button")->update(theGameState, gameState::playing, theAreaClicked);
		theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, gameState::menu, theAreaClicked);
	}

	if (theGameState == gameState::menu)
	{
		theGameState = theButtonMgr->getBtn("play_button")->update(theGameState, gameState::playing, theAreaClicked);
		gameOver = false;
		if (theGameState == gameState::playing && gameOver == false)
		{
			//theTileMap.update(theShip.getMapPosition(), 1, 0.0f);
			//theTileMap.update(theBottle.getMapPosition(), 1, 0.0f);
			//theTileMap.update(thePirate.getMapPosition(), 1, 0.0f);
			//theShip.setMapPosition(spriteRandom(gen), spriteRandom(gen));
			//theBottle.genRandomPos(theShip.getMapPosition().R, theShip.getMapPosition().C);
			//// Lab Code goes here
			//theTileMap.update(theShip.getMapPosition(), 3, theShip.getShipRotation());
			//theTileMap.update(theBottle.getMapPosition(), 2, theBottle.getBottleRotation());
			//theTileMap.update(thePirate.getMapPosition(), 4, thePirate.getEnemyRotation());
			////// Lab Code goes here
			//robotsDestroyed = 0;
		}
	}

	theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, gameState::menu, theAreaClicked);
	theGameState = theButtonMgr->getBtn("hs_button")->update(theGameState, gameState::highscore, theAreaClicked);

	if (theGameState == gameState::playing)
	{

		// Check if ship has collided with the bottle
		IPoint asteroidPos = { (int)theAsteroid.getSpritePos().x, (int)theAsteroid.getSpritePos().y };
		if (asteroidPos.Y = 400)
		{
			theSoundMgr->getSnd("explosion")->play(1);
			theGameState = gameState::end;
		}

		//bottlesCollected++;
		//theBottle.genRandomPos(theShip.getMapPosition().R, theShip.getMapPosition().C);
		//theTileMap.update(theBottle.getMapPosition(), 2, theBottle.getBottleRotation());
		////Create Updated bottle count
		//strScore = gameTextList[5];
		//strScore += to_string(bottlesCollected).c_str();
		//theTextureMgr->deleteTexture("BottleCount");

		 // Check if Pirate has collided with the ship
		 // Lab Code goes here
		 //if (theShip.getMapPosition() == thePirate.getMapPosition())
		 //{
			// theSoundMgr->getSnd("explosion")->play(1);
			// theGameState = gameState::end;
		 //}

		 if (gameOver)
		 {
			 theGameState = gameState::end;
		 }
	}

	// Update the visibility and position of each asteriod
	vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
	while (asteroidIterator != theAsteroids.end())
	{
		if ((*asteroidIterator)->isActive() == false)
		{
			asteroidIterator = theAsteroids.erase(asteroidIterator);
		}
		else  
		{
			(*asteroidIterator)->update(deltaTime);
			++asteroidIterator;
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
		for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
		{
			if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
			{
				// if a collision set the bullet and asteroid to false
				(*asteroidIterator)->setActive(false);
				(*bulletIterartor)->setActive(false);
				theExplosions.push_back(new cSprite);
				int index = theExplosions.size()-1;
				theExplosions[index]->setSpriteTranslation({ 0, 0 });
				theExplosions[index]->setActive(true);
				theExplosions[index]->setNoFrames(16);
				theExplosions[index]->setTexture(theTextureMgr->getTexture("explosion"));
				theExplosions[index]->setSpriteDimensions(theTextureMgr->getTexture("explosion")->getTWidth()/ theExplosions[index]->getNoFrames(), theTextureMgr->getTexture("explosion")->getTHeight());
				theExplosions[index]->setSpritePos({ (*asteroidIterator)->getSpritePos().x + (int)((*asteroidIterator)->getSpritePos().w/2), (*asteroidIterator)->getSpritePos().y + (int)((*asteroidIterator)->getSpritePos().h / 2) });

				theSoundMgr->getSnd("explosion")->play(0);

				strScore = gameTextList[5];
				strScore += to_string(robotsDestroyed).c_str();
				
				// Lab 7 code goes here
				
			}
		}
	}

	// Update the Rockets position
	theRocket.update(deltaTime);
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
					theRocket.setRocketMove(-3); // Allows player to move right
				}
				break;

				case SDLK_LEFT:
				{
					theRocket.setRocketMove(3); // Allows player to move left
				}
				break;
				case SDLK_SPACE:
				{
					theBullets.push_back(new cBullet);
					int numBullets = theBullets.size() - 1;
					theBullets[numBullets]->setSpritePos({ theRocket.getBoundingRect().x + theRocket.getSpriteCentre().x, theRocket.getBoundingRect().y + theRocket.getSpriteCentre().y });
					theBullets[numBullets]->setSpriteTranslation({ 50, 50 });
					theBullets[numBullets]->setTexture(theTextureMgr->getTexture("bullet_clipart"));
					theBullets[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("bullet_clipart")->getTWidth(), theTextureMgr->getTexture("bullet_clipart")->getTHeight());
					theBullets[numBullets]->setBulletVelocity(15); // Changed bullet speed so that it is slightly slower
					theBullets[numBullets]->setSpriteRotAngle(theRocket.getSpriteRotAngle());
					theBullets[numBullets]->setActive(true);
					cout << "Bullet added to Vector at position - x: " << theRocket.getBoundingRect().x << " y: " << theRocket.getBoundingRect().y << endl;
				}
				theSoundMgr->getSnd("laserShot")->play(0);
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

