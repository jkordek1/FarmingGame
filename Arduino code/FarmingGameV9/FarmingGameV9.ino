/*
 U ovoj verziji je dodana DUMMY LEDica 
*/

#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
#include <PNGdec.h>
#include "images.h"
#include <sys/random.h>

#define NEOPIXEL_PIN        12
#define NUMPIXELS           17
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define I2C_SDA 33
#define I2C_SCL 32
#define debug true
#define MAX_IMAGE_WIDTH 240 // Adjust for your images

#define NUMBER_OF_TURNS 7

#define CORN_TURNS 1
#define CARROT_TURNS 2
#define TOMATO_TURNS 2
#define LETTUCE_TURNS 3
#define BLUEBERRY_TURNS 4

#define CORN_POINTS 1
#define CARROT_POINTS 3
#define TOMATO_POINTS 4
#define LETTUCE_POINTS 5
#define BLUEBERRY_POINTS 7

int buttonPins[] = {4, 13, 16, 35, 36, 39, 26, 27, 34, 17, 18, 25};
bool previousState[] = {true, true, true, true, true, true, true, true, true, true, true, true};
bool currentState[] = {true, true, true, true, true, true, true, true, true, true, true, true};
bool P1Buttons[] = {false, false, false};
bool P2Buttons[] = {false, false, false};
bool P3Buttons[] = {false, false, false};
bool P4Buttons[] = {false, false, false};

TwoWire I2C_1 = TwoWire(0);
TwoWire I2C_2 = TwoWire(1);
Ticker readButtons;

PNG png; // PNG decoder instance

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_1, -1);
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_1, -1);

Adafruit_SSD1306 display3(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_2, -1);
Adafruit_SSD1306 display4(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_2, -1);

Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


uint8_t var2 = 1; //redni broj igrača
uint8_t pos;
uint8_t pos2;
uint8_t state = 0;
uint8_t state0Counter = 0;
uint8_t player1TurnCounter = 0;
uint8_t player2TurnCounter = 0;
uint8_t player3TurnCounter = 0;
uint8_t player4TurnCounter = 0;
bool printOnce = true;
bool player1Active = false;
bool player2Active = false;
bool player3Active = false;
bool player4Active = false;

bool players[] = {false, false, false, false};

bool generateWeatherOnce = true;

bool state0Started = false;
unsigned long startMillis;
unsigned long currentMillis;

struct playerStats {
  String weatherState;
  uint8_t points;
  String Field[4];
  int8_t FieldGrow[4];
};

playerStats player1 = {"", 0, {"corn", "empty", "empty", "empty"}, { 1, -1, -1, -1}};
playerStats player2 = {"", 0, {"corn", "empty", "empty", "empty"}, { 1, -1, -1, -1}};
playerStats player3 = {"", 0, {"corn", "empty", "empty", "empty"}, { 1, -1, -1, -1}};
playerStats player4 = {"", 0, {"corn", "empty", "empty", "empty"}, { 1, -1, -1, -1}};


