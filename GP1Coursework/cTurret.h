/*
=================
cTurret.h
- Header file for class definition - SPECIFICATION
- Header file for the Turret class which is a child of cSprite class
=================
*/
#ifndef _CTURRET_H
#define _CTURRET_H
#include "cSprite.h"

class cTurret : public cSprite
{
private:
	int turretVelocity;
	int move;

public:
	cTurret();
	void update(double deltaTime);		// Turret update method
	void setTurretVelocity(int turretVel);   // Sets the velocity for the turret
	int getTurretVelocity();				 // Gets the turret velocity
	void setTurretMove(int turretMove);   // Sets the turret move Value
	int getTurretMove();				 // Gets the turret move value
}; 
#endif