// initialize the library with the numbers of the interface pins
#include <LiquidCrystal.h>
#include "graphics.h"
#include "settings.h"
#include "patterns.h"
#include "buttons.h"
#include "types.h"

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int expectedNumbers = 0;
int remainingNumbers = 0;
int oldPercent = 0;

// serialData
char serialData[1];

char numberBuffer[20] = "";

// the shwon interface
bool displayNumber = true;
Interface interface = Intro;
Interface_States state = Init;
int flowerIndex[3] = {0, 0, 0};

// menu selection
const int maxGenerators = 6;
byte currentGenerator = 0;

// patterns
RNGPointer patterns[maxGenerators];

int introStep = 0;
int menuStep = 0;
int menuWait = MENU_FRAME;

void setup() {
  Serial.begin(9600);

  //patterns
  patterns[0] = pattern1;
  patterns[1] = pattern2;
  patterns[2] = pattern3;
  patterns[3] = pattern4;
  patterns[4] = pattern5;
  patterns[5] = pattern3;

  //buttons
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);

  button1.pin = button1Pin;
  button2.pin = button2Pin;

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
}

void readSerial() {
  if (remainingNumbers > 0) {
    button2.takeAction = 1;
    remainingNumbers--;
    return;
  }

  Serial.readBytes(serialData, 1);

  if (serialData[0] == 'a') //press button1
    button1.takeAction = 1;

  if (serialData[0] == 'b') //press button2
    button2.takeAction = 1;

  if (serialData[0] == 'c') { //turn off display
    displayNumber = 0;
  }

  if (serialData[0] == 'd') { //turn on display
    displayNumber = 1;
  }

  if (serialData[0] == 'p') { //select pattern
    Serial.readBytes(serialData, 1);

    if (serialData[0] == ':') {
      int pattern = Serial.parseInt() - 1;
      currentGenerator = pattern < maxGenerators ? pattern : 0;
      state = Init;
      showGenerator();
    }
  }

  if (serialData[0] == 't') { //take numbers
    Serial.readBytes(serialData, 1);

    if (serialData[0] == ':') {
      expectedNumbers = Serial.parseInt();
      remainingNumbers = expectedNumbers;

      lcd.setCursor(0, 1);
      lcd.print("                ");
    }
  }

  if (serialData[0] == 'x') { //send device info
    sendInfo();
  }
}

void sendInfo() {
  remainingNumbers = 0;

  char buffer[500];
  sprintf(buffer, "info:%s:%s:%d", Device_Name, Device_Screen_Color, currentGenerator);

  Serial.println(buffer);
}

void detectActions() {
  if (Serial.available() > 0) {
    readSerial();
  } else if (Read_Hardware_Buttons) {
    checkButton(&button1);
    checkButton(&button2);
  }

  // menu actions
  if (interface == Menu) {
    if (button1.takeAction && state == Idle) {
      state = Action;

      currentGenerator++;
      currentGenerator = currentGenerator < maxGenerators ? currentGenerator : 0;
      sendInfo();
    }

    if (button2.takeAction) {
      interface = Number;
      state = Init;
    }
  }

  // generator actions
  if (interface == Number) {
    if (button1.takeAction) {
      interface = Menu;
      state = Init;

      currentGenerator++;
      currentGenerator = currentGenerator < maxGenerators ? currentGenerator : 0;
      sendInfo();
    }

    if (button2.takeAction && state == Idle) {
      state = Action;
    }
  }

  button1.takeAction = 0;
  button2.takeAction = 0;
}

void loop() {
  detectActions();

  if (interface == Intro) {
    showIntro();
  }

  if (interface == Menu) {
    showMenu();
  }

  if (interface == Number) {
    showGenerator();
  }
}

