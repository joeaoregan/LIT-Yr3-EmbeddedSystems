/*
	Joe O'Regan 
	K00203642
	
  Embedded Systems Assignment
  
	Laser.h
	
	This file is used to create Laser objects
*/

#include "GameObject.h"

#define LASER_VELOCITY 10

#ifndef LASER_H
#define LASER_H

class Laser : public GameObject {
  public:  
  // Constructor
  Laser(){
  	width = 10;               // Set the Laser width
  	height = 4;               // Set the Laser height
  }
  
  // Move the Laser
  virtual void movement(){
  	if (active){              // If the Laser is active
	  	x += LASER_VELOCITY;		// Set the position of the laser
	  	
	  	if (x > 245) {				  // If the laser moves off screen
	  		active = false;       // Set it as not active, allowing the Player to fire again
		  }
	  }
  }  
};

#endif
