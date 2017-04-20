/* 
 *  Joe O'Regan
 *  K00203642
 *  
 *  Embedded Systems Assignment
 *  17/03/2017
 *  
 *  This program demonstrates how the Arduino interacts with the touchscreen shield using the SPI interface.
 *  
 *  Taking one of the original example programs: onoffbutton
 *  (This example implements a simple sliding On/Off button. The example demonstrates drawing and touch operations.
 *  Thanks to Adafruit forums member Asteroid for the original sketch!)
 *   
 *  I was able to implement a simple 2D game, with random Enemy spawning (green circle), 
 *  and a Player object (flappy bird) controlled with touchscreen button movement. The
 *  Player can also fire lasers to destroy the Enemy object.
 *  
 *  The high score is stored in EEPROM. The game also features a countdown timer, and the game play
 *  ends when the counter reaches zero, overwriting the high score if a new high score is achieved. *  
 *  
 */

#include <Adafruit_GFX.h>       // Core graphics library. Handle graphics rendering, for lines, shapes, text, etc.
#include <SPI.h>                // Serial Peripheral Interface, synchonous data protocol
#include <Wire.h>               // Inter-Intergrated (I2C) synchronous serial communciation (Works without this)
#include <Adafruit_ILI9341.h>   // Hardware-specific Library. Controls the TFT screen
#include <Adafruit_STMPE610.h>  // Controls the touchscreen
#include <EEPROM.h>             // Store the high score in the Arduinos Eeprom memory

#include "GameObject.h"         // 2017/04/17 Added GameObject base class
#include "Player.h"             // 2017/04/17 Added Player class
#include "Enemy.h"              // 2017/04/17 Added Enemy class
#include "Laser.h"              // 2017/04/17 Added Laser class

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

/* Setting the Digital pins */
#define STMPE_CS 8                                        // Chip Select (CS) is always 8 for the touchscreen shield
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);       // Create a touchscreen object, with hardware SPI, so clock, mosi, and miso pins are not defined
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);  // Create a TFT object

/* Button positions on screen */
#define BTN_X_1 0               // Button 1st x position (left)
#define BTN_X_2 60              // Button 2nd x position (up, down)
#define BTN_X_3 120             // Button 3rd x position (right)
#define BTN_X_4 180             // Button 4th x position (fire)

#define BTN_R1 220              // Button 1st row y position
#define BTN_R2 270              // Button 2nd row y position
#define BTN_W 60                // Button width
#define BTN_H 50                // Button height

#define FIRE_BUTTON_X 180       // Fire button x coordinate
#define FIRE_BUTTON_Y BTN_R2    // Fire button y coordinate
#define FIRE_BUTTON_W BTN_W     // Fire button width
#define FIRE_BUTTON_H BTN_H     // Fire button height

#define PAUSE_BUTTON_X 140      // Pause button x coordinate
#define PAUSE_BUTTON_Y BTN_R1   // Pause button y coordinate
#define PAUSE_BUTTON_W 90       // Pause button width
#define PAUSE_BUTTON_H BTN_H    // Pause button height

boolean playGame = true;        // Start / Pause the game
boolean gameFinished = false;   // End the game
boolean menu = true;

/* Create the Game Objects */
GameObject* enemy = new Enemy();    // Create a GameObject of type Enemy
GameObject* player = new Player();  // Create a GameObject of type Player
GameObject* laser = new Laser();    // Create a GameObject of type Laser

int score = 0;                      // Score for the Player
int prevScore = -1;                 // Store the previous score, so the score string only updates when there is a change
int highScore;                      // High score stored in EEPROM

#define COUNTDOWN_TIMER 10

unsigned long previousTime = 0;           // The previous time
unsigned long currentTime = 0;            // The current time
unsigned int gameTime = COUNTDOWN_TIMER;  // The time to countdown from

/* Setup the game, including the touchscreen and tft, 
   and spawn the player, enemy, and control buttons */
