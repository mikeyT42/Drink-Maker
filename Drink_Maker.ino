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
int address;
const int bPinRight = 3;
const int bPinTop = 4;
const int bPinBot = 5;
const int bPinLeft = 6;

void setup() {
  pinMode(bPinTop, INPUT_PULLUP);
  pinMode(bPinLeft, INPUT_PULLUP);
  pinMode(bPinRight, INPUT_PULLUP);
  pinMode(bPinBot, INPUT_PULLUP);
  oled.begin();
  oled.clear(ALL);
  oled.display();
  Serial.begin(9600);
  while(!Serial) {;}
  initializeEEPROMData();
  //readDrinkData();
}

void loop() {
  menu();
  menuSelect();
}

void initializeEEPROMData() {
  String tdrinks[numDrinks][numInfo] = { 
    {"drink1", "1", "2", "3", "2", "0", "0"},
    {"drink2", "1", "4", "1", "1", "7", "3"},
    {"drink3", "3", "4", "2", "1", "2", "2"},
    {"drink3", "0", "0", "1", "3", "4", "1"},
    {"drink4", "0", "0", "3", "2", "4", "2"},
    {"drink5", "4", "2", "9", "2", "5", "3"}
  };
  char c;
  int column = 0;
  int beginC = 0;
  int endC;
  int drink = 0;

  /* Add to EEPROM */
  for (address = 0; drink < numDrinks-1; address++) {
    /* Reached the end of a line */
    if (column > numInfo) {
      drink++;
      column = 0;
      Serial.print("address=");
      Serial.print(address);
      Serial.print("  drink=");
      Serial.println(drink);
      EEPROM.write(address, '\n');
    }
    String info = tdrinks[drink][column];
    Serial.println(info);
    /* Add characters from info into EEPROM */
    for (int index = 0; ; index++, address++) {
      if (index > info.length()-1) {
        Serial.print("address=");
        Serial.print(address);
        Serial.println("  done reading info");
        EEPROM.write(address, ' ');
        column++;
        break;
      }
      c = info.charAt(index);
      Serial.print("address=");
      Serial.print(address);
      Serial.print("  let=\"");
      Serial.print(c);
      Serial.println('\"');
      EEPROM.write(address, c);
    }
  }
}

void readDrinkData() {
  char c;
  int column = 0;
  int beginC = 0;
  int endC;
  int drink = 0;
  
  for(address = 0; drink < numDrinks; address++) {
    c = EEPROM.read(address);
    
    /* Reached the end of a line */
    if (c == '\n') drink++;
    
    /* Reached the end of a string */
    if (c == ' ') {
      String info;
      endC = address;
      for (int index = beginC; address < endC; index++) {
        c = EEPROM.read(index);
        info += c;
      }
      beginC = address;
      drinks[drink][column] = info;
      column++;
    }
  }
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
