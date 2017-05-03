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
#define C1OZ 1
#define C2OZ 2
#define C3OZ 3

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
const byte numInfo = 4;
String drinks[numDrinks][numInfo] = { /* For initialization */
    {"drink1", "2.1", "2.3", "0"},
    {"drink2", "2", "2", "3"},
    {"drink3", "2", "1", "2"},
    {"drink4", "0", "1", "2"},
    {"drink5", "0", "2", "2"},
    {"drink6", "2", "1", "3"}
  };

/* don't need need these variables. only need 3 containers.
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
*/

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
  saveEEPROMData();
  //readDrinkData();
  Serial.print("\n\nSetup Complete\n\n");
}
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------
void loop() {
  int ms;
  int drink;
  
  printMenu();
  ms = menuSelect();

  
  switch(ms) {
    case SELECT:
      drinkSelect();
      break;
    case MOD:
      drink = drinkSelect();
      modDrink(drink);
      break;
    case NEW:
      newDrink();
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
      oled.print("Create");
      oled.setCursor(12,25);
      oled.print("new drink");
      oled.display();
      delay(SEC);
      return NEW;
    }
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------

//------------------------------------------------------------------
int drinkSelect(){
  int x;
  int drink = 0;

  Serial.print("\n\nEntered drink selection\n\n");
  
  oled.setFontType(0);
  oled.clear(PAGE);
  oled.setCursor(0,0);
  Serial.println(drinks[drink][NAME]);
  oled.print("Push BBot to sel\n\n");
  printDrinkData(drink, NAME);
  oled.display();
  for (;;) {
    x = xjoyStick();
    /* Drink is selected */
    if (digitalRead(bPinBot) == LOW) {
      Serial.println("drink selected");
      oled.clear(PAGE);
      oled.setCursor(0,0);
      oled.print("You sel\n\n");
      printDrinkData(drink, NAME);
      oled.display();
      delay(SEC);
      //makeDrink
      return drink;
    }
    
    if (x != -1) {
      if (x == LEFT) {
        while(xjoyStick() == LEFT) {;}  // do nothing
        drink--;
      } else /* x == RIGHT */ {
        while (xjoyStick() == RIGHT) {;} // do nothing
        drink++;
      }
        
      if (drink < 0) drink = numDrinks-1;
      if (drink >= numDrinks) drink = 0;
      oled.clear(PAGE);
      oled.setCursor(0,0);
      Serial.println(drinks[drink][NAME]);
      oled.print("Push BBot to sel\n\n");
      printDrinkData(drink, NAME);
      oled.display();
    }
  }
}

//------------------------------------------------------------------
void modDrink(int drink) {
  Serial.print("\n\nEntered mod drink\n\n");

  /*modCont(drink, C1);
  modOZ(drink, C1OZ);
  modCont(drink, C2);
  modOZ(drink, C2OZ);
  modCont(drink, C3);
  modOZ(drink, C3OZ);*/
  int x;
  int y;
  double oz = drinks[drink][C1OZ].toFloat();
  int cont = 1;

  oled.setFontType(0);
  oled.clear(PAGE);
  printHeader(drink);
  printCont(cont);
  printOZ(oz);
  oled.display();
  
  for (;;) {
    x = xjoyStick();
    y = yjoyStick();

    if(digitalRead(bPinBot) == LOW) {
      Serial.println("OZ selected");
      while (digitalRead(bPinBot) == LOW) {;}
      break;
    }

    if (y != -1) {
      if (y == DOWN) {
        while (yjoyStick() == DOWN) {;}
        cont--;
      } else /* y == Up */ {
        while (yjoyStick() == UP) {;}
        cont++;
      }
      if (cont < 1) cont = 3;
      if (cont > 3) cont = 1;

      oz = drinks[drink][cont].toFloat();
      oled.clear(PAGE);
      printHeader(drink);
      printCont(cont);
      printOZ(oz);
      oled.display();
    }
    
    if (x != -1) {
      if (x == LEFT) {
        while (xjoyStick() == LEFT) {;} // do nothing
        oz -= 0.5;
      } else /* x == RIGHT */ {
        while (xjoyStick() == RIGHT) {;} // do nothing
        oz += 0.5;
      }
        
      if (oz <= 0) oz = 10;
      if (oz > 10) oz = 0;

      drinks[drink][cont] = String(oz);
      oled.clear(PAGE);
      printHeader(drink);
      printCont(cont);
      printOZ(oz);
      oled.display();
    }
  }

  saveEEPROMData();
}

//------------------------------------------------------------------
void printOZ(double n) {
  oled.setFontType(1);
  oled.setCursor(10,30);
  oled.print(n);
  oled.setFontType(0);
}

//------------------------------------------------------------------
void printHeader(int drink) {
  oled.setCursor(4,0);
  oled.print("<");
  printDrinkData(drink, NAME);
  oled.println(">");
}

//------------------------------------------------------------------
void modCont(int drink, int CONT) {
  int x;
  int cont = drinks[drink][CONT].toInt();
  int temp = cont;

  oled.setFontType(0);
  oled.clear(PAGE);
  printHeader(drink);
  oled.print("Replace\ncntnr ");
  oled.println(temp);
  printCont(cont);
  oled.display();
  
  for (;;) {
    x = xjoyStick();

    if (digitalRead(bPinBot) == LOW) {
      while (digitalRead(bPinBot) == LOW) {;}
      Serial.println("Container selected");
      break;
    }
    
    if (x != -1) {
      if (x == LEFT) {
        while (xjoyStick() == LEFT) {;} // do nothing
        cont--;
      } else /* x == RIGHT */ {
        while (xjoyStick() == RIGHT) {;} // do nothing
        cont++;
      }
        
      if (cont < 1) cont = 3;
      if (cont > 3) cont = 1;

      oled.clear(PAGE);
      printHeader(drink);
      oled.print("Replace\ncntnr ");
      oled.println(temp);
      printCont(cont);
      oled.display();
    }
  }
}

//------------------------------------------------------------------
void printCont(int cont) {
  oled.setFontType(0);
  oled.setCursor(0,10);
  oled.print("Cntnr ");
  oled.print(cont);
}

//------------------------------------------------------------------
void newDrink() {
  // Check if there are 6 drinks
  // If there are 6 drinks, choose which one to replace
  int x;
  int y;
  int drink = 0;

  Serial.print("\n\nEntered drink selection\n\n");
  
  oled.setFontType(0);
  oled.setCursor(0,0);
  Serial.println(drinks[drink][NAME]);
  printDrinkData(drink, NAME);
  oled.display();
  for (;;) {
    x = xjoyStick();

      if(digitalRead(bPinBot) == LOW) {
      Serial.println("Drink selected");
      oled.clear(PAGE);
      oled.setCursor(0,0);
      oled.print("You sel\n\n");
      printDrinkData(drink, NAME);
      oled.display();
      delay(SEC);
      //new drink function
      break;
    }
    
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
      oled.display();
    }
  }
}