void setup(void) {
  Serial.begin(9600);                                                 // Baud rate / Data rate in bits per second
  tft.begin();                                                        // Initialise the tft screen
  if (!ts.begin())  Serial.println("Unable to start touchscreen.");   // Serial Monitor Error: Can't initialise the touchscreen, driver not found
  else Serial.println("Touchscreen started.");                        // Touch screen is started and ready to use

  tft.setRotation(0);                                                 // origin = left,top landscape (USB left upper) // Set the screen rotation

  highScore = EEPROM.read(0);                                         // Read the High Score value from EEPROM

  displayMenu();                                                      // Draw the initial game menu
  
  //resetGame();                                                        // Initialise the game level
}

/* The main loop for the game */
void loop() {
  // See if there's any  touch data for us
  if (!ts.bufferEmpty()) {                                            // Retrieve a point. The touchscreen driver stores touchpoints every loop
    TS_Point p = ts.getPoint();                                       // If there is data in the buffer, get the oldest or most recent point from the buffer
    // Scale using the calibration #'s and rotate coordinate system
    // Horizontal screen orientation mapping
    //p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());              // Map touch coordinates for horizontal screen orientation
    //p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
    // Vertical screen orientation mapping
    p.x = map(p.x, TS_MINX, 4000, 0, tft.width());                    // Map touch coordinates for vertical screen orientation, this needed to be adjusted as the touch range was smaller after changing orientation from horizontal to vertical
    p.y = map(p.y, TS_MINY, 4200, 0, tft.height());

    /* Previous Axis alignement change */
    //int y = tft.height() - p.x;                                     // Convert coordinates for horizontal screen orientation
    //int x = p.y;
    /* Axis reset for vertical screen orientation */
    int y = p.y;                                                      // Vertical rotation  is default, and doesn't need to be rotated, so can set x here, or change x to p.x further down the code
    int x = p.x;   
    
    //Serial.println(playGame); 
    Serial.print("x: ");                                              // Output the X touch coordinate to the serial monitor 
    Serial.print(x);                                                  // Output the X touch coordinate to the serial monitor 
    Serial.print(" y: ");                                             // Output the X touch coordinate to the serial monitor 
    Serial.println(y);                                                // Output the Y touch coordinate to the serial monitor

    if (menu){
      //displayMenu();                                                  // Draw the initial game menu
      startPressed(x, y);                                             // Check if the start button was pressed
    }
    else {
      pausePressed(x, y);                                             // Check if the pause button was pressed
      firePressed(x, y);                                              // Check if the fire button was pressed
      if (gameFinished) restartPressed(x, y);                         // Check if the restart button was pressed
      if (playGame) playerMovement(x, y);                             // If the game is playing, check the player movement
    }
  }  

  if (!gameFinished && playGame && !menu) {                           // If the game isn't finished OR paused, and isn't in the menu
    movement();                                                       // Update the game movment
    draw();                                                           // Render the game objects to the screen
    checkCollision();                                                 // Check collisions between game objects (only tested collisions for laser and enemy)
    timer();                                                          // 2017/04/19 Added Timer
  }
}

