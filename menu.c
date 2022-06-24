#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "ui.h"
#include "cd.h"
#include "colorLUT.h"

int stateCounter = 0;
SPRTDATA legalPage;

uint8_t menu_showLegal() {
	if (stateCounter == 0) {
		u_long* legal = cdLoad_File("\\LEGAL.TIM;1");
		TIM_IMAGE tim;
		ui_LoadTexture(legal, &tim);
		legalPage = ui_LoadSpriteData(&tim);
		legalPage.u = 0;
		legalPage.v = 0;
		legalPage.x = 32;
		legalPage.y = 0;
		legalPage.w = 255;
		legalPage.h = 255;
		free(legal);
		stateCounter++;
	}
	ui_DrawSprite(legalPage, norm);
	ui_SetTPage(legalPage);
	if (stateCounter > 300) {
		stateCounter = 0;
		return 1;
	}
	else{
		stateCounter++;
		return 0;
	}
}

uint8_t menu_mainMenu() {
	if (stateCounter == 0) {
		u_long* back = cdLoad_File("\\CASTLE.TIM;1");
	}
}