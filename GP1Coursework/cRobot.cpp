/*
=================
cRobot.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cRobot.h"

/*
=================================================================
Defualt Constructor
=================================================================
*/
cRobot::cRobot() : cSprite()
{
	this->robotVelocity = 0;
}
/*
=================================================================
Update the sprite position
=================================================================
*/

void cRobot::update(double deltaTime)
{

	SDL_Rect currentSpritePos = this->getSpritePos();
	currentSpritePos.x += (int)(this->getSpriteTranslation().x * deltaTime);
	currentSpritePos.y -= (int)(this->getSpriteTranslation().y * deltaTime);

	this->setSpritePos({ currentSpritePos.x, currentSpritePos.y });
	this->setBoundingRect(this->getSpritePos());
}
/*
=================================================================
  Sets the velocity for the Robot
=================================================================
*/
void cRobot::setRobotVelocity(int RobotVel)
{
	robotVelocity = RobotVel;
}
/*
=================================================================
  Gets the Robot velocity
=================================================================
*/
int cRobot::getRobotVelocity()
{
	return robotVelocity;
}