void setup() {

  delay(500);
  Serial.begin(115200);

  //Tipkala INIT
  for (int i = 0; i < 12; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  I2C_2.begin(I2C_SDA, I2C_SCL, 400000);

  if (!display1.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  if (!display2.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  if (!display3.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  if (!display4.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  // Read Buttons every 0.1s
  readButtons.attach(0.1, buttonsPressed);

  tft.init();
  tft.fillScreen(TFT_BLACK);

  display1.clearDisplay();
  display2.clearDisplay();
  display3.clearDisplay();
  display4.clearDisplay();

  pixels.setBrightness(255);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear(); // Set all pixel colors to 'off'
  for ( int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, 0, 0, 0);
  }
  pixels.show();
}

void buttonsPressed() {
  for (int i = 0; i < 12; i++) {
    currentState[i] = digitalRead(buttonPins[i]);
  }
  for (int i = 0; i < 12; i++) {
    if ((previousState[i] == true) && (currentState[i] == false)) {
      if (i < 3) {
        P1Buttons[i] = true;
      } else if ((i > 2) && (i < 6)) {
        P4Buttons[i - 3] = true;
      } else if ((i > 5) && (i < 9)) {
        P3Buttons[i - 6] = true;
      } else if (i > 8) {
        P2Buttons[i - 9] = true;
      }
    }
  }
  for (int i = 0; i < 12; i++) {
    previousState[i] = currentState[i];
  }
}

void state0 () {
  /* Print "Press to play" on displays
     10 sec countdown
     Print which players are active and which are not
  */

  if (!state0Started) {
    startMillis = millis();
    state0Started = true;
  }
  currentMillis = millis();

  if (!players[0]) {
    display1.clearDisplay();
    display1.setRotation(2); //rotacija za 180
    display1.setTextSize(2);
    display1.setTextColor(WHITE);
    display1.setCursor(20, 10);
    display1.println("Press");
    display1.drawBitmap(85, 7, confirmIcon, 20, 20, WHITE);
    display1.setCursor(20, 40);
    display1.println("to play");
    display1.display();
  } else {
    display1.clearDisplay();
    display1.setRotation(2); //rotacija za 180
    display1.setTextSize(2);
    display1.setTextColor(WHITE);
    display1.setCursor(30, 10);
    display1.println("Player");
    display1.setCursor(15, 40);
    display1.println("confirmed");
    display1.display();
  }
  if (!players[1]) {
    display2.clearDisplay();
    display2.setRotation(2); //rotacija za 180
    display2.setTextSize(2);
    display2.setTextColor(WHITE);
    display2.setCursor(20, 10);
    display2.println("Press");
    display2.drawBitmap(85, 7, confirmIcon, 20, 20, WHITE);
    display2.setCursor(20, 40);
    display2.println("to play");
    display2.display();
  } else {
    display2.clearDisplay();
    display2.setRotation(2); //rotacija za 180
    display2.setTextSize(2);
    display2.setTextColor(WHITE);
    display2.setCursor(30, 10);
    display2.println("Player");
    display2.setCursor(15, 40);
    display2.println("confirmed");
    display2.display();
  }
  if (!players[2]) {
    display3.clearDisplay();
    display3.setRotation(2); //rotacija za 180
    display3.setTextSize(2);
    display3.setTextColor(WHITE);
    display3.setCursor(20, 10);
    display3.println("Press");
    display3.drawBitmap(85, 7, confirmIcon, 20, 20, WHITE);
    display3.setCursor(20, 40);
    display3.println("to play");
    display3.display();
  } else {
    display3.clearDisplay();
    display3.setRotation(2); //rotacija za 180
    display3.setTextSize(2);
    display3.setTextColor(WHITE);
    display3.setCursor(30, 10);
    display3.println("Player");
    display3.setCursor(15, 40);
    display3.println("confirmed");
    display3.display();
  }
  if (!players[3]) {
    display4.clearDisplay();
    display4.setRotation(2); //rotacija za 180
    display4.setTextSize(2);
    display4.setTextColor(WHITE);
    display4.setCursor(20, 10);
    display4.println("Press");
    display4.drawBitmap(85, 7, confirmIcon, 20, 20, WHITE);
    display4.setCursor(20, 40);
    display4.println("to play");
    display4.display();
  } else {
    display4.clearDisplay();
    display4.setRotation(2); //rotacija za 180
    display4.setTextSize(2);
    display4.setTextColor(WHITE);
    display4.setCursor(30, 10);
    display4.println("Player");
    display4.setCursor(15, 40);
    display4.println("confirmed");
    display4.display();
  }

  if (currentMillis - startMillis >= 1000)  //test whether the period has elapsed
  {
    state0Counter++;

    if (state0Counter == 11) {
      tft.fillScreen(TFT_BLACK);

      //Sljedeći igrač na redu
      //var2 = 0;

      if (debug) Serial.print("Sljedeći igrač na redu: ");
      if (debug) Serial.println(var2);

      //number of players
      uint8_t var = 0;
      for ( int i = 0; i < 4; i++) {
        if (players[i]) var++;
      }

      if (debug) Serial.print("Ukupan broj igrača: ");
      if (debug) Serial.println(var);

      //global var2 => redni broj igrača na potezu
      //Ovdje treba odrediti koji igrač je aktivan
      for (int i = 0; i < 4; i++) {
        if (players[i]) {
          if (i == 0) player1Active = true;
          else if (i == 1) player2Active = true;
          else if (i == 2) player3Active = true;
          else if (i == 3) player4Active = true;
          if (debug) Serial.print("Aktivni igrač: ");
          if (debug) Serial.println(i + 1);
          break;
        }
      }

      if (var > 1) state = 4; //Weather
      else state = 6; //warning

      if (!players[0]) {
        display1.clearDisplay();
        display1.display();
      }
      if (!players[1]) {
        display2.clearDisplay();
        display2.display();
      }
      if (!players[2]) {
        display3.clearDisplay();
        display3.display();
      }
      if (!players[3]) {
        display4.clearDisplay();
        display4.display();
      }
    } else {
      tft.setRotation(2);
      tft.drawSmoothArc(tft.width() / 2, tft.height() / 2, 110, 80, 0, state0Counter * 36, TFT_WHITE, TFT_BLACK, true);
    }
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }

  //Polling the Confirm buttons
  if (P1Buttons[1]) {
    P1Buttons[1] = false;
    players[0] = true;
    if (debug) Serial.println("Player1 confirmed");
  }
  if (P2Buttons[1]) {
    P2Buttons[1] = false;
    players[1] = true;
    if (debug) Serial.println("Player2 confirmed");
  }
  if (P3Buttons[1]) {
    P3Buttons[1] = false;
    players[2] = true;
    if (debug) Serial.println("Player3 confirmed");
  }
  if (P4Buttons[1]) {
    P4Buttons[1] = false;
    players[3] = true;
    if (debug) Serial.println("Player4 confirmed");
  }
}

void weatherPhase() {

  if (player1Active) {

    if (generateWeatherOnce) {
      tft.setRotation(2);
      //Generate weather
      player1.weatherState = generateWeather();
      if (debug) Serial.print("The weather for player1 is ");
      if (debug) Serial.println(player1.weatherState);

      displayColors();

      display1.clearDisplay();
      display1.setTextSize(2);
      display1.setTextColor(WHITE);
      display1.setCursor(25, 5);
      display1.println("Weather:");

      if ( player1.weatherState != "drought" ) {
        for (int i = 0; i < 4; i++) {
          if (player1.FieldGrow[i] > 0)
            player1.FieldGrow[i] --;
        }
      }

      if ( player1.weatherState == "drought" ) display1.setCursor(25, 22);
      else if ( player1.weatherState == "sunny" ) display1.setCursor(32, 22);
      else if ( player1.weatherState == "rain" ) display1.setCursor(35, 22);

      display1.print(player1.weatherState);
      display1.drawBitmap(54, 40, confirmIcon, 20, 20, WHITE);
      display1.display();
      generateWeatherOnce = false;
    }
    if (P1Buttons[1]) {
      //Confirm
      state = 1;
      pos = 1;
      P1Buttons[1] = false;
      generateWeatherOnce = true;
      displayField();
    }
  } else if (player2Active) {

    if (generateWeatherOnce) {
      tft.setRotation(3);
      //Generate weather
      player2.weatherState = generateWeather();
      if (debug) Serial.print("The weather for player2 is ");
      if (debug) Serial.println(player2.weatherState);

      displayColors();

      display2.clearDisplay();
      display2.setTextSize(2);
      display2.setTextColor(WHITE);
      display2.setCursor(25, 5);
      display2.println("Weather:");

      if ( player2.weatherState != "drought" ) {
        for (int i = 0; i < 4; i++) {
          if (player2.FieldGrow[i] > 0)
            player2.FieldGrow[i] --;
        }
      }

      if ( player2.weatherState == "drought" ) display2.setCursor(25, 22);
      else if ( player2.weatherState == "sunny" ) display2.setCursor(32, 22);
      else if ( player2.weatherState == "rain" ) display2.setCursor(35, 22);

      display2.print(player2.weatherState);
      display2.drawBitmap(54, 40, confirmIcon, 20, 20, WHITE);
      display2.display();
      generateWeatherOnce = false;
    }
    if (P2Buttons[1]) {
      //Confirm
      state = 1;
      pos = 1;
      P2Buttons[1] = false;
      generateWeatherOnce = true;
      displayField();
    }
  } else if (player3Active) {

    if (generateWeatherOnce) {
      tft.setRotation(0);
      //Generate weather
      player3.weatherState = generateWeather();
      if (debug) Serial.print("The weather for player3 is ");
      if (debug) Serial.println(player3.weatherState);

      displayColors();

      display3.clearDisplay();
      display3.setTextSize(2);
      display3.setTextColor(WHITE);
      display3.setCursor(25, 5);
      display3.println("Weather:");

      if ( player3.weatherState != "drought" ) {
        for (int i = 0; i < 4; i++) {
          if (player3.FieldGrow[i] > 0)
            player3.FieldGrow[i] --;
        }
      }

      if ( player3.weatherState == "drought" ) display3.setCursor(25, 22);
      else if ( player3.weatherState == "sunny" ) display3.setCursor(32, 22);
      else if ( player3.weatherState == "rain" ) display3.setCursor(35, 22);

      display3.print(player3.weatherState);
      display3.drawBitmap(54, 40, confirmIcon, 20, 20, WHITE);
      display3.display();
      generateWeatherOnce = false;
    }
    if (P3Buttons[1]) {
      //Confirm
      state = 1;
      pos = 1;
      P3Buttons[1] = false;
      generateWeatherOnce = true;
      displayField();
    }
  } else if (player4Active) {

    if (generateWeatherOnce) {
      tft.setRotation(1);
      //Generate weather
      player4.weatherState = generateWeather();
      if (debug) Serial.print("The weather for player4 is ");
      if (debug) Serial.println(player4.weatherState);

      displayColors();

      display4.clearDisplay();
      display4.setTextSize(2);
      display4.setTextColor(WHITE);
      display4.setCursor(25, 5);
      display4.println("Weather:");

      if ( player4.weatherState != "drought" ) {
        for (int i = 0; i < 4; i++) {
          if (player4.FieldGrow[i] > 0)
            player4.FieldGrow[i] --;
        }
      }

      if ( player4.weatherState == "drought" ) display4.setCursor(25, 22);
      else if ( player4.weatherState == "sunny" ) display4.setCursor(32, 22);
      else if ( player4.weatherState == "rain" ) display4.setCursor(35, 22);

      display4.print(player4.weatherState);
      display4.drawBitmap(54, 40, confirmIcon, 20, 20, WHITE);
      display4.display();
      generateWeatherOnce = false;
    }
    if (P4Buttons[1]) {
      //Confirm
      state = 1;
      pos = 1;
      P4Buttons[1] = false;
      generateWeatherOnce = true;
      displayField();
    }
  }
}

void harvestPhase() {

  if (player1Active) {
    tft.setRotation(2);

    if (pos < 5) {
      display1.clearDisplay();
      display1.setTextSize(2);
      display1.setTextColor(WHITE);
      display1.setCursor(25, 10);
      display1.print("Harvest:");

      display1.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display1.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);

      display1.setTextSize(3);
      display1.setCursor(24, 37);
      display1.print("1");
      display1.setCursor(45, 37);
      display1.print("2");
      display1.setCursor(66, 37);
      display1.print("3");
      display1.setCursor(87, 37);
      display1.print("4");

      if (pos == 1)
        display1.drawRect(22, 34, 20, 26, 1);
      else if (pos == 2)
        display1.drawRect(43, 34, 20, 26, 1);
      else if (pos == 3)
        display1.drawRect(64, 34, 20, 26, 1);
      else if (pos == 4)
        display1.drawRect(85, 34, 20, 26, 1);
    } else {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
      display1.clearDisplay();
      display1.setTextSize(2);
      display1.setCursor(28, 8);
      display1.println("Harvest");
      display1.setCursor(40, 43);
      display1.println("Plant");
      display1.drawBitmap(0, 5, leftArrow, 20, 20, WHITE);
      display1.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    }

    display1.display();

    if (P1Buttons[0]) {
      //Left
      P1Buttons[0] = false;
      if (pos > 1) {
        pos--;
        displayField();
      }
    } else if (P1Buttons[1]) {
      //Confirm
      if ( player1.FieldGrow[ pos - 1 ] == 0 ) {
        if (player1.Field [ pos - 1] == "corn" ) player1.points += CORN_POINTS;
        else if (player1.Field [ pos - 1] == "carrot" ) player1.points += CARROT_POINTS;
        else if (player1.Field [ pos - 1] == "tomato" ) player1.points += TOMATO_POINTS;
        else if (player1.Field [ pos - 1] == "lettuce" ) player1.points += LETTUCE_POINTS;
        else if (player1.Field [ pos - 1] == "blueberry" ) player1.points += BLUEBERRY_POINTS;
        player1.FieldGrow[ pos - 1 ] = -1; //empty
        player1.Field[ pos - 1 ] = "empty";
      }
      P1Buttons[1] = false;
      displayField();
      displayColors();
    } else if (P1Buttons[2]) {
      //Right
      P1Buttons[2] = false;
      if (pos < 6 ) {
        pos++;
        displayField();
      }
      if ( pos == 6) {
        pos = 1;
        state = 2;
        pos2 = 2;
        displayField();
      }
    }
  } else if (player2Active) {
    tft.setRotation(3);

    if (pos < 5) {
      display2.clearDisplay();
      display2.setTextSize(2);
      display2.setTextColor(WHITE);
      display2.setCursor(25, 10);
      display2.print("Harvest:");

      display2.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display2.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);

      display2.setTextSize(3);
      display2.setCursor(24, 37);
      display2.print("1");
      display2.setCursor(45, 37);
      display2.print("2");
      display2.setCursor(66, 37);
      display2.print("3");
      display2.setCursor(87, 37);
      display2.print("4");

      if (pos == 1)
        display2.drawRect(22, 34, 20, 26, 1);
      else if (pos == 2)
        display2.drawRect(43, 34, 20, 26, 1);
      else if (pos == 3)
        display2.drawRect(64, 34, 20, 26, 1);
      else if (pos == 4)
        display2.drawRect(85, 34, 20, 26, 1);
    } else {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
      display2.clearDisplay();
      display2.setTextSize(2);
      display2.setCursor(28, 8);
      display2.println("Harvest");
      display2.setCursor(40, 43);
      display2.println("Plant");
      display2.drawBitmap(0, 5, leftArrow, 20, 20, WHITE);
      display2.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    }

    display2.display();

    if (P2Buttons[0]) {
      //Left
      P2Buttons[0] = false;
      if (pos > 1) {
        pos--;
        displayField();
      }
    } else if (P2Buttons[1]) {
      //Confirm
      if ( player2.FieldGrow[ pos - 1 ] == 0 ) {
        if (player2.Field [ pos - 1] == "corn" ) player2.points += CORN_POINTS;
        else if (player2.Field [ pos - 1] == "carrot" ) player2.points += CARROT_POINTS;
        else if (player2.Field [ pos - 1] == "tomato" ) player2.points += TOMATO_POINTS;
        else if (player2.Field [ pos - 1] == "lettuce" ) player2.points += LETTUCE_POINTS;
        else if (player2.Field [ pos - 1] == "blueberry" ) player2.points += BLUEBERRY_POINTS;
        player2.FieldGrow[ pos - 1 ] = -1; //empty
        player2.Field[ pos - 1 ] = "empty";
      }
      P2Buttons[1] = false;
      displayField();
      displayColors();
    } else if (P2Buttons[2]) {
      //Right
      P2Buttons[2] = false;
      if (pos < 6 ) {
        pos++;
        displayField();
      }
      if ( pos == 6) {
        pos = 1;
        state = 2;
        pos2 = 2;
        displayField();
      }
    }
  } else if (player3Active) {
    tft.setRotation(0);

    if (pos < 5) {
      display3.clearDisplay();
      display3.setTextSize(2);
      display3.setTextColor(WHITE);
      display3.setCursor(25, 10);
      display3.print("Harvest:");

      display3.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display3.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);

      display3.setTextSize(3);
      display3.setCursor(24, 37);
      display3.print("1");
      display3.setCursor(45, 37);
      display3.print("2");
      display3.setCursor(66, 37);
      display3.print("3");
      display3.setCursor(87, 37);
      display3.print("4");

      if (pos == 1)
        display3.drawRect(22, 34, 20, 26, 1);
      else if (pos == 2)
        display3.drawRect(43, 34, 20, 26, 1);
      else if (pos == 3)
        display3.drawRect(64, 34, 20, 26, 1);
      else if (pos == 4)
        display3.drawRect(85, 34, 20, 26, 1);
    } else {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
      display3.clearDisplay();
      display3.setTextSize(2);
      display3.setCursor(28, 8);
      display3.println("Harvest");
      display3.setCursor(40, 43);
      display3.println("Plant");
      display3.drawBitmap(0, 5, leftArrow, 20, 20, WHITE);
      display3.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    }

    display3.display();

    if (P3Buttons[0]) {
      //Left
      P3Buttons[0] = false;
      if (pos > 1) {
        pos--;
        displayField();
      }
    } else if (P3Buttons[1]) {
      //Confirm
      if ( player3.FieldGrow[ pos - 1 ] == 0 ) {
        if (player3.Field [ pos - 1] == "corn" ) player3.points += CORN_POINTS;
        else if (player3.Field [ pos - 1] == "carrot" ) player3.points += CARROT_POINTS;
        else if (player3.Field [ pos - 1] == "tomato" ) player3.points += TOMATO_POINTS;
        else if (player3.Field [ pos - 1] == "lettuce" ) player3.points += LETTUCE_POINTS;
        else if (player3.Field [ pos - 1] == "blueberry" ) player3.points += BLUEBERRY_POINTS;
        player3.FieldGrow[ pos - 1 ] = -1; //empty
        player3.Field[ pos - 1 ] = "empty";
      }
      P3Buttons[1] = false;
      displayField();
      displayColors();
    } else if (P3Buttons[2]) {
      //Right
      P3Buttons[2] = false;
      if (pos < 6 ) {
        pos++;
        displayField();
      }
      if ( pos == 6) {
        pos = 1;
        state = 2;
        pos2 = 2;
        displayField();
      }
    }
  } else if (player4Active) {
    tft.setRotation(1);

    if (pos < 5) {
      display4.clearDisplay();
      display4.setTextSize(2);
      display4.setTextColor(WHITE);
      display4.setCursor(25, 10);
      display4.print("Harvest:");

      display4.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display4.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);

      display4.setTextSize(3);
      display4.setCursor(24, 37);
      display4.print("1");
      display4.setCursor(45, 37);
      display4.print("2");
      display4.setCursor(66, 37);
      display4.print("3");
      display4.setCursor(87, 37);
      display4.print("4");

      if (pos == 1)
        display4.drawRect(22, 34, 20, 26, 1);
      else if (pos == 2)
        display4.drawRect(43, 34, 20, 26, 1);
      else if (pos == 3)
        display4.drawRect(64, 34, 20, 26, 1);
      else if (pos == 4)
        display4.drawRect(85, 34, 20, 26, 1);
    } else {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
      display4.clearDisplay();
      display4.setTextSize(2);
      display4.setCursor(28, 8);
      display4.println("Harvest");
      display4.setCursor(40, 43);
      display4.println("Plant");
      display4.drawBitmap(0, 5, leftArrow, 20, 20, WHITE);
      display4.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    }

    display4.display();

    if (P4Buttons[0]) {
      //Left
      P4Buttons[0] = false;
      if (pos > 1) {
        pos--;
        displayField();
      }
    } else if (P4Buttons[1]) {
      //Confirm
      if ( player4.FieldGrow[ pos - 1 ] == 0 ) {
        if (player4.Field [ pos - 1] == "corn" ) player4.points += CORN_POINTS;
        else if (player4.Field [ pos - 1] == "carrot" ) player4.points += CARROT_POINTS;
        else if (player4.Field [ pos - 1] == "tomato" ) player4.points += TOMATO_POINTS;
        else if (player4.Field [ pos - 1] == "lettuce" ) player4.points += LETTUCE_POINTS;
        else if (player4.Field [ pos - 1] == "blueberry" ) player4.points += BLUEBERRY_POINTS;
        player4.FieldGrow[ pos - 1 ] = -1; //empty
        player4.Field[ pos - 1 ] = "empty";
      }
      P4Buttons[1] = false;
      displayField();
      displayColors();
    } else if (P4Buttons[2]) {
      //Right
      P4Buttons[2] = false;
      if (pos < 6 ) {
        pos++;
        displayField();
      }
      if ( pos == 6) {
        pos = 1;
        state = 2;
        pos2 = 2;
        displayField();
      }
    }
  }
}

