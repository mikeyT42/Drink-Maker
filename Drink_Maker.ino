#include <Wire.h>
#include <SFE_MicroOLED.h>
#include <TimerOne.h>
#include <EEPROM.h>
//#include <Stepper.h>

//==================================================================
//==================================================================
//==========================DEFINITIONS=============================
//==================================================================
//==================================================================

#define PIN_RESET 9
#define DC_JUMPER 1

#define SEC 1000
#define MAX 900
#define MIN 600
#define MAXDRINKS 6
#define MAXCOLUMNS 4
#define EOF '#'

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

String drinks[MAXDRINKS][MAXCOLUMNS] = { /* For initialization */
    {"d", "2.5", "2.5", "0"},
    {"dr", "2.0", "2.0", "3.0"},
    {"dri", "2.0", "1.0", "2.0"},
    {"drin", "0", "1.0", "2.0"},
    {"drink", "0", "2.0", "2.0"},
    {"drink6", "2.0", "1.5", "3.0"}
  };
byte dsize = 6;

const int motor1 = 7;
const int motor2 = 8;
const int motor3 = 9;
const int SPR = 200;  // Steps per revolution
//Stepper someStepper(SPR, 10, 11, 12, 13);

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

  //someStepper.setSpeed(10);
  //pinMode(motor1, OUTPUT); 
  //pinMode(motor2, OUTPUT); 
  //pinMode(motor3, OUTPUT);
  
  Serial.begin(9600);
  while(!Serial) {;}
  delay(SEC);
  saveEEPROMData();
  //readDrinkData();
  Serial.print("\n\nSetup Complete\n\n");
}

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
  oled.clear(PAGE);
  oled.setFontType(0);
  oled.setCursor(0,0);
  oled.print("Select\n   BTop\nModify\n   BL\nNew\n   BR");
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
      oled.setCursor(13,10);
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
    } else if (digitalRead(bPinRight) == LOW) { /* Create a new Drink */
      oled.clear(PAGE);
      oled.setCursor(10,10);
      oled.print("Create");
      oled.setCursor(5,25);
      oled.print("new drink");
      oled.display();
      delay(SEC);
      return NEW;
    }
  }
}

//------------------------------------------------------------------
/*
 * This method gets the selection of a drink from a user.
 */
int drinkSelect(){
  int x;
  int drink = 0;

  //Serial.print("\n\nEntered drink selection\n\n");
  
  oled.setFontType(0);
  oled.clear(PAGE);
  oled.setCursor(0,0);
  //Serial.println(drinks[drink][NAME]);
  oled.print("Push BBot to select\n\n");
  printDrinkData(drink, NAME);
  oled.display();
  for (;;) {
    x = xjoyStick();
    /* Drink is selected */
    if (digitalRead(bPinBot) == LOW) {
      //Serial.println("drink selected");
      oled.clear(PAGE);
      oled.setCursor(0,0);
      oled.print("You\nselected\n\n");
      printDrinkData(drink, NAME);
      oled.display();
      delay(SEC);
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
        
      if (drink < 0) drink = dsize-1;
      if (drink >= dsize) drink = 0;
      oled.clear(PAGE);
      oled.setCursor(0,0);
      //Serial.println(drinks[drink][NAME]);
      oled.print("Push BBot to select\n\n");
      printDrinkData(drink, NAME);
      oled.display();
    }
  }
}

//-----------------------------------------------------------------------------------------------
/*
 * This method is the interface for the modifying a drink's ozs paramaters.
 */
void modDrink(int drink) {
  //Serial.print("\n\nEntered mod drink\n\n");

  drinks[drink][NAME] = chooseNAME();
  chooseOZ(drink);

  saveEEPROMData();
}

//----------------------------------------------------------------------------------------------
void chooseOZ(int drink) {
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
      //Serial.println("OZ selected");
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
      /* get the ozs of the associated container */
      oz = drinks[drink][cont].toFloat();
      /*------------*/
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
      /* set the oz of the associated container */
      drinks[drink][cont] = String(oz);
      oled.clear(PAGE);
      printHeader(drink);
      printCont(cont);
      printOZ(oz);
      oled.display();
    }
  }
}