/* Check for collisions between game Objects
   Only implemented check between Laser and Enemy 
*/
void checkCollision(){
  //if (laser->x >= enemy->x - 15 && laser->x <= enemy->x +15 && laser->y >= enemy->y + 15 && laser->y >= enemy->y - 20) clearEnemy(enemy->x, enemy->y);  // Testing different collisions
  //enemy->spawn(245, 100); // Respawn if killed
  //enemy->active = false;  // Set active false (should probably go before spawn, now that I think about it, that partly explains why this didn't work)

  //if (laser->x > enemy->x) enemy->x = 245;
  bool collisionLaserEnemy = false;                               // If there is a collision this value will be set as true, otherwise it remains false
  bool collisionPlayerEnemy = false;
  
  //if (laser->y + laser->height <= enemy->y || laser->y >= enemy->y + enemy->height || laser->x + laser->width <= enemy->x || laser->x >= enemy->x + enemy->width) // should be AND not OR
  //if (laser->y + laser->height <= enemy->y && laser->x + laser->width <= enemy->x && laser->y >= enemy->y + enemy->height && laser->x >= enemy->x + enemy->width) 
  
  if (laser->active){                                           // If the laser is active
    if (laser->y + laser->height <= enemy->y - enemy->height && // and the bottom of the laser is not overlapping the top of the Enemy (accounting for radius),
        laser->y >= enemy->y + enemy->height &&                 // and the top of the laser is not overlapping the bottom of the Enemy, 
        laser->x + laser->width <= enemy->x &&                  // and the right side of the laser is not overlapping the left side of the Enemy,
        laser->x >= enemy->x + enemy->width)                    // and the left side of the laser is not overlapping the right side of the Enemy
      collisionLaserEnemy = false;                              // Then the Laser is not colliding with the Enemy
    else {
      collisionLaserEnemy = true;                               // Otherwise it is colliding
      laser->active = false;                                    // Set the laser as not active
    }
  }

  //tft.drawRect(player->x, player->y, player->width, player->height, ILI9341_WHITE); // Player box collider
  //tft.drawRect(enemy->x - 20, enemy->y - 20, enemy->width + 20, enemy->height + 20, ILI9341_WHITE); // Enemy box collider
  
  if (player->y + player->height <= enemy->y - enemy->height && // If the bottom of the Player is not overlapping the top of the Enemy (accounting for radius),
      player->y >= enemy->y + enemy->height &&                  // and the top of the Player is not overlapping the bottom of the Enemy, 
      player->x + player->width <= enemy->x &&                  // and the right side of the Player is not overlapping the left side of the Enemy,
      player->x >= enemy->x + enemy->width)                     // and the left side of the Player is not overlapping the right side of the Enemy
    collisionPlayerEnemy = false;                               // Then the Playerer is not colliding with the Enemy
  else {
    collisionPlayerEnemy = true;                                // Otherwise it is colliding
    //player->lives--;                                          // Decrement the number of Player lives
  }
  
  if (collisionLaserEnemy) {                // If there is a collision between Laser and Enemy            
    score += 10;                            // Increment the score by 10
    Serial.println(score);                  // Output the score
    clearLaser(laser->prevX, laser->prevY); // Clear the laser from the screen
    clearEnemy(enemy->prevX, enemy->prevY); // Clear the enemy from the screen
    laser->active = false;                  // Set the laser as not active
    //enemy->spawn(245, random(35, 180));   // XXX - Respawn the enemy
    //enemy->x = 245;                       // Set the enemy X coordinate
    //enemy->y = random(40,180);            // Set the enemy Y coordinate
    enemy->reset();
  }

 // score +=1;                              // Test score is incrementing
  //tft.println(0xDEADBEEF, DEC);           // Test how numbers are displayed
}

/* Register the start button being pressed */
void startPressed(int x, int y){
  if(x > 0 && y > 250) {                    // If a touch happens within the coordinates of the Start button
    Serial.println("Begin pressed");        // Output message to serial monitor indicating the Start button has been pressed

    menu = false;
    resetGame();                                                        // Initialise the game level
  }
}
/* Register the restart button being pressed */
void restartPressed(int x, int y){
  if(x > 0 && y > 250) {                    // If a touch happens within the coordinates of the Restart button
    Serial.println("Restart pressed");      // Output message to serial monitor indicating the Restart button has been pressed

    resetGame();                            // Reset the game back to original state
  }
}

/* Restart the game */
void resetGame(){
  /* Reset Game Variables */
  gameFinished = false;
  gameTime = COUNTDOWN_TIMER;
  score = 0;  
  playGame = true;  
  
  previousTime = 0;
  prevScore = -1;
  
  tft.fillScreen(ILI9341_BLUE); // Set the background colour
  
  // Spawn Game Objects
  //player->spawn(50,50);         // Spawn Player at starting position
  //enemy->spawn(245, 40);        // Spawn Enemy at starting position
  enemy->reset();
  player->reset();
  //player->lives = 3;            // Reset the number of Player lives // Moved to reset
  drawPlayerControls();           // Draw the control buttons
}

/* Register the pause button being pressed */
void pausePressed(int x, int y){
  //if(x > PAUSE_BUTTON_X && x < (PAUSE_BUTTON_X + PAUSE_BUTTON_W) && y > PAUSE_BUTTON_Y && y <= PAUSE_BUTTON_Y + PAUSE_BUTTON_H) { // Check coordinates -> rearranged before realising the touch screen mapping needed to be adjusted
  if(x > 140 && x < 240 && y > 200 && y < 250) {  // If a touch happens within the coordinates of the Pause button
    Serial.println("Pause pressed");              // Output message to serial monitor indicating the Pause button has been pressed

    if (playGame) playGame = false;               // If the game is Playing set to Pause
    else playGame = true;                         // If the game is Paused set to Play    
  }
}