void plantPhase() {

  if (player1Active) {
    tft.setRotation(2);
    if ( pos < 5) {

      display1.clearDisplay();
      display1.setTextSize(2);
      display1.setTextColor(WHITE);
      display1.setCursor(30, 10);
      display1.print("Plant:");

      display1.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display1.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);

      display1.setTextSize(3);
      display1.setCursor(24, 37);
      display1.print("1");
      display1.setCursor(45, 37);
      display1.print("2");
      display1.setCursor(66, 37);
      display1.print("3");
      display1.setCursor(87, 37);
      display1.print("4");

      if (pos == 1)
        display1.drawRect(22, 34, 20, 26, 1);
      else if (pos == 2)
        display1.drawRect(43, 34, 20, 26, 1);
      else if (pos == 3)
        display1.drawRect(64, 34, 20, 26, 1);
      else if (pos == 4)
        display1.drawRect(85, 34, 20, 26, 1);
    } else {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
      display1.clearDisplay();
      display1.setTextSize(2);
      display1.setCursor(28, 8);
      display1.println("Plant");
      display1.setCursor(0, 43);
      display1.println("End turn");
      display1.drawBitmap(0, 5, leftArrow, 20, 20, WHITE);
      display1.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    }
    display1.display();

    if (P1Buttons[0]) {
      //Left
      P1Buttons[0] = false;
      if (pos > 1) {
        pos--;
        displayField();
      }
    } else if (P1Buttons[1]) {
      //Confirm
      P1Buttons[1] = false;
      if (( pos == 1 && player1.Field[0] == "empty") || ( pos == 2 && player1.Field[1] == "empty") || ( pos == 3 && player1.Field[2] == "empty") || ( pos == 4 && player1.Field[3] == "empty"))
        state = 3; //Crops
    } else if (P1Buttons[2]) {
      //Right
      P1Buttons[2] = false;
      if (pos < 6 ) {
        pos++;
        displayField();
      }
      if ( pos == 6) {
        pos = 1;
        pos2 = 2;
        tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
        tft.fillScreen(TFT_BLACK);

        player1TurnCounter++; //turn ended

        display1.clearDisplay();
        display1.setTextSize(2);
        display1.setCursor(38, 8);
        display1.print("Turn");
        display1.setCursor(42, 28);
        display1.print(player1TurnCounter);
        display1.print("/");
        display1.print(NUMBER_OF_TURNS);
        display1.setCursor(35, 48);
        display1.print("ended");
        display1.display();

        if (player1TurnCounter >= NUMBER_OF_TURNS) {
          state = 5; //game ended
          players[0] = false;
          var2--;
          player1Active = false;
        }
        else state = 4; // Weather phase

        //number of players
        uint8_t var = 0;
        for ( int i = 0; i < 4; i++) {
          if (players[i]) var++;
        }

        if (debug) Serial.print("Ukupan broj igrača: ");
        if (debug) Serial.println(var);

        var2++; // next player
        if (var == 0) var2 = 0;
        if (var2 > var) var2 = 1;

        if (debug) Serial.print("Sljedeći igrač na redu: ");
        if (debug) Serial.println(var2);
        if (var2 > 0) {
          //global var2 => redni broj igrača na potezu
          //Ovdje treba odrediti koji igrač je aktivan
          uint8_t var3 = 0; //local var for loop counter
          for (int i = 0; i < 4; i++) {
            if ( players[i]) var3++;
            if (var2 == var3) {
              if (i == 0) {
                player1Active = true;
                player2Active = false;
                player3Active = false;
                player4Active = false;
              }
              else if (i == 1) {
                player1Active = false;
                player2Active = true;
                player3Active = false;
                player4Active = false;
              }
              else if (i == 2) {
                player1Active = false;
                player2Active = false;
                player3Active = true;
                player4Active = false;
              }
              else if (i == 3) {
                player1Active = false;
                player2Active = false;
                player3Active = false;
                player4Active = true;
              } else {
                player1Active = false;
                player2Active = false;
                player3Active = false;
                player4Active = false;
              }
              if (debug) Serial.print("Aktivni igrač: ");
              if (debug) Serial.println(i + 1);
              break;
            }
          }
        }
      }
    }
  } else if (player2Active) {
    tft.setRotation(3);
    if ( pos < 5) {

      display2.clearDisplay();
      display2.setTextSize(2);
      display2.setTextColor(WHITE);
      display2.setCursor(30, 10);
      display2.print("Plant:");

      display2.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display2.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);

      display2.setTextSize(3);
      display2.setCursor(24, 37);
      display2.print("1");
      display2.setCursor(45, 37);
      display2.print("2");
      display2.setCursor(66, 37);
      display2.print("3");
      display2.setCursor(87, 37);
      display2.print("4");

      if (pos == 1)
        display2.drawRect(22, 34, 20, 26, 1);
      else if (pos == 2)
        display2.drawRect(43, 34, 20, 26, 1);
      else if (pos == 3)
        display2.drawRect(64, 34, 20, 26, 1);
      else if (pos == 4)
        display2.drawRect(85, 34, 20, 26, 1);
    } else {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
      display2.clearDisplay();
      display2.setTextSize(2);
      display2.setCursor(28, 8);
      display2.println("Plant");
      display2.setCursor(0, 43);
      display2.println("End turn");
      display2.drawBitmap(0, 5, leftArrow, 20, 20, WHITE);
      display2.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    }
    display2.display();

    if (P2Buttons[0]) {
      //Left
      P2Buttons[0] = false;
      if (pos > 1) {
        pos--;
        displayField();
      }
    } else if (P2Buttons[1]) {
      //Confirm
      P2Buttons[1] = false;
      if (( pos == 1 && player2.Field[0] == "empty") || ( pos == 2 && player2.Field[1] == "empty") || ( pos == 3 && player2.Field[2] == "empty") || ( pos == 4 && player2.Field[3] == "empty"))
        state = 3; //Crops
    } else if (P2Buttons[2]) {
      //Right
      P2Buttons[2] = false;
      if (pos < 6 ) {
        pos++;
        displayField();
      }
      if ( pos == 6) {
        pos = 1;
        pos2 = 2;
        tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
        tft.fillScreen(TFT_BLACK);
        player2TurnCounter++; //turn ended

        display2.clearDisplay();
        display2.setTextSize(2);
        display2.setCursor(38, 8);
        display2.print("Turn");
        display2.setCursor(42, 28);
        display2.print(player1TurnCounter);
        display2.print("/");
        display2.print(NUMBER_OF_TURNS);
        display2.setCursor(35, 48);
        display2.print("ended");
        display2.display();

        if (player2TurnCounter >= NUMBER_OF_TURNS) {
          state = 5; //game ended
          players[1] = false;
          var2--;
          player2Active = false;
        }
        else state = 4; // Weather phase

        //number of players
        uint8_t var = 0;
        for ( int i = 0; i < 4; i++) {
          if (players[i]) var++;
        }

        if (debug) Serial.print("Ukupan broj igrača: ");
        if (debug) Serial.println(var);

        var2++; // next player
        if (var == 0) var2 = 0;
        if (var2 > var) var2 = 1;

        if (debug) Serial.print("Sljedeći igrač na redu: ");
        if (debug) Serial.println(var2);
        if (var2 > 0) {
          //global var2 => redni broj igrača na potezu
          //Ovdje treba odrediti koji igrač je aktivan
          uint8_t var3 = 0; //local var for loop counter
          for (int i = 0; i < 4; i++) {
            if ( players[i]) var3++;
            if (var2 == var3) {
              if (i == 0) {
                player1Active = true;
                player2Active = false;
                player3Active = false;
                player4Active = false;
              }
              else if (i == 1) {
                player1Active = false;
                player2Active = true;
                player3Active = false;
                player4Active = false;
              }
              else if (i == 2) {
                player1Active = false;
                player2Active = false;
                player3Active = true;
                player4Active = false;
              }
              else if (i == 3) {
                player1Active = false;
                player2Active = false;
                player3Active = false;
                player4Active = true;
              } else {
                player1Active = false;
                player2Active = false;
                player3Active = false;
                player4Active = false;
              }
              if (debug) Serial.print("Aktivni igrač: ");
              if (debug) Serial.println(i + 1);
              break;
            }
          }
        }
      }
    }
  } else if (player3Active) {
    tft.setRotation(0);
    if ( pos < 5) {

      display3.clearDisplay();
      display3.setTextSize(2);
      display3.setTextColor(WHITE);
      display3.setCursor(30, 10);
      display3.print("Plant:");

      display3.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display3.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);

      display3.setTextSize(3);
      display3.setCursor(24, 37);
      display3.print("1");
      display3.setCursor(45, 37);
      display3.print("2");
      display3.setCursor(66, 37);
      display3.print("3");
      display3.setCursor(87, 37);
      display3.print("4");

      if (pos == 1)
        display3.drawRect(22, 34, 20, 26, 1);
      else if (pos == 2)
        display3.drawRect(43, 34, 20, 26, 1);
      else if (pos == 3)
        display3.drawRect(64, 34, 20, 26, 1);
      else if (pos == 4)
        display3.drawRect(85, 34, 20, 26, 1);
    } else {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
      display3.clearDisplay();
      display3.setTextSize(2);
      display3.setCursor(28, 8);
      display3.println("Plant");
      display3.setCursor(0, 43);
      display3.println("End turn");
      display3.drawBitmap(0, 5, leftArrow, 20, 20, WHITE);
      display3.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    }
    display3.display();

    if (P3Buttons[0]) {
      //Left
      P3Buttons[0] = false;
      if (pos > 1) {
        pos--;
        displayField();
      }
    } else if (P3Buttons[1]) {
      //Confirm
      P3Buttons[1] = false;
      if (( pos == 1 && player3.Field[0] == "empty") || ( pos == 2 && player3.Field[1] == "empty") || ( pos == 3 && player3.Field[2] == "empty") || ( pos == 4 && player3.Field[3] == "empty"))
        state = 3; //Crops
    } else if (P3Buttons[2]) {
      //Right
      P3Buttons[2] = false;
      if (pos < 6 ) {
        pos++;
        displayField();
      }
      if ( pos == 6) {
        pos = 1;
        pos2 = 2;
        tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
        tft.fillScreen(TFT_BLACK);
        player3TurnCounter++; //turn ended

        display3.clearDisplay();
        display3.setTextSize(2);
        display3.setCursor(38, 8);
        display3.print("Turn");
        display3.setCursor(42, 28);
        display3.print(player1TurnCounter);
        display3.print("/");
        display3.print(NUMBER_OF_TURNS);
        display3.setCursor(35, 48);
        display3.print("ended");
        display3.display();

        if (player3TurnCounter >= NUMBER_OF_TURNS) {
          state = 5; //game ended
          players[2] = false;
          var2--;
          player3Active = false;
        }
        else state = 4; // Weather phase

        //number of players
        uint8_t var = 0;
        for ( int i = 0; i < 4; i++) {
          if (players[i]) var++;
        }

        if (debug) Serial.print("Ukupan broj igrača: ");
        if (debug) Serial.println(var);

        var2++; // next player
        if (var == 0) var2 = 0;
        if (var2 > var) var2 = 1;

        if (debug) Serial.print("Sljedeći igrač na redu: ");
        if (debug) Serial.println(var2);
        if (var2 > 0) {
          //global var2 => redni broj igrača na potezu
          //Ovdje treba odrediti koji igrač je aktivan
          uint8_t var3 = 0; //local var for loop counter
          for (int i = 0; i < 4; i++) {
            if ( players[i]) var3++;
            if (var2 == var3) {
              if (i == 0) {
                player1Active = true;
                player2Active = false;
                player3Active = false;
                player4Active = false;
              }
              else if (i == 1) {
                player1Active = false;
                player2Active = true;
                player3Active = false;
                player4Active = false;
              }
              else if (i == 2) {
                player1Active = false;
                player2Active = false;
                player3Active = true;
                player4Active = false;
              }
              else if (i == 3) {
                player1Active = false;
                player2Active = false;
                player3Active = false;
                player4Active = true;
              } else {
                player1Active = false;
                player2Active = false;
                player3Active = false;
                player4Active = false;
              }
              if (debug) Serial.print("Aktivni igrač: ");
              if (debug) Serial.println(i + 1);
              break;
            }
          }
        }
      }
    }
  } else if (player4Active) {
    tft.setRotation(1);
    if ( pos < 5) {

      display4.clearDisplay();
      display4.setTextSize(2);
      display4.setTextColor(WHITE);
      display4.setCursor(30, 10);
      display4.print("Plant:");

      display4.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display4.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);

      display4.setTextSize(3);
      display4.setCursor(24, 37);
      display4.print("1");
      display4.setCursor(45, 37);
      display4.print("2");
      display4.setCursor(66, 37);
      display4.print("3");
      display4.setCursor(87, 37);
      display4.print("4");

      if (pos == 1)
        display4.drawRect(22, 34, 20, 26, 1);
      else if (pos == 2)
        display4.drawRect(43, 34, 20, 26, 1);
      else if (pos == 3)
        display4.drawRect(64, 34, 20, 26, 1);
      else if (pos == 4)
        display4.drawRect(85, 34, 20, 26, 1);
    } else {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
      display4.clearDisplay();
      display4.setTextSize(2);
      display4.setCursor(28, 8);
      display4.println("Plant");
      display4.setCursor(0, 43);
      display4.println("End turn");
      display4.drawBitmap(0, 5, leftArrow, 20, 20, WHITE);
      display4.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    }
    display4.display();

    if (P4Buttons[0]) {
      //Left
      P4Buttons[0] = false;
      if (pos > 1) {
        pos--;
        displayField();
      }
    } else if (P4Buttons[1]) {
      //Confirm
      P4Buttons[1] = false;
      if (( pos == 1 && player4.Field[0] == "empty") || ( pos == 2 && player4.Field[1] == "empty") || ( pos == 3 && player4.Field[2] == "empty") || ( pos == 4 && player4.Field[3] == "empty"))
        state = 3; //Crops
    } else if (P4Buttons[2]) {
      //Right
      P4Buttons[2] = false;
      if (pos < 6 ) {
        pos++;
        displayField();
      }
      if ( pos == 6) {
        pos = 1;
        pos2 = 2;
        tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
        tft.fillScreen(TFT_BLACK);
        player4TurnCounter++; //turn ended

        display4.clearDisplay();
        display4.setTextSize(2);
        display4.setCursor(38, 8);
        display4.print("Turn");
        display4.setCursor(42, 28);
        display4.print(player1TurnCounter);
        display4.print("/");
        display4.print(NUMBER_OF_TURNS);
        display4.setCursor(35, 48);
        display4.print("ended");
        display4.display();

        if (player4TurnCounter >= NUMBER_OF_TURNS) {
          player4Active = false;
          players[3] = false;
          var2--;
          state = 5; //game ended
        } else state = 4; // Weather phase

        //number of players
        uint8_t var = 0;
        for ( int i = 0; i < 4; i++) {
          if (players[i]) var++;
        }

        if (debug) Serial.print("Ukupan broj igrača: ");
        if (debug) Serial.println(var);

        var2++; // next player
        if (var == 0) var2 = 0;
        if (var2 > var) var2 = 1;

        if (debug) Serial.print("Sljedeći igrač na redu: ");
        if (debug) Serial.println(var2);
        if (var2 > 0) {
          //global var2 => redni broj igrača na potezu
          //Ovdje treba odrediti koji igrač je aktivan
          uint8_t var3 = 0; //local var for loop counter
          for (int i = 0; i < 4; i++) {
            if ( players[i]) var3++;
            if (var2 == var3) {
              if (i == 0) {
                player1Active = true;
                player2Active = false;
                player3Active = false;
                player4Active = false;
              }
              else if (i == 1) {
                player1Active = false;
                player2Active = true;
                player3Active = false;
                player4Active = false;
              }
              else if (i == 2) {
                player1Active = false;
                player2Active = false;
                player3Active = true;
                player4Active = false;
              }
              else if (i == 3) {
                player1Active = false;
                player2Active = false;
                player3Active = false;
                player4Active = true;
              } else {
                player1Active = false;
                player2Active = false;
                player3Active = false;
                player4Active = false;
              }
              if (debug) Serial.print("Aktivni igrač: ");
              if (debug) Serial.println(i + 1);
              break;
            }
          }
        }
      }
    }
  }
}