//-----------------------------------------------------------------------------------------------
/*
 * print to OLED the ozs of a drink. used for editing and making of a drink.
 */
void printOZ(double n) {
  oled.setFontType(1);
  if (n == 10.0) 
    oled.setCursor(1,30);
  else
    oled.setCursor(10,30);
    
  oled.print(n);
  oled.setFontType(0);
  oled.setCursor(46,34);
  oled.print("oz");
}

//-----------------------------------------------------------------------------------------------
/*
 * print the header for modify and mod drink functions; 
 * ex:
 * (top line centered) <drink1>
 */
void printHeader(int drink) {
  switch(drinks[drink][NAME].length()) {
  case 1:
    oled.setCursor(22,0);
    oled.print("<");
    printDrinkData(drink, NAME);
    oled.println(">");
    break;
  case 2:
    oled.setCursor(19,0);
    oled.print("<");
    printDrinkData(drink, NAME);
    oled.println(">");
    break;
  case 3:
    oled.setCursor(16,0);
    oled.print("<");
    printDrinkData(drink, NAME);
    oled.println(">");
    break;
  case 4:
    oled.setCursor(13,0);
    oled.print("<");
    printDrinkData(drink, NAME);
    oled.println(">");
    break;
  case 5:
    oled.setCursor(8,0);
    oled.print("<");
    printDrinkData(drink, NAME);
    oled.println(">");
    break;
  case 6:
    oled.setCursor(5,0);
    oled.print("<");
    printDrinkData(drink, NAME);
    oled.println(">");
    break;
  case 7:
    oled.setCursor(3,0);
    oled.print("<");
    printDrinkData(drink, NAME);
    oled.println(">");
    break;
  }
}

//-----------------------------------------------------------------------------------------------
void printCont(int cont) {
  oled.setFontType(0);
  oled.setCursor(0,10);
  oled.print("Cntnr ");
  oled.print(cont);
}

//-----------------------------------------------------------------------------------------------
void newDrink() {
  /*-----Name the new drink-----*/
  String nm = chooseNAME();
  /*--------------------*/
  /* save drink name */
  //Serial.println(dsize);
  dsize++;
  int pos = saveDrinkName(nm, dsize);
  if (pos == -1) return;
  /*------------*/
  drinks[pos][C1OZ] = "0.0";
  drinks[pos][C2OZ] = "0.0";
  drinks[pos][C3OZ] = "0.0";
  chooseOZ(pos);

  oled.clear(PAGE);
  oled.setCursor(0,0);
  oled.print("Saving");
  oled.display();

  saveEEPROMData();
}

//-----------------------------------------------------------------------------------------------
int saveDrinkName(String nm, int pos) {
  if (dsize > MAXDRINKS) { // no more space in storage
    dsize--;
    int ans = 0;
    oled.clear(PAGE);
    oled.setCursor(0,0);
    oled.print("Not enough space");
    oled.display();
    delay(SEC);
    /* see if user wants to replace */
    oled.clear(PAGE);
    oled.setCursor(0,0);
    oled.print("Replace?");
    oled.setCursor(12,10);
    oled.print("no");
    oled.display();
    for (int y = 0;;) {
      if(digitalRead(bPinBot) == LOW) {
        //Serial.println("OZ selected");
        break;
      }
      
      y = yjoyStick();
      if (y != -1) {
        if (y == UP) {
          while (yjoyStick() == UP) {;}
          ans++;
        }
        if (ans > 1) ans = 0;
        /*------------*/
        oled.setCursor(12,10);
        if (ans == 1) {
          oled.clear(PAGE);
          oled.setCursor(0,0);
          oled.print("Replace?");
          oled.setCursor(12,10);
          oled.print("yes");
          oled.display();
        } else {
          oled.clear(PAGE);
          oled.setCursor(0,0);
          oled.print("Replace?");
          oled.setCursor(12,10);
          oled.print("no");
          oled.display();
        }
        oled.display();
      }
    }
    /*--------*/
    if (ans == 1) {
      oled.clear(PAGE);
      oled.setCursor(0,0);
      oled.print("Choose a\ndrink to \nreplace");
      oled.display();
      delay(SEC);
      pos = drinkSelect();
      drinks[pos][NAME] = nm;
      return pos;
    } else { // cancel
      oled.clear(PAGE);
      oled.setCursor(0,0);
      oled.print("Canceling");
      oled.display();
      delay(SEC);
      return -1;
    }
  } else { // there is still space to store
    drinks[dsize-1][NAME] = nm;
    return dsize-1;
  }
}
//-----------------------------------------------------------------------------------------------
String chooseNAME() {
  int x;
  int y;
  String dName;
  bool done = false;

  oled.setFontType(0);
  oled.setCursor(0,0);
  oled.clear(PAGE);
  oled.print("Enter yourdrink namein serial\nmonitor");
  oled.display();

  Serial.println("------------------------------------------------------------------\n");
  while (!done) {
    Serial.println("\nEnter drink name");
    while (!done) {
      while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\n') done = true; 
        dName += c;
      }
    }
    if (dName.length()-1 > 7) {
      Serial.print("You entered ");
      Serial.println(dName);
      Serial.println("The length of your name is to big, max is 7 characters long");
      dName.remove(0);
      done = false;
    }
  }
  Serial.print("\nYou entered ");
  Serial.println(dName);
  Serial.println("\n------------------------------------------------------------------\n\n");

  oled.clear(PAGE);
  oled.setCursor(0,0);
  oled.println("Input:");
  oled.print(dName);
  oled.display();
  delay(2*SEC);

  dName.trim();
  return dName;
}