/* Register the fire button being pressed */
void firePressed (int x, int y){
  if((x > FIRE_BUTTON_X) && 
     (x < (FIRE_BUTTON_X + FIRE_BUTTON_W)) && 
     (y > FIRE_BUTTON_Y) && 
     (y <= (FIRE_BUTTON_Y + FIRE_BUTTON_H))) {
    //if (!laser->active && playGame)               // Not needed as check has been moved to the spawn function, and movement function also checks game is playing
    laser->spawn(player->x + 25, player->y + 10);   // Spawn a laser at the players coordinates
      
    Serial.println("Fire pressed");                 // Output message to serial monitor indicating the Fire button has been presseed
  }  
}

/* Draw the game objects */
void draw(){
  drawEnemy();                       // Draw enemy at new position
  drawLaser();                       // Render the laser
  //if (player->prevX != player->x && player->prevY != player->y) // 2017/04/19 Only draw the Player if it's coordinates have changed // Tested - Draws the Player erratically
  drawPlayer(player->x, player->y);  // Draw player at new position
  
  drawScore();
}

/* Draw a countdown clock (This causes a black line to be randomly drawn from the bird down) */
void timer(){
  currentTime = millis();

  if (currentTime - previousTime >= 1000) {
    previousTime = millis();    

    //tft.drawRect(10, 1, 25, 15, ILI9341_BLUE);
    tft.fillRect(10, 1, 25, 15, ILI9341_BLUE);// Clear the text first
    tft.setCursor(10 , 1);                    // Set cursor position to draw text for score
    tft.setTextColor(ILI9341_WHITE);          // Set the text colour as white
    tft.setTextSize(2);                       // Set the font size as 2
    
    if (gameTime > 0) gameTime--;             // Decrement the time to be displayed  
    tft.print(gameTime, DEC);                 // Output the time to screen      
  }
  
  if (gameTime == 0) gameOver();              // 2017/04/19 When time runs out the game is over
}

void displayMenu(){
  tft.fillScreen(ILI9341_BLACK);              // Set the background colour 
  tft.setTextColor(ILI9341_BLUE);             // Set the text colour as blue 
  tft.setCursor(20 ,40);                      // Set cursor position to draw text
  tft.setTextSize(3);                         // Set the font size as 4   
  tft.println("Joe O'Regan");                 // Output "Joe O'Regan" Text
  
  tft.setTextColor(ILI9341_WHITE);            // Set the text colour as white 
  tft.setCursor(25 ,70);                      // Set cursor position to draw text for score
  tft.setTextSize(4);                         // Set the font size as 4   
  tft.println("EMBEDDED");                    // Output "Embedded Systems" Text
  tft.setCursor(35 ,120);                 
  tft.println("SYSTEMS");   
  
  tft.setCursor(30 ,170);                     // Set cursor position to draw text
  tft.setTextSize(3);                         // Set the font size as 3   
  tft.println("ASSIGNMENT");                  // Output "Assignment" Text    
  
  tft.setTextColor(ILI9341_BLUE);             // Set the text colour as blue 
  tft.setCursor(40 ,210);                     // Set cursor position to draw text
  tft.setTextSize(3);                         // Set the font size as 4   
  tft.println("K00203642");                   // Output "Joe O'Regan" Text    

  startBtn();
}
void gameOver(){
  playGame = false;
  
  tft.fillScreen(ILI9341_RED);                // Set the background colour
  tft.setCursor(25 ,70);                      // Set cursor position to draw text for score
  tft.setTextColor(ILI9341_WHITE);            // Set the text colour as white
  tft.setTextSize(8);                         // Set the font size as 2    
  tft.println("GAME");                        // Output "Game Over" message
  tft.setCursor(25 ,135);                 
  tft.println("OVER");                    
  tft.setCursor(21 ,200);                 
  tft.setTextSize(3);                     
  tft.print("Your Score: ");                  // Output the score for the current game
  tft.setCursor(20 ,230);                
  tft.print(score);

  if (score > highScore) {                    // If the score the Player achieves is greater than the high score
    highScore = score;                        // Set the score as the new high score
    EEPROM.write(0, highScore);               // Write the score to the EEPROM
  }
  
  tft.setCursor(20 ,10);                       // Output the current game High Score            
  tft.setTextSize(3);           
  tft.print("High Score: ");
  tft.setCursor(20 ,40);                
  tft.print(highScore);

  restartBtn();
  
  gameFinished = true;                        // Set the game as finished  
}

