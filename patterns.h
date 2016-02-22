#include "Arduino.h"

typedef double (* RNGPointer) ();
extern double previousPattern1;
extern double previousPattern5;

double uniformRandom(double min, double max);

double pattern1();
double pattern2();
double pattern3();
double pattern4();
double pattern5();