//-----------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------
int xjoyStick() {
  int x = analogRead(pin1);
  if ( x == RIGHT || x == LEFT ) {
    //Serial.println("Valid");
    //Serial.println(x);
    return x;
  } else 
      return -1;
}

//-----------------------------------------------------------------------------------------------
int yjoyStick() {
  int y = analogRead(pin2);
  if ( y == UP || y == DOWN ) {
    //Serial.println("Valid");
    //Serial.println(y);
    return y;
  } else
      return -1;
}

//==============================================================================================
//=======================================Memory Functions=======================================
//==============================================================================================

//-----------------------------------------------------------------------------------------------
/*
 * Saves the data in drinks[][] to eeprom
*/
void saveEEPROMData() {
  Serial.print("\n\nSaving\n\n");
  char c;
  int address;
  int column;
  int drink;
  String info;

  /* Add to EEPROM */
  for (address = drink = column = 0; drink < dsize; address++) {
    /* Reached the end of a line */
    if (column == MAXCOLUMNS) {
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
  //Serial.print("address=");
  //Serial.println(address);
  //Serial.print("let=\"");
  //Serial.print(EOF);
  //Serial.println('\"');
  EEPROM.write(address, EOF);
}

//-----------------------------------------------------------------------------------------------
/*
 * Read the drink data from eeprom
*/
void readDrinkData() {
  Serial.print("\n\nReading\n\n");
  char c;
  int address;
  int column = 0;
  int drink = 0;
  String info;  // The data in each column
  dsize = 0;
  for(address = 0; drink < MAXDRINKS; address++) {
    c = EEPROM.read(address);
    //Serial.print("let=");
    //Serial.print(c);
    //Serial.print(" @ address=");
    //Serial.println(address);

    if (c == EOF) break;
    
    /* Reached the end of a line */
    if (c == '\n') {
      drink++;
      dsize++;
      column = NAME;
      //Serial.print("new drink\ndrink=");
      //Serial.println(drink);
      continue;
    }
    /* Reached the end of a string */
    if (c == ' ') {
      //Serial.println("\nreached end of string");
      //Serial.print("info=\"");
      //Serial.print(info);
      //Serial.println('\"');
      drinks[drink][column] = info;
      info.remove(0);
      column++;
      //Serial.print("column=");
      //Serial.println(column);
      //Serial.println();
    } else /* Add to the current read letter to the info string */
      info += c;
  }
}

//-----------------------------------------------------------------------------------------------
// makeDrink plays with the motors
void makeDrink() {
  Serial.println("Forward");
  //someStepper.step(10);
  delay(500);

  // Step backward 10 steps:
  Serial.println("Backward");
  //someStepper.step(-10);
  delay(500);

}