void crops() {

  if (player1Active) {
    tft.setRotation(2);

    display1.clearDisplay();
    display1.setTextSize(3);
    display1.setTextColor(WHITE);
    display1.setCursor(0, 10);

    if (pos2 == 1) {
      display1.setCursor(0, 5);
      display1.drawBitmap(0, 5, leftArrow, 20, 20, WHITE);
      display1.setTextSize(2);
      display1.setCursor(30, 8);
      display1.print("Fields");
      display1.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
      display1.setCursor(38, 40);
      display1.print("Crops");
      display1.setTextSize(3);

    } else if (pos2 == 2) {
      displayCorn();
      display1.setCursor(30, 5);
      display1.println("Corn");
      display1.setCursor(45, 40);
      display1.setTextSize(2);
      display1.println("1pt");
      display1.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display1.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    }  else if (pos2 == 3) {
      displayCarrot();
      display1.setCursor(12, 5);
      display1.setTextSize(3);
      display1.println("Carrot");
      display1.setCursor(40, 40);
      display1.setTextSize(2);
      display1.println("2pts");
      display1.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display1.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    } else if (pos2 == 4) {
      displayTomato();
      display1.setCursor(12, 5);
      display1.setTextSize(3);
      display1.println("Tomato");
      display1.setCursor(40, 40);
      display1.setTextSize(2);
      display1.println("2pts");
      display1.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display1.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    } else if (pos2 == 5) {
      displayLettuce();
      display1.setCursor(2, 5);
      display1.setTextSize(3);
      display1.println("Lettuce");
      display1.setCursor(40, 40);
      display1.setTextSize(2);
      display1.println("3pts");
      display1.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display1.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    } else if (pos2 == 6) {
      displayBlueberry();
      display1.setTextSize(3);
      display1.setCursor(10, 10);
      display1.setTextSize(2);
      display1.println("Blueberry");
      display1.setCursor(40, 40);
      display1.setTextSize(2);
      display1.println("4pts");
      display1.setTextSize(3);
      display1.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
    }
    display1.display();

    if (P1Buttons[0]) {
      //Left
      if (pos2 > 0) pos2--;
      if (pos2 == 0) {
        pos2 = 2;
        state = 2;
        displayField();
        displayColors();
      }
      P1Buttons[0] = false;
    } else if (P1Buttons[1]) {
      //Confirm
      if ( pos2 == 2) {
        //Corn selected
        player1.Field[pos - 1] = "corn";
        player1.FieldGrow[pos - 1] = CORN_TURNS;
        pixels.setPixelColor(pos - 1, 255, 255, 0); //YELLOW
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 3) {
        //Corn selected
        player1.Field[pos - 1] = "carrot";
        player1.FieldGrow[pos - 1] = CARROT_TURNS;
        pixels.setPixelColor(pos - 1, 255, 165, 0); //ORANGE
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 4) {
        //Corn selected
        player1.Field[pos - 1] = "tomato";
        player1.FieldGrow[pos - 1] = TOMATO_TURNS;
        pixels.setPixelColor(pos - 1, 255, 0, 0); //RED
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 5) {
        //Corn selected
        player1.Field[pos - 1] = "lettuce";
        player1.FieldGrow[pos - 1] = LETTUCE_TURNS;
        pixels.setPixelColor(pos - 1, 0, 255, 0); //GREEN
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 6) {
        //Corn selected
        player1.Field[pos - 1] = "blueberry";
        player1.FieldGrow[pos - 1] = BLUEBERRY_TURNS;
        pixels.setPixelColor(pos - 1, 0, 0, 255); //BLUE
        pixels.show();   // Send the updated pixel colors to the hardware.
      }

      P1Buttons[1] = false;
      pos2 = 2;
      state = 2;
      displayField();
      displayColors();
    } else if (P1Buttons[2]) {
      //Right
      if (pos2 < 6) pos2++;
      P1Buttons[2] = false;
    }
  } else if (player2Active) {
    tft.setRotation(3);

    display2.clearDisplay();
    display2.setTextSize(3);
    display2.setTextColor(WHITE);
    display2.setCursor(0, 10);

    if (pos2 == 1) {
      display2.setCursor(0, 5);
      display2.drawBitmap(0, 5, leftArrow, 20, 20, WHITE);
      display2.setTextSize(2);
      display2.setCursor(30, 8);
      display2.print("Fields");
      display2.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
      display2.setCursor(38, 40);
      display2.print("Crops");
      display2.setTextSize(3);

    } else if (pos2 == 2) {
      displayCorn();
      display2.setCursor(30, 5);
      display2.println("Corn");
      display2.setCursor(45, 40);
      display2.setTextSize(2);
      display2.println("1pt");
      display2.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display2.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    }  else if (pos2 == 3) {
      displayCarrot();
      display2.setCursor(12, 5);
      display2.setTextSize(3);
      display2.println("Carrot");
      display2.setCursor(40, 40);
      display2.setTextSize(2);
      display2.println("2pts");
      display2.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display2.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    } else if (pos2 == 4) {
      displayTomato();
      display2.setCursor(12, 5);
      display2.setTextSize(3);
      display2.println("Tomato");
      display2.setCursor(40, 40);
      display2.setTextSize(2);
      display2.println("2pts");
      display2.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display2.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    } else if (pos2 == 5) {
      displayLettuce();
      display2.setCursor(2, 5);
      display2.setTextSize(3);
      display2.println("Lettuce");
      display2.setCursor(40, 40);
      display2.setTextSize(2);
      display2.println("3pts");
      display2.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display2.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    } else if (pos2 == 6) {
      displayBlueberry();
      display2.setTextSize(3);
      display2.setCursor(10, 10);
      display2.setTextSize(2);
      display2.println("Blueberry");
      display2.setCursor(40, 40);
      display2.setTextSize(2);
      display2.println("4pts");
      display2.setTextSize(3);
      display2.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
    }
    display2.display();

    if (P2Buttons[0]) {
      //Left
      if (pos2 > 0) pos2--;
      if (pos2 == 0) {
        pos2 = 2;
        state = 2;
        displayField();
        displayColors();
      }
      P2Buttons[0] = false;
    } else if (P2Buttons[1]) {
      //Confirm
      if ( pos2 == 2) {
        //Corn selected
        player2.Field[pos - 1] = "corn";
        player2.FieldGrow[pos - 1] = CORN_TURNS;
        pixels.setPixelColor(pos + 4, 255, 255, 0); //YELLOW
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 3) {
        //Corn selected
        player2.Field[pos - 1] = "carrot";
        player2.FieldGrow[pos - 1] = CARROT_TURNS;
        pixels.setPixelColor(pos + 4, 255, 165, 0); //ORANGE
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 4) {
        //Corn selected
        player2.Field[pos - 1] = "tomato";
        player2.FieldGrow[pos - 1] = TOMATO_TURNS;
        pixels.setPixelColor(pos + 4, 255, 0, 0); //RED
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 5) {
        //Corn selected
        player2.Field[pos - 1] = "lettuce";
        player2.FieldGrow[pos - 1] = LETTUCE_TURNS;
        pixels.setPixelColor(pos + 4, 0, 255, 0); //GREEN
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 6) {
        //Corn selected
        player2.Field[pos - 1] = "blueberry";
        player2.FieldGrow[pos - 1] = BLUEBERRY_TURNS;
        pixels.setPixelColor(pos + 4, 0, 0, 255); //BLUE
        pixels.show();   // Send the updated pixel colors to the hardware.
      }

      P2Buttons[1] = false;
      pos2 = 2;
      state = 2;
      displayField();
      displayColors();
    } else if (P2Buttons[2]) {
      //Right
      if (pos2 < 6) pos2++;
      P2Buttons[2] = false;
    }
  } else if (player3Active) {
    tft.setRotation(0);

    display3.clearDisplay();
    display3.setTextSize(3);
    display3.setTextColor(WHITE);
    display3.setCursor(0, 10);

    if (pos2 == 1) {
      display3.setCursor(0, 5);
      display3.drawBitmap(0, 5, leftArrow, 20, 20, WHITE);
      display3.setTextSize(2);
      display3.setCursor(30, 8);
      display3.print("Fields");
      display3.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
      display3.setCursor(38, 40);
      display3.print("Crops");
      display3.setTextSize(3);

    } else if (pos2 == 2) {
      displayCorn();
      display3.setCursor(30, 5);
      display3.println("Corn");
      display3.setCursor(45, 40);
      display3.setTextSize(2);
      display3.println("1pt");
      display3.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display3.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    }  else if (pos2 == 3) {
      displayCarrot();
      display3.setCursor(12, 5);
      display3.setTextSize(3);
      display3.println("Carrot");
      display3.setCursor(40, 40);
      display3.setTextSize(2);
      display3.println("2pts");
      display3.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display3.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    } else if (pos2 == 4) {
      displayTomato();
      display3.setCursor(12, 5);
      display3.setTextSize(3);
      display3.println("Tomato");
      display3.setCursor(40, 40);
      display3.setTextSize(2);
      display3.println("2pts");
      display3.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display3.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    } else if (pos2 == 5) {
      displayLettuce();
      display3.setCursor(2, 5);
      display3.setTextSize(3);
      display3.println("Lettuce");
      display3.setCursor(40, 40);
      display3.setTextSize(2);
      display3.println("3pts");
      display3.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display3.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    } else if (pos2 == 6) {
      displayBlueberry();
      display3.setTextSize(3);
      display3.setCursor(10, 10);
      display3.setTextSize(2);
      display3.println("Blueberry");
      display3.setCursor(40, 40);
      display3.setTextSize(2);
      display3.println("4pts");
      display3.setTextSize(3);
      display3.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
    }
    display3.display();

    if (P3Buttons[0]) {
      //Left
      if (pos2 > 0) pos2--;
      if (pos2 == 0) {
        pos2 = 2;
        state = 2;
        displayField();
        displayColors();
      }
      P3Buttons[0] = false;
    } else if (P3Buttons[1]) {
      //Confirm
      if ( pos2 == 2) {
        //Corn selected
        player3.Field[pos - 1] = "corn";
        player3.FieldGrow[pos - 1] = CORN_TURNS;
        pixels.setPixelColor(pos + 8, 255, 255, 0); //YELLOW
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 3) {
        //Corn selected
        player3.Field[pos - 1] = "carrot";
        player3.FieldGrow[pos - 1] = CARROT_TURNS;
        pixels.setPixelColor(pos + 8, 255, 165, 0); //ORANGE
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 4) {
        //Corn selected
        player3.Field[pos - 1] = "tomato";
        player3.FieldGrow[pos - 1] = TOMATO_TURNS;
        pixels.setPixelColor(pos + 8, 255, 0, 0); //RED
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 5) {
        //Corn selected
        player3.Field[pos - 1] = "lettuce";
        player3.FieldGrow[pos - 1] = LETTUCE_TURNS;
        pixels.setPixelColor(pos + 8, 0, 255, 0); //GREEN
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 6) {
        //Corn selected
        player3.Field[pos - 1] = "blueberry";
        player3.FieldGrow[pos - 1] = BLUEBERRY_TURNS;
        pixels.setPixelColor(pos + 8, 0, 0, 255); //BLUE
        pixels.show();   // Send the updated pixel colors to the hardware.
      }

      P3Buttons[1] = false;
      pos2 = 2;
      state = 2;
      displayField();
      displayColors();
    } else if (P3Buttons[2]) {
      //Right
      if (pos2 < 6) pos2++;
      P3Buttons[2] = false;
    }
  } else if (player4Active) {
    tft.setRotation(1);

    display4.clearDisplay();
    display4.setTextSize(3);
    display4.setTextColor(WHITE);
    display4.setCursor(0, 10);

    if (pos2 == 1) {
      display4.setCursor(0, 5);
      display4.drawBitmap(0, 5, leftArrow, 20, 20, WHITE);
      display4.setTextSize(2);
      display4.setCursor(30, 8);
      display4.print("Fields");
      display4.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
      display4.setCursor(38, 40);
      display4.print("Crops");
      display4.setTextSize(3);

    } else if (pos2 == 2) {
      displayCorn();
      display4.setCursor(30, 5);
      display4.println("Corn");
      display4.setCursor(45, 40);
      display4.setTextSize(2);
      display4.println("1pt");
      display4.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display4.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    }  else if (pos2 == 3) {
      displayCarrot();
      display4.setCursor(12, 5);
      display4.setTextSize(3);
      display4.println("Carrot");
      display4.setCursor(40, 40);
      display4.setTextSize(2);
      display4.println("2pts");
      display4.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display4.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    } else if (pos2 == 4) {
      displayTomato();
      display4.setCursor(12, 5);
      display4.setTextSize(3);
      display4.println("Tomato");
      display4.setCursor(40, 40);
      display4.setTextSize(2);
      display4.println("2pts");
      display4.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display4.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    } else if (pos2 == 5) {
      displayLettuce();
      display4.setCursor(2, 5);
      display4.setTextSize(3);
      display4.println("Lettuce");
      display4.setCursor(40, 40);
      display4.setTextSize(2);
      display4.println("3pts");
      display4.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
      display4.drawBitmap(108, 40, rightArrow, 20, 20, WHITE);
    } else if (pos2 == 6) {
      displayBlueberry();
      display4.setTextSize(3);
      display4.setCursor(10, 10);
      display4.setTextSize(2);
      display4.println("Blueberry");
      display4.setCursor(40, 40);
      display4.setTextSize(2);
      display4.println("4pts");
      display4.setTextSize(3);
      display4.drawBitmap(0, 40, leftArrow, 20, 20, WHITE);
    }
    display4.display();

    if (P4Buttons[0]) {
      //Left
      if (pos2 > 0) pos2--;
      if (pos2 == 0) {
        pos2 = 2;
        state = 2;
        displayField();
        displayColors();
      }
      P4Buttons[0] = false;
    } else if (P4Buttons[1]) {
      //Confirm
      if ( pos2 == 2) {
        //Corn selected
        player4.Field[pos - 1] = "corn";
        player4.FieldGrow[pos - 1] = CORN_TURNS;
        pixels.setPixelColor(pos + 12, 255, 255, 0); //YELLOW
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 3) {
        //Corn selected
        player4.Field[pos - 1] = "carrot";
        player4.FieldGrow[pos - 1] = CARROT_TURNS;
        pixels.setPixelColor(pos + 12, 255, 165, 0); //ORANGE
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 4) {
        //Corn selected
        player4.Field[pos - 1] = "tomato";
        player4.FieldGrow[pos - 1] = TOMATO_TURNS;
        pixels.setPixelColor(pos + 12, 255, 0, 0); //RED
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 5) {
        //Corn selected
        player4.Field[pos - 1] = "lettuce";
        player4.FieldGrow[pos - 1] = LETTUCE_TURNS;
        pixels.setPixelColor(pos + 12, 0, 255, 0); //GREEN
        pixels.show();   // Send the updated pixel colors to the hardware.
      } else if ( pos2 == 6) {
        //Corn selected
        player4.Field[pos - 1] = "blueberry";
        player4.FieldGrow[pos - 1] = BLUEBERRY_TURNS;
        pixels.setPixelColor(pos + 12, 0, 0, 255); //BLUE
        pixels.show();   // Send the updated pixel colors to the hardware.
      }

      P4Buttons[1] = false;
      pos2 = 2;
      state = 2;
      displayField();
      displayColors();
    } else if (P4Buttons[2]) {
      //Right
      if (pos2 < 6) pos2++;
      P4Buttons[2] = false;
    }
  }
}

