#include <Wire.h>
#include <SFE_MicroOLED.h>
#include <TimerOne.h>
#include <EEPROM.h>

#define PIN_RESET 9
#define DC_JUMPER 1
#define SEC 1000
#define MAX 900
#define MIN 600
#define NAME 0
#define LIQUID1 1
#define LIQUID2 3
#define LIQUID3 5
#define l1OZ 2
#define l2OZ 4
#define l3OZ 6
#define SELECT 0
#define MOD 1
#define NEW 2
#define LEFT 1
#define RIGHT 1023
#define UP 1023
#define DOWN 3

MicroOLED oled(PIN_RESET, DC_JUMPER);

//==================================================================
//==================================================================
//==========================DEFINITIONS=============================
//==================================================================
//==================================================================

const byte numDrinks = 6;
const byte numInfo = 7;
String drinks[numDrinks][numInfo] = { 
    {"drink1", "1", "2", "3", "2", "0", "0"},
    {"drink2", "1", "4", "1", "1", "7", "3"},
    {"drink3", "3", "4", "2", "1", "2", "2"},
    {"drink4", "0", "0", "1", "3", "4", "1"},
    {"drink5", "0", "0", "3", "2", "4", "2"},
    {"drink6", "4", "2", "9", "2", "5", "3"}
  };
const byte none = 0;
const byte whiskey = 1;
const byte vodka = 2;
const byte tequila = 3;
const byte beer = 4;
const byte rum = 5;
const byte gin = 6;
const byte sake = 7;
const byte water = 8;
const byte redbull = 9;
const byte bPinRight = 3;
const byte bPinTop = 4;
const byte bPinBot = 5;
const byte bPinLeft = 6;
const byte pin1 = 0; // x value
const byte pin2 = 1; // y value

//==================================================================
//==================================================================
//===============================MAIN===============================
//==================================================================
//==================================================================

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
  delay(SEC);
  //initializeEEPROMData();
  //readDrinkData();
  Serial.print("\n\nSetup Complete\n\n");
}
//------------------------------------------------------------------
void loop() {
  int ms;
  
  printMenu();
  ms = menuSelect();
  
  switch(ms) {
    case SELECT:
      drinkSelect();
      break;
  }
}

//==================================================================
//==================================================================
//=============================FUNCTIONS============================
//==================================================================
//==================================================================

//------------------------------------------------------------------
/*
 * Print a menu on screen
*/
void printMenu() {
  oled.setFontType(0);
  oled.clear(PAGE);
  oled.setFontType(0);
  oled.setCursor(0,0);
  oled.print("Sel(BTop)\nMod(BL)\nNew(BR)");
  oled.display();
}

//------------------------------------------------------------------
/*
 * Controller for the menu.
 */
int menuSelect() {
  oled.setFontType(0);
  for (;;){ /* wait for input */
    if (digitalRead(bPinLeft) == LOW) { /* Mod a Drink */
      oled.clear(PAGE);
      oled.setCursor(5,10);
      oled.print("Mod a");
      oled.setCursor(14,25);
      oled.print("drink");
      oled.display();
      delay(SEC);
      return MOD;
    } else if (digitalRead(bPinTop) == LOW) {  /* Select a Drink */
      oled.clear(PAGE);
      oled.setCursor(5,10);
      oled.print("Select a");
      oled.setCursor(14,25);
      oled.print("drink");
      oled.display();
      delay(SEC);
      return SELECT;
    } else if (digitalRead(bPinRight) == LOW) {  /* Create a new Drink */
      oled.clear(PAGE);
      oled.setCursor(5,10);
      oled.print("Create a");
      oled.setCursor(14,25);
      oled.print("new drink");
      oled.display();
      delay(SEC);
      return NEW;
    }
  }
}

//------------------------------------------------------------------
void drinkSelect(){
  int x;
  int y;
  int drink = 0;

  Serial.print("\n\nentered drink selection\n\n");
  
  oled.setFontType(0);
  oled.setCursor(0,0);
  Serial.println(drinks[drink][NAME]);
  printDrinkData(drink, NAME);
  for (;;) {
    x = xjoyStick();
    
    if (x != -1) {
      if (x == LEFT) {
        while (xjoyStick() == LEFT) {;} // do nothing
        drink--;
      } else /* x == RIGHT */ {
        while (xjoyStick() == RIGHT) {;} // do nothing
        drink++;
      }
        
      if (drink < 0) drink = numDrinks-1;
      if (drink >= numDrinks) drink = 0;
      oled.setCursor(0,0);
      Serial.println(drinks[drink][NAME]);
      printDrinkData(drink, NAME);
    }
  }
}

