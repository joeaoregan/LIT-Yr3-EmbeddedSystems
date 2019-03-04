/*
  Arduino Starter Kit example
 Project 2  - Spaceship Interface

 This sketch is written to accompany Project 2 in the
 Arduino Starter Kit

 Parts required:
 1 green LED
 2 red LEDs
 pushbutton
 10 kilohm resistor
 3 220 ohm resistors

 Created 13 September 2012
 by Scott Fitzgerald

 http://www.arduino.cc/starterKit

 This example code is part of the public domain
*/

// Create a global variable to hold the
// state of the switch. This variable is persistent
// throughout the program. Whenever you refer to
// switchState, you’re talking about the number it holds
int switchstate = 0;

void setup() {
  // declare the LED pins as outputs
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  
  pinMode(2, INPUT);  // declare the switch pin as an input
}

void loop() {  
  // digitalRead() checks to see if there is voltage on the pin or not
  switchstate = digitalRead(2); // read the value of the switch

// if the button is not pressed turn on the green LED and off the red LEDs
  if (switchstate == LOW) {
    digitalWrite(3, HIGH); // turn the green LED on pin 3 on
    digitalWrite(4, LOW);  // turn the red LED on pin 4 off
    digitalWrite(5, LOW);  // turn the red LED on pin 5 off
    digitalWrite(6, LOW);  // turn the red LED on pin 6 off
    digitalWrite(7, LOW);  // turn the red LED on pin 7 off
  }
// if the switch is not LOW (the button is pressed) 
// turn off the green LED and blink alternatively the red LEDs
  else {
    digitalWrite(3, LOW);  // turn the green LED on pin 3 off
    digitalWrite(4, HIGH); // turn the red LED on pin 4 on
    digitalWrite(5, LOW);  // turn the red LED on pin 5 off
    digitalWrite(6, LOW);  // turn the red LED on pin 6 off
    digitalWrite(7, LOW);  // turn the red LED on pin 7 off
    delay(250); // wait for a quarter second before changing the light
    digitalWrite(4, LOW);  // turn the red LED on pin 4 off
    digitalWrite(5, HIGH); // turn the red LED on pin 5 on
    delay(250); // wait for a quarter second before changing the light
    digitalWrite(5, LOW);  // turn the red LED on pin 5 off
    digitalWrite(6, HIGH); // turn the red LED on pin 6 on
    delay(250);// wait for a quarter second before changing the light
    digitalWrite(6, LOW);  // turn the red LED on pin 6 off
    digitalWrite(7, HIGH); // turn the red LED on pin 7 on
    delay(250);   // wait for a quarter second before changing the light
    digitalWrite(6, HIGH); // turn the red LED on pin 6 off
    digitalWrite(7, LOW);  // turn the red LED on pin 7 off
    delay(250);// wait for a quarter second before changing the light
    digitalWrite(5, HIGH); // turn the red LED on pin 5 off
    digitalWrite(6, LOW);  // turn the red LED on pin 6 off
    delay(250);// wait for a quarter second before changing the light
  }
}