void displayField() {

  if (player1Active) {
    tft.setRotation(2);
    tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
    tft.drawSmoothArc(tft.width() / 2, tft.height() / 2, 120, 100, 0, 360, TFT_RED, TFT_BLACK, true);
    tft.setTextColor(TFT_WHITE);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    // Print a message on the screen
    tft.drawString("Score", tft.width() / 2, 193);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString(String(player1.points), tft.width() / 2, 210);
    tft.setTextDatum(TL_DATUM);

    tft.fillRect(56, 56, 128, 128, TFT_BLACK);
    if ( pos == 1 ) {
      if ( player1.Field[0] == "empty" ) displayEmpty();
      else if ( player1.Field[0] == "corn") displayCorn();
      else if ( player1.Field[0] == "carrot") displayCarrot();
      else if ( player1.Field[0] == "tomato") displayTomato();
      else if ( player1.Field[0] == "lettuce") displayLettuce();
      else if ( player1.Field[0] == "blueberry") displayBlueberry();
    } else if ( pos == 2 ) {
      if ( player1.Field[1] == "empty" ) displayEmpty();
      else if ( player1.Field[1] == "corn") displayCorn();
      else if ( player1.Field[1] == "carrot") displayCarrot();
      else if ( player1.Field[1] == "tomato") displayTomato();
      else if ( player1.Field[1] == "lettuce") displayLettuce();
      else if ( player1.Field[1] == "blueberry") displayBlueberry();
    } else if ( pos == 3 ) {
      if ( player1.Field[2] == "empty" ) displayEmpty();
      else if ( player1.Field[2] == "corn") displayCorn();
      else if ( player1.Field[2] == "carrot") displayCarrot();
      else if ( player1.Field[2] == "tomato") displayTomato();
      else if ( player1.Field[2] == "lettuce") displayLettuce();
      else if ( player1.Field[2] == "blueberry") displayBlueberry();
    } else if ( pos == 4 ) {
      if ( player1.Field[3] == "empty" ) displayEmpty();
      else if ( player1.Field[3] == "corn") displayCorn();
      else if ( player1.Field[3] == "carrot") displayCarrot();
      else if ( player1.Field[3] == "tomato") displayTomato();
      else if ( player1.Field[3] == "lettuce") displayLettuce();
      else if ( player1.Field[3] == "blueberry") displayBlueberry();
    }

    if ( player1.FieldGrow[pos - 1] == -1) {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
    } else {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
      tft.setTextDatum(MC_DATUM);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(3);
      tft.drawString(String(player1.FieldGrow[pos - 1]), tft.width() / 2, tft.height() / 2 - 75);
      tft.setTextDatum(TL_DATUM);
    }

  } else if (player2Active) {
    tft.setRotation(3);
    tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
    tft.drawSmoothArc(tft.width() / 2, tft.height() / 2, 120, 100, 0, 360, TFT_YELLOW, TFT_BLACK, true);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    // Print a message on the screen
    tft.drawString("Score", tft.width() / 2, 193);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString(String(player2.points), tft.width() / 2, 210);
    tft.setTextDatum(TL_DATUM);

    tft.fillRect(56, 56, 128, 128, TFT_BLACK);
    if ( pos == 1 ) {
      if ( player2.Field[0] == "empty" ) displayEmpty();
      else if ( player2.Field[0] == "corn") displayCorn();
      else if ( player2.Field[0] == "carrot") displayCarrot();
      else if ( player2.Field[0] == "tomato") displayTomato();
      else if ( player2.Field[0] == "lettuce") displayLettuce();
      else if ( player2.Field[0] == "blueberry") displayBlueberry();
    } else if ( pos == 2 ) {
      if ( player2.Field[1] == "empty" ) displayEmpty();
      else if ( player2.Field[1] == "corn") displayCorn();
      else if ( player2.Field[1] == "carrot") displayCarrot();
      else if ( player2.Field[1] == "tomato") displayTomato();
      else if ( player2.Field[1] == "lettuce") displayLettuce();
      else if ( player2.Field[1] == "blueberry") displayBlueberry();
    } else if ( pos == 3 ) {
      if ( player2.Field[2] == "empty" ) displayEmpty();
      else if ( player2.Field[2] == "corn") displayCorn();
      else if ( player2.Field[2] == "carrot") displayCarrot();
      else if ( player2.Field[2] == "tomato") displayTomato();
      else if ( player2.Field[2] == "lettuce") displayLettuce();
      else if ( player2.Field[2] == "blueberry") displayBlueberry();
    } else if ( pos == 4 ) {
      if ( player2.Field[3] == "empty" ) displayEmpty();
      else if ( player2.Field[3] == "corn") displayCorn();
      else if ( player2.Field[3] == "carrot") displayCarrot();
      else if ( player2.Field[3] == "tomato") displayTomato();
      else if ( player2.Field[3] == "lettuce") displayLettuce();
      else if ( player2.Field[3] == "blueberry") displayBlueberry();
    }

    if ( player2.FieldGrow[pos - 1] == -1) {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
    } else {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
      tft.setTextDatum(MC_DATUM);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(3);
      tft.drawString(String(player2.FieldGrow[pos - 1]), tft.width() / 2, tft.height() / 2 - 75);
      tft.setTextDatum(TL_DATUM);
    }

  } else if (player3Active) {
    tft.setRotation(0);
    tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
    tft.drawSmoothArc(tft.width() / 2, tft.height() / 2, 120, 100, 0, 360, TFT_GREEN, TFT_BLACK, true);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    // Print a message on the screen
    tft.drawString("Score", tft.width() / 2, 193);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(String(player3.points), tft.width() / 2, 210);
    tft.setTextDatum(TL_DATUM);

    tft.fillRect(56, 56, 128, 128, TFT_BLACK);
    if ( pos == 1 ) {
      if ( player3.Field[0] == "empty" ) displayEmpty();
      else if ( player3.Field[0] == "corn") displayCorn();
      else if ( player3.Field[0] == "carrot") displayCarrot();
      else if ( player3.Field[0] == "tomato") displayTomato();
      else if ( player3.Field[0] == "lettuce") displayLettuce();
      else if ( player3.Field[0] == "blueberry") displayBlueberry();
    } else if ( pos == 2 ) {
      if ( player3.Field[1] == "empty" ) displayEmpty();
      else if ( player3.Field[1] == "corn") displayCorn();
      else if ( player3.Field[1] == "carrot") displayCarrot();
      else if ( player3.Field[1] == "tomato") displayTomato();
      else if ( player3.Field[1] == "lettuce") displayLettuce();
      else if ( player3.Field[1] == "blueberry") displayBlueberry();
    } else if ( pos == 3 ) {
      if ( player3.Field[2] == "empty" ) displayEmpty();
      else if ( player3.Field[2] == "corn") displayCorn();
      else if ( player3.Field[2] == "carrot") displayCarrot();
      else if ( player3.Field[2] == "tomato") displayTomato();
      else if ( player3.Field[2] == "lettuce") displayLettuce();
      else if ( player3.Field[2] == "blueberry") displayBlueberry();
    } else if ( pos == 4 ) {
      if ( player3.Field[3] == "empty" ) displayEmpty();
      else if ( player3.Field[3] == "corn") displayCorn();
      else if ( player3.Field[3] == "carrot") displayCarrot();
      else if ( player3.Field[3] == "tomato") displayTomato();
      else if ( player3.Field[3] == "lettuce") displayLettuce();
      else if ( player3.Field[3] == "blueberry") displayBlueberry();
    }

    if ( player3.FieldGrow[pos - 1] == -1) {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
    } else {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
      tft.setTextDatum(MC_DATUM);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(3);
      tft.drawString(String(player3.FieldGrow[pos - 1]), tft.width() / 2, tft.height() / 2 - 75);
      tft.setTextDatum(TL_DATUM);
    }

  } else if (player4Active) {
    tft.setRotation(1);
    tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
    tft.drawSmoothArc(tft.width() / 2, tft.height() / 2, 120, 100, 0, 360, TFT_BLUE, TFT_BLACK, true);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    // Print a message on the screen
    tft.drawString("Score", tft.width() / 2, 193);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.drawString(String(player4.points), tft.width() / 2, 210);
    tft.setTextDatum(TL_DATUM);

    tft.fillRect(56, 56, 128, 128, TFT_BLACK);
    if ( pos == 1 ) {
      if ( player4.Field[0] == "empty" ) displayEmpty();
      else if ( player4.Field[0] == "corn") displayCorn();
      else if ( player4.Field[0] == "carrot") displayCarrot();
      else if ( player4.Field[0] == "tomato") displayTomato();
      else if ( player4.Field[0] == "lettuce") displayLettuce();
      else if ( player4.Field[0] == "blueberry") displayBlueberry();
    } else if ( pos == 2 ) {
      if ( player4.Field[1] == "empty" ) displayEmpty();
      else if ( player4.Field[1] == "corn") displayCorn();
      else if ( player4.Field[1] == "carrot") displayCarrot();
      else if ( player4.Field[1] == "tomato") displayTomato();
      else if ( player4.Field[1] == "lettuce") displayLettuce();
      else if ( player4.Field[1] == "blueberry") displayBlueberry();
    } else if ( pos == 3 ) {
      if ( player4.Field[2] == "empty" ) displayEmpty();
      else if ( player4.Field[2] == "corn") displayCorn();
      else if ( player4.Field[2] == "carrot") displayCarrot();
      else if ( player4.Field[2] == "tomato") displayTomato();
      else if ( player4.Field[2] == "lettuce") displayLettuce();
      else if ( player4.Field[2] == "blueberry") displayBlueberry();
    } else if ( pos == 4 ) {
      if ( player4.Field[3] == "empty" ) displayEmpty();
      else if ( player4.Field[3] == "corn") displayCorn();
      else if ( player4.Field[3] == "carrot") displayCarrot();
      else if ( player4.Field[3] == "tomato") displayTomato();
      else if ( player4.Field[3] == "lettuce") displayLettuce();
      else if ( player4.Field[3] == "blueberry") displayBlueberry();
    }

    if ( player4.FieldGrow[pos - 1] == -1) {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
    } else {
      tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
      tft.setTextDatum(MC_DATUM);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(3);
      tft.drawString(String(player4.FieldGrow[pos - 1]), tft.width() / 2, tft.height() / 2 - 75);
      tft.setTextDatum(TL_DATUM);
    }
  }
}

