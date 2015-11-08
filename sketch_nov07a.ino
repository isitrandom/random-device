// initialize the library with the numbers of the interface pins
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

typedef double (* RNGPointer) ();

enum Interface_States {
  Init,
  Idle,
  Action,
  ActionPerformed
};

const int button1Pin = A0;
const int button2Pin = A1;

// button states
const int Default_Max_Clock = 2500;

struct Button {
  int pin;
  bool pressed;
  bool takeAction;
  int clock;
  int maxClock = Default_Max_Clock;
};

Button button1;
Button button2;

// the shwon interface
bool isMenuShowing = false;
Interface_States state = Init;

// menu selection
const int maxGenerators = 6;
byte currentGenerator = 0;
byte menuSelection;

// patterns
RNGPointer patterns[maxGenerators];

void setup() {
  Serial.begin(9600);

  //patterns
  patterns[0] = pattern1;
  patterns[1] = pattern2;
  patterns[2] = pattern3;
  patterns[3] = pattern3;
  patterns[4] = pattern3;
  patterns[5] = pattern3;


  //buttons
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);

  button1.pin = button1Pin;
  button1.pressed = false;
  button1.takeAction = false;

  button2.pin = button2Pin;
  button2.pressed = false;
  button2.takeAction = false;

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  lcd.setCursor(5, 0);
  lcd.print("Hello!");

  lcd.setCursor(2, 1);
  lcd.print("Is it random?");
}

void checkButton(struct Button *button) {
  bool state = analogRead(button->pin) < 100;

  if (button->pressed != state) {
    button->pressed = state;
    button->takeAction = button->pressed;

    if (state) {
      button->clock = button->maxClock;
    } else {
      button->maxClock = Default_Max_Clock;
    }
  } else if (state) {
    if (button->clock == 0) {
      button->takeAction = true;
      button->clock = button->maxClock;

      if(button->maxClock > 600) {
        button->maxClock -= button->maxClock / 10;
      }
    } else {
      button->clock--;
      button->takeAction = false;
    }
  }

}

void detectActions() {
  checkButton(&button1);
  checkButton(&button2);

  // menu actions
  if (isMenuShowing) {
    if (button1.takeAction && state == Idle) {
      state = Action;

      menuSelection++;
      menuSelection = menuSelection < maxGenerators ? menuSelection : 0;
    }

    if (button2.takeAction) {
      currentGenerator = menuSelection;
      isMenuShowing = false;
      state = Init;
    }
  }

  // generator actions
  if (!isMenuShowing) {
    if (button1.takeAction) {
      isMenuShowing = true;
      state = Init;
    }

    if (button2.takeAction && state == Idle) {
      state = Action;
    }
  }

  if (!button1.takeAction && !button2.takeAction) {
    state = Idle;
  }
}

void loop() {

  detectActions();

  if (isMenuShowing) {
    showMenu();
  } else {
    showGenerator();
  }
}

void showMenu() {
  if (state == Init) {
    lcd.clear();
    lcd.cursor();
    lcd.blink();

    lcd.setCursor(0, 0);
    lcd.print("Pattern select:");

    char buffer[20] = "";
    for (int i = 1; i < maxGenerators + 1; i++) {
      sprintf(buffer, "%s %d", buffer, i);
    }

    lcd.print(buffer);

    lcd.setCursor(0, 1);
    lcd.print(buffer);

    menuSelection = currentGenerator;

    state = Action;
  }

  if (state == Action) {
    showMenuSelection('>', currentGenerator);
    showMenuSelection('<', currentGenerator+1);

    lcd.setCursor(menuSelection * 2 + 1, 1);

    state = ActionPerformed;
  }
}

void showMenuSelection(char c, byte position) {
  lcd.setCursor(position * 2, 1);
  lcd.print(c);
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

    state = Action;
  }

  if (state == Action || state == Init) {
    double number = patterns[currentGenerator]();
    char buffer[200];
    
    lcd.setCursor(0, 1);
    lcd.print("                ");

    lcd.setCursor(0, 1);
    
    dtostrf(number, 7, 5, buffer);

    lcd.print(buffer);    
    Serial.println(buffer);

    state = ActionPerformed;
  }
}

/**
  Returns a random number. If a random number was already returned, the new
  generated number will be larger than previous number but not bigger than 1.
*/
double pattern1() {
  return (double)random(2147483647L) / 2147483647;
}

double pattern2() {
  return 0.2f;
}

double pattern3() {
  return (double)random(2147483647L) / 2147483647;
}