//-----------------------------------------------------------------
/*
 * Print a spcific piece of information (column) about a drink.
 * Requires the cursor and font to be set beforehand.
 * 
 * example:
 * printDrinkData(4, l1OZ);
 */
void printDrinkData(const int drink, const int column) {
  String info = drinks[drink][column];
  oled.print(info);
}

//------------------------------------------------------------------
int xjoyStick() {
  int x = analogRead(pin1);
  if ( x == RIGHT || x == LEFT ) {
    //Serial.println("Valid");
    //Serial.println(x);
    return x;
  } else 
      return -1;
}

//------------------------------------------------------------------
int yjoyStick() {
  int y = analogRead(pin2);
  if ( y == UP || y == DOWN ) {
    //Serial.println("Valid");
    //Serial.println(y);
    return y;
  } else
      return -1;
}

//==================================================================
//==================================================================
//==================================================================

//------------------------------------------------------------------
/*
 * Saves the data in drinks[][] to eeprom
*/
void saveEEPROMData() {
  Serial.print("\n\nInitializing\n\n");
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
      //Serial.print("address=");
      //Serial.println(address);
      //Serial.print("New drink\ndrink=");
      //Serial.println(drink);
      EEPROM.write(address, '\n');
      continue;
    }
    info = drinks[drink][column];
    //Serial.print("info=");
    //Serial.println(info);
    /* Add characters from info into EEPROM */
    for (int index = 0; ; index++, address++) {
      if (index == info.length()) {/* Reached the end of string */
        //Serial.print("address=");
        //Serial.println(address);
        //Serial.println("done writing info");
        EEPROM.write(address, ' ');
        column++;
        break;
      }
      c = info.charAt(index);
      //Serial.print("address=");
      //Serial.println(address);
      //Serial.print("let=\"");
      //Serial.print(c);
      //Serial.println('\"');
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
    Serial.print(" @ address=");
    Serial.println(address);
    
    /* Reached the end of a line */
    if (c == '\n') {
      drink++;
      column = NAME;
      Serial.print("new drink\ndrink=");
      Serial.println(drink);
      continue;
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