/* Draw the score for the game (only if the score changes) */
void drawScore(){
  if (score != prevScore) {
    tft.fillRect(65, 1, 150, 15, ILI9341_BLUE);// Clear the text first
    tft.setCursor(65 , 1);                    // Set cursor position to draw text for score
    tft.setTextColor(ILI9341_WHITE);          // Set the text colour as white
    tft.setTextSize(2);                       // Set the font size as 2
    tft.print("Score: ");                     // Output the score text
    //tft.print(score, DEC);                  // Output the score as a number
    tft.println(score);                       // Output the score as a number
    prevScore = score;                        // 2017/04/19 If the score has updated, the previous score must update too
  }
}

/* Handle movement for Game Objects */
void movement(){
  if (playGame){                      // If the game is playing
     enemy->movement();               // Move the Enemy
     laser->movement();               // Move the Laser
  }
}

/* Move the player, depending on which directional button is pressed */
void playerMovement(int x, int y){
      // Handle button presses for movement
      if (x > BTN_X_1 && x < BTN_X_2 && y > BTN_R1 && y < BTN_R2) player->x -= PLAYER_VELOCITY;               // Move player left
      else if (x > BTN_X_2 && x < BTN_X_3 && y > BTN_R1 - BTN_H && y < BTN_R1) player->y -= PLAYER_VELOCITY;  // Move player up
      else if (x > BTN_X_2 && x < BTN_X_3 && y > BTN_R1 && y < BTN_R2) player->y += PLAYER_VELOCITY;          // Move player down
      else if (x > BTN_X_3 && x < BTN_X_4 && y > BTN_R1 && y < BTN_R2) player->x += PLAYER_VELOCITY;          // Move player right

      player->movement();             // Check boundaries etc.
}

/* Draw a rectangle representing the Player's Laser weapon */
void drawLaser() {
  if (laser->active){                             // If the Laser is active on screen
    clearLaser(laser->prevX, laser->prevY);       // Clear the lasers previous position
    laser->draw();                                // Update the lasers previous position
    tft.fillRect(laser->x, laser->y, laser->width, laser->height, ILI9341_GREEN);  // Draw a rectangle width = 10, height = 2
  }
}
/* Draw a circle to represent the Enemy */
void drawEnemy() {
  clearEnemy(enemy->prevX, enemy->prevY);         // Clear the Enemies previous position on the screen

  enemy->draw();                                  // Update previous coordinates
  
  tft.drawCircle(enemy->x, enemy->y, 20, ILI9341_BLACK);  // Draw the outline of the circle in black
  tft.fillCircle(enemy->x, enemy->y, 19, ILI9341_GREEN);  // Draw a solid circle in green
}

