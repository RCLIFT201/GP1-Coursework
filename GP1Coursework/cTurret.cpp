/*
=================
cTurret.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cTurret.h"

/*
=================================================================
Defualt Constructor
=================================================================
*/
cTurret::cTurret() : cSprite()
{
	this->turretVelocity = 0;
}
/*
=================================================================
Update the sprite position
=================================================================
*/

void cTurret::update(double deltaTime)
{
	auto rads = PI / 90.0f * (this->getSpriteRotAngle() - 90.0f); // Changed 180.0f to 90.0f to allow player to move only left or right

	FPoint direction = { 0.0f, 0.0f };
	direction.X = (float)(cos(rads));
	direction.Y = (float)(sin(rads));

	SDL_Rect currentSpritePos = this->getSpritePos();
	
	currentSpritePos.x += (int)(this->turretVelocity * direction.X * this->move * deltaTime);
	currentSpritePos.y -= (int)(this->turretVelocity * direction.Y * this->move * deltaTime);

	this->setSpritePos({ currentSpritePos.x , currentSpritePos.y  });
	this->setBoundingRect(this->getSpritePos());

}
/*
=================================================================
Sets the velocity for the turret
=================================================================
*/
void cTurret::setTurretVelocity(int turretVel)
{
	turretVelocity = turretVel;
}
/*
=================================================================
Gets the turret velocity
=================================================================
*/
int cTurret::getTurretVelocity()
{
	return turretVelocity;
}
/*
=================================================================
Sets the move value for the turret
=================================================================
*/
void cTurret::setTurretMove(int turretMove)
{
	move = turretMove;
}
/*
=================================================================
Gets the turret move value
=================================================================
*/
int cTurret::getTurretMove()
{
	return move;
}