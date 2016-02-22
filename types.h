#ifndef TYPES_H
#define TYPES_H

#include "settings.h"

enum Interface {
  Menu,
  Number,
  Intro
};

enum Interface_States {
  Init,
  Idle,
  Action,
  ActionPerformed
};

struct Button {
  int pin;
  bool pressed;
  bool takeAction;
  int clock;
  int maxClock = Default_Max_Clock;
};

#endif
