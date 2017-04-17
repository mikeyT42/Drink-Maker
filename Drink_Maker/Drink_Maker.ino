#include <Wire.h>
#include <SFE_MicroOLED.h>
#include <TimerOne.h>
#include <EEPROM.h>

#define PIN_RESET 9
#define DC_JUMPER 1
#define SEC 1000

MicroOLED oled(PIN_RESET, DC_JUMPER);

const int numDrinks = 6;
const int numInfo = 7;
String drinks[numDrinks][numInfo];
const int none = 0;
const int whiskey = 1;
const int vodka = 2;
const int tequila = 3;
const int beer = 4;
const int rum = 5;
const int gin = 6;
const int sake = 7;
const int water = 8;
const int redbull = 9;

void setup() {
  oled.begin();
  oled.clear(ALL);
  oled.display();
}

void loop() {
  oled.clear(PAGE);
  oled.setFontType(0);
  oled.setCursor(0,0);
  oled.print("Select a \nDrink");
  oled.display();
  delay(SEC);
  oled.clear(PAGE);
  oled.setCursor(0, 0);
  oled.setFontType(1);
}
