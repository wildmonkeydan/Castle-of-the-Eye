#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "menu.h"

uint8_t gameMode = 0;

void doGame() {
	switch (gameMode) {
	case 0:
		showLegal();
		break;
	case 1:
	}
}