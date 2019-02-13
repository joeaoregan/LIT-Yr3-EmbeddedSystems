# Embedded Systems
## Joe O'Regan (K00203642)
### Year 3 (2016/17), Semester 7
#### BSc Computing (Games Design and Development)
##### Limerick Institute of Technology

---

## Embedded Systems Assignment. 

Project working with Arduino shields, looking at how they communicate with the Arduino using SPI and I2C inteface.
Simple 2D game implemented, using the Adafruit 2.8" TFT Touchscreen Shield. 
* Highscore stored in EEPROM
* Touchscreen buttons to control player movement and firing
* Simple menu interface
* Player score and countdown timer updated on screen

---

Game tested using Arduino Uno and [Adafruit 2.8" TFT Touch Shield for Arduino](https://www.adafruit.com/product/1651). 
##### Specs:
* **Interface**: Serial Peripheral Interface (SPI)
* **Data Range**: 0 to 4095
* **Resolution**: 320 x 240
##### Libraries:
* **TFT Screen Library**: [https://github.com/adafruit/Adafruit_ILI9341](https://github.com/adafruit/Adafruit_ILI9341 "Adafruit_ILI9341 TFT Screen Library on GitHub")
* **Touchscreen Library**: [https://github.com/adafruit/Adafruit_STMPE610](https://github.com/adafruit/Adafruit_STMPE610 "Adafruit_STMPE610 Touchscreen Library on GitHub")

---

![Embedded Systems CA - Menu](https://raw.githubusercontent.com/joeaoregan/Yr3-Embedded-Systems-CA/master/Screenshots/1-menu.jpg "1. Game Menu")
###### 1. Game Menu
---
![Embedded Systems CA - Level](https://raw.githubusercontent.com/joeaoregan/Yr3-Embedded-Systems-CA/master/Screenshots/2a-game.jpg "2a. Game Level")
###### 2a. Game Level
---
![Embedded Systems CA - Level (Old)](https://raw.githubusercontent.com/joeaoregan/Yr3-Embedded-Systems-CA/master/Screenshots/2b-gameold.jpg "2b. Game Level (Older Version)")
###### 2b. Game Level (Older Version)
---
![Embedded Systems CA - Game Over](https://raw.githubusercontent.com/joeaoregan/Yr3-Embedded-Systems-CA/master/Screenshots/3-gameover.jpg "3. Game Over & High Score")
###### 3. Game Over and High Score saved
---
![Embedded Systems CA - Serial Monitor](https://raw.githubusercontent.com/joeaoregan/Yr3-Embedded-Systems-CA/master/Screenshots/4-serial-monitor.png "4. Serial Monitor")
###### 4. Serial Monitor
---