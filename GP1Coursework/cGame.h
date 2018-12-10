#ifndef _GAME_H
#define _GAME_H

/*
==================================================================================
cGame.h
==================================================================================
*/

#include <SDL.h>
#include <random>
// Game specific includes
#include "asteroidsGame.h"


using namespace std;

class cGame
{
public:
	cGame();

	void initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void cleanUp(SDL_Window* theSDLWND);
	void render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre);
	void update();
	void update(double deltaTime);
	bool getInput(bool theLoop);
	double getElapsedSeconds();

	static cGame* getInstance();

private:

	static cGame* pInstance;
	// for framerates
	time_point< high_resolution_clock > m_lastTime;
	time_point< high_resolution_clock > m_CurrentTime;
	duration< double > deltaTime;
	bool loop;

	/* Let the computer pick a random number */
	random_device rd;    // non-deterministic engine 
	mt19937 gen{ rd() }; // deterministic engine. For most common uses, std::mersenne_twister_engine, fast and high-quality.
	uniform_int_distribution<> AsteroidDis{ 0, 10 };
	uniform_int_distribution<> AsteroidTextDis{ 0, 0 };

	// Sprites for displaying background and rocket textures
	cSprite spriteBkgd;
	cRocket theRocket;
	cAsteroid theAsteroid;
	cBullet theBullet;
	// game related variables
	vector<LPCSTR> textureName;
	vector<LPCSTR> textName;
	vector<LPCSTR> texturesToUse;
	vector<LPCSTR> btnNameList;
	vector<LPCSTR> btnTexturesToUse;
	vector<SDL_Point> btnPos;
	vector<cButton> theButtons;
	vector<cAsteroid*> theAsteroids;
	vector<cBullet*> theBullets;
	vector<cSprite*> theExplosions;
	// Fonts to use
	vector<LPCSTR> fontList;
	vector<LPCSTR> fontsToUse;
	// Text for Game
	vector<LPCSTR> gameTextNames;
	vector<LPCSTR> gameTextList;
	vector<LPCSTR> highScoreTextures;
	// Game Sounds
	vector<LPCSTR> soundList;
	vector<soundType> soundTypes;
	vector<LPCSTR> soundsToUse;
	// Game objects
	// Define the elements and there position in/on the array/map
	int renderWidth, renderHeight;
	int theScore;
	string strScore;
	FPoint scale;
	btnTypes theBtnType;
	gameState theGameState;
	SDL_Point theAreaClicked;
	cTexture* tempTextTexture;
	SDL_Rect pos;
	bool gameOver = false;
	string theHighScoreTable;
	int theHSTableSize;
	cHighScoreTable theHSTable;
	int numTableItems;
	int robotsDestroyed;
	int frames = 1;
	bool updateScore = false;
};

#endif