/* Draws the player sprite --> reused actual sprite from flappy birds game, needed to set the clear and position update (draw) */
void drawPlayer(int x, int y) {  
  clearPlayer(player->prevX, player->prevY);      // Clear previous position
  player->draw();                                 // Update previous coordinates

  // Show the number of lives for the Player
  if (player->lives > 0)
    tft.fillCircle(230, 8, 7, ILI9341_YELLOW);  // Player has 1 life
  if (player->lives > 1)
    tft.fillCircle(215, 8, 7, ILI9341_YELLOW);  // Player has 2 lives
  else
    tft.fillCircle(215, 8, 7, ILI9341_BLUE); 
  if (player->lives > 2)
    tft.fillCircle(200, 8, 7, ILI9341_YELLOW);  // Player has 3 lives
  else
    tft.fillCircle(200, 8, 7, ILI9341_BLUE); 
    
  //tft.drawRect(x, y, 34, 24, ILI9341_WHITE); // box collider
    
  /* borrowed this sprite from the Flappy Birds example */
  
  // Upper & lower body
  tft.fillRect(x+2, y+8, 2, 10, ILI9341_BLACK);
  tft.fillRect(x+4, y+6, 2, 2, ILI9341_BLACK);
  tft.fillRect(x+6, y+4, 2, 2, ILI9341_BLACK);
  tft.fillRect(x+8, y+2, 4, 2, ILI9341_BLACK);
  tft.fillRect(x+12, y, 12, 2, ILI9341_BLACK);
  tft.fillRect(x+24, y+2, 2, 2, ILI9341_BLACK);
  tft.fillRect(x+26, y+4, 2, 2, ILI9341_BLACK);
  tft.fillRect(x+28, y+6, 2, 6, ILI9341_BLACK);
  tft.fillRect(x+10, y+22, 10, 2, ILI9341_BLACK);
  tft.fillRect(x+4, y+18, 2, 2, ILI9341_BLACK);
  tft.fillRect(x+6, y+20, 4, 2, ILI9341_BLACK);
  
  // Body fill
  tft.fillRect(x+12, y+2, 6, 2, ILI9341_YELLOW);
  tft.fillRect(x+8, y+4, 8, 2, ILI9341_YELLOW);
  tft.fillRect(x+6, y+6, 10, 2, ILI9341_YELLOW);
  tft.fillRect(x+4, y+8, 12, 2, ILI9341_YELLOW);
  tft.fillRect(x+4, y+10, 14, 2, ILI9341_YELLOW);
  tft.fillRect(x+4, y+12, 16, 2, ILI9341_YELLOW);
  tft.fillRect(x+4, y+14, 14, 2, ILI9341_YELLOW);
  tft.fillRect(x+4, y+16, 12, 2, ILI9341_YELLOW);
  tft.fillRect(x+6, y+18, 12, 2, ILI9341_YELLOW);
  tft.fillRect(x+10, y+20, 10, 2, ILI9341_YELLOW);
  
  // Eye
  tft.fillRect(x+18, y+2, 2, 2, ILI9341_BLACK);
  tft.fillRect(x+16, y+4, 2, 6, ILI9341_BLACK);
  tft.fillRect(x+18, y+10, 2, 2, ILI9341_BLACK);
  tft.fillRect(x+18, y+4, 2, 6, ILI9341_WHITE);
  tft.fillRect(x+20, y+2, 4, 10, ILI9341_WHITE);
  tft.fillRect(x+24, y+4, 2, 8, ILI9341_WHITE);
  tft.fillRect(x+26, y+6, 2, 6, ILI9341_WHITE);
  tft.fillRect(x+24, y+6, 2, 4, ILI9341_BLACK);
  
  // Beak
  tft.fillRect(x+20, y+12, 12, 2, ILI9341_BLACK);
  tft.fillRect(x+18, y+14, 2, 2, ILI9341_BLACK);
  tft.fillRect(x+20, y+14, 12, 2, ILI9341_RED);
  tft.fillRect(x+32, y+14, 2, 2, ILI9341_BLACK);
  tft.fillRect(x+16, y+16, 2, 2, ILI9341_BLACK);
  tft.fillRect(x+18, y+16, 2, 2, ILI9341_RED);
  tft.fillRect(x+20, y+16, 12, 2, ILI9341_BLACK);
  tft.fillRect(x+18, y+18, 2, 2, ILI9341_BLACK);
  tft.fillRect(x+20, y+18, 10, 2, ILI9341_RED);
  tft.fillRect(x+30, y+18, 2, 2, ILI9341_BLACK);
  tft.fillRect(x+20, y+20, 10, 2, ILI9341_BLACK);
}

/* Clear the Game Objects before they're re-drawn at new positions */
void clearPlayer(int x, int y) {
  tft.fillRect(x, y, 34, 24, ILI9341_BLUE); // Write over the Players previous position with a solid rectangle matching the background colour
}
void clearLaser(int x, int y) {
  tft.fillRect(x, y, 10, 4, ILI9341_BLUE);  // Write over the Lasers previous positions with a solid rectangle matching the background colour
}
void clearEnemy(int x, int y) {
  tft.fillCircle(x, y, 20, ILI9341_BLUE);   // Write over the Enemies previous position with a solid circle matching the background colour
  //tft.fillRect(x-20, y-20, 41, 41, ILI9341_BLUE); // Test rect to see if it stops flashing of Enemy object when it is drawn
}

