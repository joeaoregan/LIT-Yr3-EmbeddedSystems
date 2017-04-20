/*
	Joe O'Regan 
	K00203642

  Embedded Systems Assignment
	
	Enemy.h
	
	This file is used to create Enemy objects
*/

#include "GameObject.h"

#define ENEMY_VELOCITY 2

#ifndef ENEMY_H
#define ENEMY_H

class Enemy : public GameObject {
  public:  
  // Constructor
  Enemy() {
    width = 20;   // Circle of radius 20
    height = 20;  // Circle of radius 20
  }
  
  // Move the Enemy
  virtual void movement(){
  	x -= ENEMY_VELOCITY;    // Set the movement for the Enemy
  	
  	if (x < 0) {            // If the Enemy moves off screen to the left
  		x = 240;              // Restart the Enemy moving from the right
  		y = random(30, 180);  // At a random Y position
	  }
  }

  virtual void reset(){    
    x = 245;                // Set the enemy X coordinate
    y = random(40,180);     // Set the enemy Y coordinate
  }
  /*
  // Draw the Enemy
  virtual void draw(){
  //	clear(prevX, prevY);		// Clear the Enemies previous position
  	prevX = x;					// Store the coordinates to be cleared next time
  	prevY = y;
  	
  	// Draw circle with outline to represent the Enemy
   // tft.drawCircle(x, y, 20, ILI9341_BLACK);
   // tft.fillCircle(x, y, 19, ILI9341_GREEN);
  }
  
 // virtual void clear(int x, int y){  	
 // 	tft.fillCircle(x, y, 20, ILI9341_BLUE);	// Clear the Enemies previous position by drawing a circle matching the background colour
 // }
 */
};

#endif
