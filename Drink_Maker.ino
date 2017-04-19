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
const int bPinRight = 3;
const int bPinTop = 4;
const int bPinBot = 5;
const int bPinLeft = 6;

void setup() {
  oled.begin();
  oled.clear(ALL);
  oled.display();
  pinMode(bPinTop, INPUT_PULLUP);
  pinMode(bPinLeft, INPUT_PULLUP);
  pinMode(bPinRight, INPUT_PULLUP);
  pinMode(bPinBot, INPUT_PULLUP);
}

void loop() {
menu();
menuSelect();
}

void menu() {
  oled.clear(PAGE);
  oled.setFontType(0);
  oled.setCursor(0,0);
  oled.print("Sel(BTop)\nMod(BL)\nNew(BR)");
  oled.display();
}

void menuSelect() {
  for (;;){
    if (digitalRead(bPinLeft) == LOW) {
      oled.clear(PAGE);
      oled.print("Test1\n");
      oled.display();
      delay(SEC);
      break;
    } else if (digitalRead(bPinTop) == LOW){
      oled.clear(PAGE);
      oled.print("Test2\n");
      oled.display();
      delay(SEC);
      break;
    }else if (digitalRead(bPinRight) == LOW) {
      oled.clear(PAGE);
      oled.print("Test3\n");
      oled.display();
      delay(SEC);
      break;
    }else if (digitalRead(bPinBot) == LOW) {
      oled.clear(PAGE);
      oled.print("Test4\n");
      oled.display();
      delay(SEC);
      break;
    }
  }
}

void joyStick() {
}

