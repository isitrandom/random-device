#include "settings.h"

char* Device_Name = "Development Board";
char* Device_Screen_Color = "red";

/*
int button1Pin = 8;
int button2Pin = 9;
*/

int button1Pin = 6;
int button2Pin = 10;

int MENU_FRAME = 8000;

// button states
int Default_Max_Clock = 5000;

bool Read_Hardware_Buttons = false;
