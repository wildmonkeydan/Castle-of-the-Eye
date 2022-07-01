#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "menu.h"
#include "testSuite.h"

static uint8_t gameMode = 0;

void gf_doGame() {
	switch (gameMode) {
	case 0:
		gameMode = menu_ShowLegal();
		break;
	case 1:
		gameMode = menu_MainMenu();
		break;
	case 2:
		// Main Game
		break;
	case 3: 
		testS_Run();
		break;
	case 4:
		// Options
		break;
	}
}