//------------------------------------------------------------------
/*
 * Print a spcific piece of information (column) about a drink.
 * Requires the cursor and font to be set beforehand.
 * 
 * example:
 * printDrinkData(4, NAME);
 */
void printDrinkData(const int drink, const int column) {
  String info = drinks[drink][column];
  oled.clear(PAGE);
  oled.print(info);
  oled.display();
}

//------------------------------------------------------------------
int xjoyStick() {
  int x = analogRead(pin1);
  if ( x == RIGHT || x == LEFT ) {
    Serial.println("Valid");
    Serial.println(x);
    return x;
  } else 
      return -1;
}

//------------------------------------------------------------------
int yjoyStick() {
  int y = analogRead(pin2);
  if ( (y <= MAX && y >= MIN) && (y != UP && y != DOWN) )
    return -1;
  else
    return y;
}

//------------------------------------------------------------------
void selectDrink() {
  int selectDrinker = 0;
  // Use array 0-6, use loop to go through 0-6 with joystick, at 7 go back to 0
  /*read joystick*/ 
     //left drinks[drinkselection--][NAME]
     //right drinks[drinkseleection++][NAME]
  
}

//------------------------------------------------------------------
void modDrink() {
  
}

//------------------------------------------------------------------
void newDrink() {

}

//==================================================================
//==================================================================
//==================================================================

//------------------------------------------------------------------
/*
 * Initializes the data in eeprom
*/
void initializeEEPROMData() {
  Serial.print("\n\nInitializing\n\n");
  String tdrinks[numDrinks][numInfo] = { 
    {"drink1", "1", "2", "3", "2", "0", "0"},
    {"drink2", "1", "4", "1", "1", "7", "3"},
    {"drink3", "3", "4", "2", "1", "2", "2"},
    {"drink4", "0", "0", "1", "3", "4", "1"},
    {"drink5", "0", "0", "3", "2", "4", "2"},
    {"drink6", "4", "2", "9", "2", "5", "3"}
  };
  /*
  tdrinks[0][NAME] = "drink1";
  tdrinks[0][LIQUID1] = "1";
  tdrinks[0][l1OZ] = "2";
  tdrinks[0][LIQUID2] = "3";
  tdrinks[0][l2OZ] = "2";
  tdrinks[0][LIQUID3] = "0";
  tdrinks[0][l3OZ] = "0";
  */
  //drinks = tdrinks;
  char c;
  int address;
  int column;
  int drink;
  String info;

  /* Add to EEPROM */
  for (address = drink = column = 0; drink < numDrinks; address++) {
    /* Reached the end of a line */
    if (column == numInfo) {
      drink++;
      column = NAME;
      Serial.print("address=");
      Serial.println(address);
      Serial.print("New drink\ndrink=");
      Serial.println(drink);
      EEPROM.write(address, '\n');
      continue;
    }
    info = tdrinks[drink][column];
    Serial.print("info=");
    Serial.println(info);
    /* Add characters from info into EEPROM */
    for (int index = 0; ; index++, address++) {
      if (index == info.length()) {/* Reached the end of string */
        Serial.print("address=");
        Serial.println(address);
        Serial.println("done writing info");
        EEPROM.write(address, ' ');
        column++;
        break;
      }
      c = info.charAt(index);
      Serial.print("address=");
      Serial.println(address);
      Serial.print("let=\"");
      Serial.print(c);
      Serial.println('\"');
      EEPROM.write(address, c);
    }
  }
}

//------------------------------------------------------------------
/*
 * Read the drink data from eeprom
*/
void readDrinkData() {
  char c;
  int address;
  int column = 0;
  int drink = 0;
  String info;  // The data in each column
  
  for(address = 0; drink < numDrinks; address++) {
    c = EEPROM.read(address);
    Serial.print("let=");
    Serial.print(c);
    Serial.print(" @ address=");
    Serial.println(address);
    
    /* Reached the end of a line */
    if (c == '\n') {
      drink++;
      column = NAME;
      Serial.print("new drink\ndrink=");
      Serial.println(drink);
    }
    /* Reached the end of a string */
    if (c == ' ') {
      Serial.println("\nreached end of string");
      Serial.print("info=\"");
      Serial.print(info);
      Serial.println('\"');
      drinks[drink][column] = info;
      info.remove(0);
      column++;
      Serial.print("column=");
      Serial.println(column);
      Serial.println();
    } else /* Add to the current read letter to the info string */
      info += c;
  }
}

