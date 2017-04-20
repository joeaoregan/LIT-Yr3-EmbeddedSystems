/*
	Joe O'Regan 
	K00203642
	
  Embedded Systems Assignment
  
	Player.h
	
	This file is used to create Player objects
*/

#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"

#define PLAYER_VELOCITY 5 // Player movment handled in main file
#define MAX_NUM_LIVES 3   // Number of lives for the Player

//enum movementState{ MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN, STOP}; // set movement state for player (not needed)

class Player : public GameObject {
  public:  
  // Constructor 
  Player(){
    width = 34;                   // Dimensions not needed unless checking collisions
    height = 24;
    lives = MAX_NUM_LIVES;        // Number of lives
  }
  // Move the Player
  virtual void movement(){
    /* X Axis Bounds Checks: Move the player from one side of the screen to the other */
    if (x > 245) x = 0;           // If the Player moves off screen to the right, restart from the left
    else if (x < -width) x = 245; // If the Player moves off screen to the left, restart from right

    /* Bounds checks: Prevent the Player sprite from overwriting the Score, and Control Buttons */
    if (y <= 10) y = 10;          // If the Player reaches the top of the screen, stop it moving further
    else if (y >= 185) y = 185;   // If the Player reaches the bottom of the screen, stop it moving further
  }

  virtual void reset(){
    x = 50;
    y = 50;
    lives = MAX_NUM_LIVES;
  }
};

#endif
