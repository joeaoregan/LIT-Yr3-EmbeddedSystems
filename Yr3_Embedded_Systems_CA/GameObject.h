/*
	Joe O'Regan 
	K00203642
	
  Embedded Systems Assignment
  
	GameObject.h
	
	This file is used to create Game Objects
*/

#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

class GameObject {
  public:  
    //byte x, y;    // using bytes gave an error message when compiling, probably need to be casted to int for some calculations
    int x;			    // X coordinate
    int y;			    // Y coordinate
    int prevX = x;	// Coordinates used to clear the previous GameObject instance
    int prevY = y;
    
    int width;      // Dimensions
    int height;
  
    int lives;                        // Number of lives
    
    bool active = false;						  // Is the Game Object active on screen
    
    virtual void movement() { };			// Move the Game Object
   // virtual void clear() { };				// Clear the Game Objects previous position
    virtual void draw() { 
	  	prevX = x;							        // Store the coordinates to be cleared next time
	  	prevY = y;
	};
	    
    void spawn(int posX, int posY){
    	if(!active){
    		x = posX;							// Set the x coordinate
    		y = posY;							// Set the y coordinate
    	
    		active = true;				// Set the Game Object as active on screen
    	}
	}

  virtual void reset() {};       // Reset the object
};

#endif