void showIntro() {

  if (introStep == 0) {
    lcd.createChar(0, logo1);
    lcd.createChar(1, logo2);
    lcd.createChar(2, logo3);
    lcd.createChar(3, logo4);
  }

  if (introStep < 500 && introStep > 10) {
    //logo
    lcd.setCursor(7, 0);
    lcd.write((uint8_t)0);

    lcd.setCursor(8, 0);
    lcd.write((uint8_t)2);

    lcd.setCursor(7, 1);
    lcd.write((uint8_t)1);

    lcd.setCursor(8, 1);
    lcd.write((uint8_t)3);
  }

  if (introStep < 50 && introStep > 20) {
    if (flowerIndex[0] < 8) {
      flowerIndex[0]++;
    }

    if (flowerIndex[1] < 8 && flowerIndex[0] > 2) {
      flowerIndex[1]++;
    }
    if (flowerIndex[2] < 8 && flowerIndex[0] > 4) {
      flowerIndex[2]++;
    }

    lcd.createChar(4, flower[flowerIndex[0]]);
    lcd.createChar(5, flower[flowerIndex[1]]);
    lcd.createChar(6, flower[flowerIndex[2]]);

    //flowers
    lcd.setCursor(5, 1);
    lcd.write((uint8_t)4);

    lcd.setCursor(10, 1);
    lcd.write((uint8_t)4);

    if (flowerIndex[0] > 2) {
      lcd.setCursor(12, 1);
      lcd.write((uint8_t)5);

      lcd.setCursor(3, 1);
      lcd.write((uint8_t)5);
    }

    if (flowerIndex[0] > 4) {
      lcd.setCursor(1, 1);
      lcd.write((uint8_t)6);

      lcd.setCursor(14, 1);
      lcd.write((uint8_t)6);
    }
  }

  introStep++;
  delay(40);

  if (introStep >= 50) {
    interface = Menu;

    lcd.setCursor(0, 0);
    lcd.print("Hello!");

    delay(1500);

    lcd.setCursor(0, 0);
    lcd.print("Is it ");

    lcd.setCursor(9, 0);
    lcd.print("random?");

    delay(3000);
  }
}

void setupMenuGraphics() {
  int rnd = random(4);
  lcd.createChar(0, patternGraphic[rnd][0]);
  lcd.createChar(1, patternGraphic[rnd][1]);
  lcd.createChar(2, patternGraphic[rnd][2]);
  lcd.createChar(3, patternGraphic[rnd][3]);
  lcd.createChar(4, patternGraphic[rnd][4]);
  lcd.createChar(5, patternGraphic[rnd][5]);
  lcd.createChar(6, patternGraphic[rnd][6]);
  lcd.createChar(7, patternGraphic[rnd][7]);
}

void showMenu() {
  char buffer[20] = "";

  if (state == Init) {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Pattern");

    state = Action;

    setupMenuGraphics();
  }

  if (state == Action) {
    sprintf(buffer, "%d", currentGenerator + 1);

    lcd.setCursor(8, 0);
    lcd.print(buffer);

    setupMenuGraphics();
  }


  if (menuWait == 0) {
    menuStep++;

    if (menuStep >= 4) {
      menuStep = 0;
    }

    //display patterns on row 1
    for (int i = 0; i < 16; i++) {
      lcd.setCursor(i, 1);

      if (i % 2) {
        lcd.write((uint8_t) menuStep % 4);
      } else {
        lcd.write((uint8_t) 4 + menuStep % 4);
      }
    }

    menuWait = MENU_FRAME;
  } else {
    menuWait--;
  }

  state = Idle;
}

void showGenerator() {
  if (state == Init) {
    lcd.clear();
    lcd.noCursor();
    lcd.noBlink();

    lcd.setCursor(0, 0);
    char title[16] = "Pattern";
    sprintf(title, "%s %d", title, currentGenerator + 1);
    lcd.print(title);

    previousPattern1 = 0;
    previousPattern5 = 1;

    state = Action;
  }

  if (state == Action) {
    double number = patterns[currentGenerator]();

    dtostrf(number, 7, 5, numberBuffer);


    if (remainingNumbers > 0) {
      int percent = 17 - ((double)remainingNumbers / (double)expectedNumbers) * 17;

      if (percent != oldPercent) {
        lcd.createChar(0, fullGraphic);

        for (int i = 0; i < percent; i++) {
          lcd.setCursor(i, 1);
          lcd.print((char)0);
        }
      }

      oldPercent = percent;
    } else if (displayNumber) {
      lcd.setCursor(0, 1);
      lcd.print("                ");

      lcd.setCursor(0, 1);
      lcd.print(numberBuffer);
    }

    Serial.println(numberBuffer);
  }

  state = Idle;
}