void displayColors() {
  for ( int i = 0; i < 4; i++) {
    if (player1.Field[i] == "corn")
      pixels.setPixelColor(i + 1, 255, 255, 0); //YELLOW
    else if (player1.Field[i] == "carrot")
      pixels.setPixelColor(i + 1, 255, 165, 0); //ORANGE
    else if (player1.Field[i] == "tomato")
      pixels.setPixelColor(i + 1, 255, 0, 0); //RED
    else if (player1.Field[i] == "lettuce")
      pixels.setPixelColor(i + 1, 0, 255, 0); //GREEN
    else if (player1.Field[i] == "blueberry")
      pixels.setPixelColor(i + 1, 0, 0, 255); //BLUE
    else if (player1.Field[i] == "empty")
      pixels.setPixelColor(i + 1, 0, 0, 0); //OFF
  }
  for ( int i = 0; i < 4; i++) {
    if (player2.Field[i] == "corn")
      pixels.setPixelColor(i + 5, 255, 255, 0); //YELLOW
    else if (player2.Field[i] == "carrot")
      pixels.setPixelColor(i + 5, 255, 165, 0); //ORANGE
    else if (player2.Field[i] == "tomato")
      pixels.setPixelColor(i + 5, 255, 0, 0); //RED
    else if (player2.Field[i] == "lettuce")
      pixels.setPixelColor(i + 5, 0, 255, 0); //GREEN
    else if (player2.Field[i] == "blueberry")
      pixels.setPixelColor(i + 5, 0, 0, 255); //BLUE
    else if (player2.Field[i] == "empty")
      pixels.setPixelColor(i + 5, 0, 0, 0); //OFF
  }
  for ( int i = 0; i < 4; i++) {
    if (player3.Field[i] == "corn")
      pixels.setPixelColor(i + 9, 255, 255, 0); //YELLOW
    else if (player3.Field[i] == "carrot")
      pixels.setPixelColor(i + 9, 255, 165, 0); //ORANGE
    else if (player3.Field[i] == "tomato")
      pixels.setPixelColor(i + 9, 255, 0, 0); //RED
    else if (player3.Field[i] == "lettuce")
      pixels.setPixelColor(i + 9, 0, 255, 0); //GREEN
    else if (player3.Field[i] == "blueberry")
      pixels.setPixelColor(i + 9, 0, 0, 255); //BLUE
    else if (player3.Field[i] == "empty")
      pixels.setPixelColor(i + 9, 0, 0, 0); //OFF
  }
  for ( int i = 0; i < 4; i++) {
    if (player4.Field[i] == "corn")
      pixels.setPixelColor(i + 13, 255, 255, 0); //YELLOW
    else if (player4.Field[i] == "carrot")
      pixels.setPixelColor(i + 13, 255, 165, 0); //ORANGE
    else if (player4.Field[i] == "tomato")
      pixels.setPixelColor(i + 13, 255, 0, 0); //RED
    else if (player4.Field[i] == "lettuce")
      pixels.setPixelColor(i + 13, 0, 255, 0); //GREEN
    else if (player4.Field[i] == "blueberry")
      pixels.setPixelColor(i + 13, 0, 0, 255); //BLUE
    else if (player4.Field[i] == "empty")
      pixels.setPixelColor(i + 13, 0, 0, 0); //OFF
  }
  pixels.show();   // Send the updated pixel colors to the hardware.
}

