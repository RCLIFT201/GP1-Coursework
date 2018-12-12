/*
=================
cRobot.h
- Header file for class definition - SPECIFICATION
- Header file for the Turret class which is a child of cSprite class
=================
*/
#ifndef _CROBOT_H
#define _CROBOT_H
#include "cSprite.h"

class cRobot : public cSprite
{
private:
	int robotVelocity;

public:
	cRobot();
	void update(double deltaTime);		// Turret update method
	void setRobotVelocity(int robotVel);   // Sets the velocity for the robot
	int getRobotVelocity();				 // Gets the robot velocity
};
#endif