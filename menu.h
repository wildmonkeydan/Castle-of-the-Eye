#pragma once
#include <stdint.h>

// Display the PSXDEV logos and SCEI discalimer
uint8_t menu_ShowLegal();

// Display and process the Main Menu
uint8_t menu_MainMenu();

// Clean up any menu backgrounds and data
void menu_CleanUp();