void displayEmpty() {

  if (player1Active) {
    tft.setRotation(2);
    tft.drawSmoothArc(tft.width() / 2, tft.height() / 2, 120, 100, 0, 360, TFT_RED, TFT_BLACK, true);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextDatum(MC_DATUM);
    // Print a message on the screen
    tft.drawString("EMPTY", tft.width() / 2, tft.height() / 2);
    tft.setTextDatum(TL_DATUM);
  } else if (player2Active) {
    tft.setRotation(3);
    tft.drawSmoothArc(tft.width() / 2, tft.height() / 2, 120, 100, 0, 360, TFT_YELLOW, TFT_BLACK, true);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextDatum(MC_DATUM);
    // Print a message on the screen
    tft.drawString("EMPTY", tft.width() / 2, tft.height() / 2);
    tft.setTextDatum(TL_DATUM);
  } else if (player3Active) {
    tft.setRotation(0);
    tft.drawSmoothArc(tft.width() / 2, tft.height() / 2, 120, 100, 0, 360, TFT_GREEN, TFT_BLACK, true);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextDatum(MC_DATUM);
    // Print a message on the screen
    tft.drawString("EMPTY", tft.width() / 2, tft.height() / 2);
    tft.setTextDatum(TL_DATUM);
  } else if (player4Active) {
    tft.setRotation(1);
    tft.drawSmoothArc(tft.width() / 2, tft.height() / 2, 120, 100, 0, 360, TFT_BLUE, TFT_BLACK, true);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextDatum(MC_DATUM);
    // Print a message on the screen
    tft.drawString("EMPTY", tft.width() / 2, tft.height() / 2);
    tft.setTextDatum(TL_DATUM);
  }
}