/* Draw the movment control buttons */
void drawPlayerControls(){
  // Draw movement buttons
  tft.fillRect(0, 215, 245, 105, ILI9341_DARKGREY);           // 
  tft.drawRect(BTN_X_2, BTN_R1, BTN_W, BTN_H, ILI9341_BLACK); // up 
  tft.setCursor(BTN_X_2 + 17 , BTN_R1 + 20);                  // Set the cursor position to start drawing the text
  tft.setTextColor(ILI9341_WHITE);                            // Set the text colour
  tft.setTextSize(2);                                         // Set the font size
  tft.println("Up");                                          // Set the text for the button
  tft.drawRect(BTN_X_2, BTN_R2, BTN_W, BTN_H, ILI9341_BLACK); // down
  tft.setCursor(BTN_X_2 + 5 , BTN_R2 + 20);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("Down");
  tft.drawRect(BTN_X_1, BTN_R2, BTN_W, BTN_H, ILI9341_BLACK); // left
  tft.setCursor(BTN_X_1 + 5 , BTN_R2 + 20);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("Left");
  tft.drawRect(BTN_X_3, BTN_R2, BTN_W, BTN_H, ILI9341_BLACK); // right
  tft.setCursor(BTN_X_3, BTN_R2 + 20);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("Right");
  
  pauseBtn();                                                 // Draw the pause button
  fireBtn();                                                  // Draw the fire button
}

/* Frame around Pause button */
void drawFrame() {
  tft.drawRect(BTN_X_4, BTN_R2, BTN_W, BTN_H, ILI9341_BLACK);                           // Draw the frame around the button
  tft.drawRect(PAUSE_BUTTON_X, PAUSE_BUTTON_Y, PAUSE_BUTTON_W, BTN_H, ILI9341_BLACK);   // Draws the frame around the pause button
  //drawPlayerControls();                                                               // Test redrawing the player movement buttons after player sprite drawn over their position
}

/* Draw the Fire button */
void fireBtn() { 
  tft.fillRect(FIRE_BUTTON_X, FIRE_BUTTON_Y, FIRE_BUTTON_W, FIRE_BUTTON_H, ILI9341_RED);
  //tft.fillRect(PAUSE_BUTTON_X, PAUSE_BUTTON_Y, PAUSE_BUTTON_W, PAUSE_BUTTON_H, ILI9341_BLUE);
  drawFrame();
  tft.setCursor(FIRE_BUTTON_X + 5 , FIRE_BUTTON_Y + 20);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("Fire");
  //playGame = false;
}
/* Draw the Pause button */
void pauseBtn() {
  if (playGame == true) tft.fillRect(PAUSE_BUTTON_X, PAUSE_BUTTON_Y, PAUSE_BUTTON_W, PAUSE_BUTTON_H, ILI9341_GREEN);
  else  tft.fillRect(PAUSE_BUTTON_X, PAUSE_BUTTON_Y, PAUSE_BUTTON_W, PAUSE_BUTTON_H, ILI9341_RED);
  //tft.fillRect(FIRE_BUTTON_X, FIRE_BUTTON_Y, FIRE_BUTTON_W, FIRE_BUTTON_H, ILI9341_BLUE);
  drawFrame();
  
  tft.setCursor(PAUSE_BUTTON_X + 20, PAUSE_BUTTON_Y + 20);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("PAUSE");
}
/* Draw the Pause button */
void restartBtn() {  
  tft.drawRect(0,260,240,70,ILI9341_BLACK);
  tft.fillRect(1,261,239,69,ILI9341_WHITE);
  
  tft.setCursor(22, 280);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
  tft.println("Press To Restart");  
}
/* Draw the start button */
void startBtn() {
  tft.drawRect(0,260,240,70,ILI9341_BLACK);
  tft.fillRect(1,261,239,69,ILI9341_WHITE);
  
  tft.setCursor(40, 280);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextSize(2);
  tft.println("Press To Begin");    
}

