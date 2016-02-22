#include "patterns.h"

double previousPattern1 = 0;
double previousPattern5 = 1;

/**
  Returns a random number. If a random number was already returned, the new
  generated number will be larger than previous number but not bigger than 1.
*/
double pattern1() {
  double current = uniformRandom(previousPattern1, min(previousPattern1 + 0.001, 1));
  previousPattern1 = current;

  return current;
}

/**
  Randomly returns 0 or 1
*/
double pattern2() {
  return random(0, 2);
}
/**
  Returns a random number using the language built-in uniform generator
*/
double pattern3() {
  return (double)random(2147483647L) / 2147483646;
}

/**
  Returns 0.5 constant
*/
double pattern4() {
  return 0.5;
}

/**
  Returns 1 for the first call, next calls will substract 0.1 from the previous
  returned value, after 0 is returned, it will start over.
*/

double pattern5() {
  double number = previousPattern5;
  previousPattern5 = previousPattern5 - 0.1 <= -0.01 ? 1 : previousPattern5 - 0.1;

  return abs(number);
}

double uniformRandom(double min, double max) {
  double interval = max - min;
  return pattern3() * interval + min;
}
