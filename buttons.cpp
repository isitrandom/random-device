#include "buttons.h"

Button button1;
Button button2;

void checkButton(struct Button *button) {
  bool state = digitalRead(button->pin);

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

      if (button->maxClock > 600) {
        button->maxClock -= button->maxClock / 10;
      }
    } else {
      button->clock--;
      button->takeAction = false;
    }
  }
}