void gameEnded() {

  if (player1TurnCounter >= NUMBER_OF_TURNS) {

    display1.clearDisplay();
    display1.setTextSize(2);
    display1.setCursor(23, 8);
    display1.print("Player 1");
    display1.setCursor(32, 28);
    display1.print("score:");
    display1.setCursor(57, 48);
    display1.print(player1.points);
    display1.display();
    
    /*
        display1.clearDisplay();
        display1.setTextSize(2);
        display1.setTextColor(WHITE);
        display1.setCursor(0, 5);
        display1.print("Player 1 score: ");
        display1.print(player1.points);
        //display1.drawBitmap(54, 40, confirmIcon, 20, 20, WHITE);
        display1.display();
    */
    players[0] = false;
  }

  if (player2TurnCounter >= NUMBER_OF_TURNS) {
    display2.clearDisplay();
    display2.setTextSize(2);
    display2.setCursor(23, 8);
    display2.print("Player 2");
    display2.setCursor(32, 28);
    display2.print("score:");
    display2.setCursor(57, 48);
    display2.print(player2.points);
    display2.display();

    players[1] = false;
  }

  if (player3TurnCounter >= NUMBER_OF_TURNS) {
    display3.clearDisplay();
    display3.setTextSize(2);
    display3.setCursor(23, 8);
    display3.print("Player 3");
    display3.setCursor(32, 28);
    display3.print("score:");
    display3.setCursor(57, 48);
    display3.print(player3.points);
    display3.display();

    players[2] = false;
  }

  if (player4TurnCounter >= NUMBER_OF_TURNS) {

    display4.clearDisplay();
    display4.setTextSize(2);
    display4.setCursor(23, 8);
    display4.print("Player 4");
    display4.setCursor(32, 28);
    display4.print("score:");
    display4.setCursor(57, 48);
    display4.print(player4.points);
    display4.display();

    players[3] = false;
  }

  //number of players
  uint8_t var = 0;
  for ( int i = 0; i < 4; i++) {
    if (players[i]) var++;
  }
  if (var == 0) state = 7; // Svi igrači su završili
  else state = 4; //Weather Phase

}

void playerWarning() {

  display1.clearDisplay();
  display1.setTextSize(2);
  display1.setTextColor(WHITE);
  display1.setCursor(0, 5);
  display1.println("Not enough players");
  display1.drawBitmap(54, 40, confirmIcon, 20, 20, WHITE);
  display1.display();

  display2.clearDisplay();
  display2.setTextSize(2);
  display2.setTextColor(WHITE);
  display2.setCursor(0, 5);
  display2.println("Not enough players");
  display2.drawBitmap(54, 40, confirmIcon, 20, 20, WHITE);
  display2.display();

  display3.clearDisplay();
  display3.setTextSize(2);
  display3.setTextColor(WHITE);
  display3.setCursor(0, 5);
  display3.println("Not enough players");
  display3.drawBitmap(54, 40, confirmIcon, 20, 20, WHITE);
  display3.display();

  display4.clearDisplay();
  display4.setTextSize(2);
  display4.setTextColor(WHITE);
  display4.setCursor(0, 5);
  display4.println("Not enough players");
  display4.drawBitmap(54, 40, confirmIcon, 20, 20, WHITE);
  display4.display();

  if (P1Buttons[1]) {
    //Confirm
    P1Buttons[1] = false;
    state0Started = false;
    state0Counter = 0;
    state = 0;
  } else if (P2Buttons[1]) {
    //Confirm
    P2Buttons[1] = false;
    state0Started = false;
    state0Counter = 0;
    state = 0;
  } else if (P3Buttons[1]) {
    //Confirm
    P3Buttons[1] = false;
    state0Started = false;
    state0Counter = 0;
    state = 0;
  } else if (P4Buttons[1]) {
    //Confirm
    P4Buttons[1] = false;
    state0Started = false;
    state0Counter = 0;
    state = 0;
  }
  for ( int i = 0; i < 4; i++) {
    players[i] = false;
  }
}

void allPlayersDone() {

  uint8_t winner[] = {0, 0, 0, 0};
  winner[0] = player1.points;
  winner[1] = player2.points;
  winner[2] = player3.points;
  winner[3] = player4.points;

  int n = sizeof(winner) / sizeof(winner[0]);
  int res = winner[0];

  for (int i = 0; i < n; i++) {
    if (res < winner[i])
      res = winner[i];
  }

  if (player1.points == res) {
    tft.setRotation(2);
    tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
    tft.drawSmoothArc(tft.width() / 2, tft.height() / 2, 120, 100, 0, 360, TFT_RED, TFT_BLACK, true);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextDatum(MC_DATUM);
    // Print a message on the screen
    tft.drawString("WINNER", tft.width() / 2, tft.height() / 2 - 40);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString(String(player1.points), tft.width() / 2, tft.height() / 2 + 10);
    tft.setTextDatum(TL_DATUM);
  } else if (player2.points == res) {
    tft.setRotation(3);
    tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
    tft.drawSmoothArc(tft.width() / 2, tft.height() / 2, 120, 100, 0, 360, TFT_YELLOW, TFT_BLACK, true);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextDatum(MC_DATUM);
    // Print a message on the screen
    tft.drawString("WINNER", tft.width() / 2, tft.height() / 2 - 40);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString(String(player2.points), tft.width() / 2, tft.height() / 2 + 10);
    tft.setTextDatum(TL_DATUM);
  } else if (player3.points == res) {
    tft.setRotation(0);
    tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
    tft.drawSmoothArc(tft.width() / 2, tft.height() / 2, 120, 100, 0, 360, TFT_GREEN, TFT_BLACK, true);
    tft.setTextColor(TFT_WHITE);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextDatum(MC_DATUM);
    // Print a message on the screen
    tft.drawString("WINNER", tft.width() / 2, tft.height() / 2 - 40);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(String(player3.points), tft.width() / 2, tft.height() / 2 + 10);
    tft.setTextDatum(TL_DATUM);
  } else if (player4.points == res) {
    tft.setRotation(1);
    tft.fillRect(tft.width() / 2 - 24, tft.height() / 2 - 87, 50, 22, TFT_BLACK);
    tft.drawSmoothArc(tft.width() / 2, tft.height() / 2, 120, 100, 0, 360, TFT_BLUE, TFT_BLACK, true);
    tft.setTextColor(TFT_WHITE);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextDatum(MC_DATUM);
    // Print a message on the screen
    tft.drawString("WINNER", tft.width() / 2, tft.height() / 2 - 40);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.drawString(String(player4.points), tft.width() / 2, tft.height() / 2 + 10);
    tft.setTextDatum(TL_DATUM);
  }
  delay(1000);
}

String generateWeather() {
  int16_t rc;
  uint8_t weatherGen = random(3);
  String weather;

  if (weatherGen == 0) {
    rc = png.openFLASH((uint8_t *)sun, sizeof(sun), pngDraw);
    weather = "sunny";
  }
  else if (weatherGen == 1) {
    rc = png.openFLASH((uint8_t *)rain, sizeof(rain), pngDraw);
    weather = "rain";
  }
  else if (weatherGen == 2) {
    rc = png.openFLASH((uint8_t *)drought, sizeof(drought), pngDraw);
    weather = "drought";
  }

  if (rc == PNG_SUCCESS) {
    tft.startWrite();
    rc = png.decode(NULL, 0);
    tft.endWrite();
  }
  return weather;
}

void displayCorn() {
  int16_t rc;

  rc = png.openFLASH((uint8_t *)corn, sizeof(corn), pngDraw);

  if (rc == PNG_SUCCESS) {
    tft.startWrite();
    rc = png.decode(NULL, 0);
    tft.endWrite();
  }
}

void displayCarrot() {
  int16_t rc;

  rc = png.openFLASH((uint8_t *)carrot, sizeof(carrot), pngDraw);

  if (rc == PNG_SUCCESS) {
    tft.startWrite();
    rc = png.decode(NULL, 0);
    tft.endWrite();
  }
}

void displayTomato() {
  int16_t rc;

  rc = png.openFLASH((uint8_t *)tomato, sizeof(tomato), pngDraw);

  if (rc == PNG_SUCCESS) {
    tft.startWrite();
    rc = png.decode(NULL, 0);
    tft.endWrite();
  }
}

void displayLettuce() {
  int16_t rc;

  rc = png.openFLASH((uint8_t *)lettuce, sizeof(lettuce), pngDraw);

  if (rc == PNG_SUCCESS) {
    tft.startWrite();
    rc = png.decode(NULL, 0);
    tft.endWrite();
  }
}

void displayBlueberry() {
  int16_t rc;

  rc = png.openFLASH((uint8_t *)blueberry, sizeof(blueberry), pngDraw);

  if (rc == PNG_SUCCESS) {
    tft.startWrite();
    rc = png.decode(NULL, 0);
    tft.endWrite();
  }
}

void pngDraw(PNGDRAW * pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(56, 56 + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}

void loop() {

  //Player confirm if they want to play
  if (state == 0) state0();
  else if (state == 4) weatherPhase();
  else if (state == 1) harvestPhase();
  else if (state == 2) plantPhase();
  else if (state == 3) crops();
  else if (state == 5) gameEnded();
  else if (state == 6) playerWarning();
  else if (state == 7) allPlayersDone();

}
