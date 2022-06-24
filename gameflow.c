#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "menu.h"

uint8_t gameMode = 0;

void gf_doGame() {
	switch (gameMode) {
	case 0:
		gameMode = menu_showLegal();
		break;
	case 1:
		gameMode = menu_mainMenu();
		break;
	}
}