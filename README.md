﻿# Pong game

## Introduction
This project uses an Atmega328P microcontroller to implement a Pong game. The game has two joysticks to control the paddles of two players, a TFT screen for graphical control of the player and an audio buzzer. In addition, users can choose the difficulty of the game using an interactive menu. The project demonstrates the ability of Arduino to control multiple components simultaneously, putting into practice many fundamental concepts such as: the use of ADCs, SPI prototypes and interrupts.

## General Description
### Main features:

1. ```Game Difficulty Selection:```

The game starts with an interactive menu displayed on the TFT screen.
The user can navigate between options: Easy, Medium and Hard by moving the joystick along the Y-axis.
Confirmation of the selection is made by pressing the joystick button.

2. ``` Pong Game:```

The paddles are controlled by the position of two joysticks, with each player having complete control over the movement of their own paddle.
The ball moves automatically and its speed depends on the selected difficulty.

Collisions of the ball with the paddles and the edge of the screen are detected, changing the direction of the ball or awarding points.
Player scores are displayed in real time on the screen.

3. ``` Sound ```

The buzzer emits short signals for key events (collisions, menu, confirmations).


### Project Flow

1. ```Initialization:```
 Configure the pins, the SPI component for the TFT screen, and the ADC to read the joysticks.
 A welcome message is displayed on the screen, followed by the difficulty selection menu.

2. ```Difficulty menu:```
 Arduino checks the position of joystick to navigate between options (ADC reading).
 When the joystick is pressed, the interrupt associated with the button confirms the selection, and the game begins.
 The speed of the ball is adjusted according to the chosen difficulty.

3. ```Game loops:```

    Joysticks: Update the positions of the paddles based on the constant reading of the ADCs.

    Collisions: Detect the interaction of the ball with the paddles or the edges of the screen.
 
    Score: When the ball leaves the field, the point is awarded accordingly and the ball is reset.

## Block diagram
![Imagine-2](images/schema_bloc.png)

### Technical details

1. ```ADC``` (Analog-to-Digital Conversion):
Used to read the joystick positions (Y-axis for each player).
The readings are used to move the paddles or navigate the difficulty menu.

2. ``` SPI``` (Serial Peripheral Interface):
Used for fast communication with the TFT screen.
The screen displays all information: menu, game, scores and system messages.

3. ``` Interrupts: ```
Pressing joystick 2 triggers an interrupt to confirm the selection in the menu.
This allows for fast responses to user actions.


# Hardware Design

##  Components




|   Component name       |     Source piece          | Datasheet                                                                                      | 
|------------------------|---------------------------|------------------------------------------------------------------------------------------------|
|   **1x Arduino**       | faculty                 | https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega164P-324P-644P-Data-Sheet-40002071A.pdf |
|    1x LCD TFT          | I bought https://www.aliexpress.com/item/1005005978400236.html?src=google              | https://randomnerdtutorials.com/guide-to-1-8-tft-display-with-arduino/                         |
|    2x joystick         | faculty                 | https://components101.com/modules/joystick-module                                              |
|    1x buzzer           | faculty                 | https://www.farnell.com/datasheets/2171929.pdf                                                 |
|    1x breadboard       | faculty                 |                                                                                                |
     



![Imagine-1](images/schema_ponggame.png)

| Component         | Arduino Uno Pin |
|-------------------|------------------|
| **1.8" TFT Display** |               |
| LED               | 3.3V             |
| SCK               | 13               |
| SDA               | 11               |
| A0/DC             | 9                |
| RESET             | 8                |
| CS                | 10               |
| GND               | GND              |
| VCC               | 5V               |
| **Buzzer**        | 7                |
| **Joystick 1 VRy** | A0              |
| **Joystick 2 Vry**  | A1             |
| **Joystick 2 Sw**  | 3               |


# Software Design
Currently, the application is able to:

- Display a welcome screen for a defined period.
- Automatically switch to the difficulty menu after the welcome screen is finished.
- Allows navigation through the difficulty menu and selection of options.
- Allows the game to be initiated, managing the countdown and displaying the winner.
- After a round ends, the game resets and a new round starts without user intervention, if the necessary conditions are met.

### Reasons for choosing the libraries used

The following libraries were used in the project:

- Adafruit_GFX - This library provides basic functions for drawing graphics on screens, it does not implement a direct communication protocol, but provides general functions for drawing on the screen. It is required to work with most Adafruit displays, including the ST7735.

- Adafruit_ST7735 - This is the specific library for the ST7735 TFT screen, it implements all the functions necessary to communicate with the display via the SPI protocol and uses the functions provided by Adafruit_GFX to draw and update the screen.

- SPI - This library manages the Serial Peripheral Interface communication protocol. It is used by Adafruit_ST7735 to send data to the TFT screen.

### I used the following functionalities in the lab:

1. ```ADC``` (Analog-to-Digital Conversion)
ADC is used to read analog signals, for example, joystick positions, which are transmitted to the microcontroller as continuous electrical (analog) signals. In the project, this translates into reading the joystick position on the X and Y axes. The obtained values ​​are then used to move the paddles in the game or to navigate the menu.

The ADC converts the analog signal (for example, the voltage on the joystick pin) into a digital value that we can use in the program.
The analogRead() function is used to obtain the value from an analog pin, and the read values ​​are used to control the movement of the paddles on the TFT screen.

Example:
``` int joystickX = analogRead(JOYSTICK_X_PIN); // Reading the position on the X axis ```

``` int joystickY = analogRead(JOYSTICK_Y_PIN); // Read Y-axis position ```

2. ```SPI``` is used to communicate with the TFT display, allowing all visual elements of the game to be displayed, including menus, scores, and status messages (such as "Winner!").

SPI provides fast data transfer between the microcontroller and the TFT display, allowing the image on the screen to be updated quickly without significant delays.

In combination with the Adafruit_ST7735 and Adafruit_GFX libraries, SPI allows precise control of the TFT display and its dynamic updating while the game is running.

``` #define TFT_BLACK ST77XX_BLACK ```\
``` tft.initR(INITR_BLACKTAB); ```\
``` tft.setRotation(2); ```\
``` tft.fillScreen(TFT_BLACK); ```

3. ```Interrupts``` are used to quickly respond to user actions, such as pressing a button or moving a joystick, without having to wait in the main execution loop. This allows the game to continue running and update in real time while waiting for user input.

A button (or joystick) press can be detected by an interrupt, allowing the game to react immediately without having to wait sequentially in the program.

The ```attachInterrupt()``` function is used to trigger an action (for example, confirming a menu selection) when a button on the joystick is pressed, ensuring a quick reaction to user input.

```pinMode(BUTTON_PIN, INPUT_PULLUP);```
```attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressISR, FALLING);```




# Results Obtained

![Imagine-1](images/welcome.jpg)
![Imagine-2](images/menu.jpg)
![Imagine-3](images/start.jpg)

## Videoclip
[![Pong Game](https://img.youtube.com/vi/cH_iqZv2uQQ/0.jpg)](https://www.youtube.com/watch?v=cH_iqZv2uQQ)





# Conclusions
