/*
  Arduino Starter Kit example
 Project 8  - Digital Hourglass

 This sketch is written to accompany Project 8 in the
 Arduino Starter Kit

 Parts required:
 10 kilohm resistor
 six 220 ohm resistors
 six LEDs
 tilt switch

 Created 13 September 2012
 by Scott Fitzgerald

 http://www.arduino.cc/starterKit

 This example code is part of the public domain
 */

const int switchPin = 8;          // named constant for the switch pin

unsigned long previousTime = 0;   // store the last time an LED was updated
int switchState = 0;              // the current switch state
int prevSwitchState = 0;          // the previous switch state
int led = 2;                      // a variable to refer to the LEDs

// Added to flash LED at end of hour glass time
unsigned int flashTimer = 0;
int flashCounter = 0;
bool reset = false;

// 600000 = 10 minutes in milliseconds
long interval = 600000; // interval at which to light the next LED

void setup() {
  // set the LED pins as outputs
  for (int x = 2; x < 8; x++) {
    pinMode(x, OUTPUT);
  }
  
  pinMode(switchPin, INPUT);      // set the tilt switch pin as input
}

void loop() {  
  unsigned long currentTime = millis(); // store the time since the Arduino started running in a variable
  // compare the current time to the previous time an LED turned on
  // if it is greater than your interval, run the if statement
  if (currentTime - previousTime > interval) {
    previousTime = currentTime;   // save the current time as the last time you changed an LED
    digitalWrite(led, HIGH);      // Turn the LED on    
    // in 10 minutes the next LED will light up
    led++;                        // increment the led variable

    if (led == 7) {
      // the hour is up
      do {
        if (currentTime  > flashTime + 1000){ flashTimer = currentTime; // Set the flash timer 
          
          if (flashCounter % 2 == 1){
            for (int x = 2; x < 8; x++) {
              digitalWrite(x, HIGH);       // Set all LEDs on for odd number
            }
          }
          else {
            for (int x = 2; x < 8; x++) {
              digitalWrite(x, LOW);        // Set all LEDs off for even number
            }
          }
  
          flashCounter ++;                 // Increment the flash Counter
          
          if (flashCounter == 10) reset = true;
          
        } while (flashCounter != 10);      // Flash on / off 10 times (5 on / 5 off)
      }
    }
  }
  
  switchState = digitalRead(switchPin); // read the switch value

  // if the switch has changed
  if (switchState != prevSwitchState || reset == true) {  // added check for finished flashing
    // turn all the LEDs low
    for (int x = 2; x < 8; x++) {
      digitalWrite(x, LOW);
    }
    
    led = 2;                           // reset the LED variable to the first one    
    previousTime = currentTime;       //reset the timer
  }
  
  prevSwitchState = switchState;      // set the previous switch state to the current state
}
