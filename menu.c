#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "ui.h"
#include "cd.h"
#include "colorLUT.h"

int stateCounter = 0;
SPRTDATA legalPage;

void showLegal() {
	if (stateCounter == 0) {
		u_long* legal = cdLoad_File("\\LEGAL.TIM;1");
		TIM_IMAGE tim;
		ui_LoadTexture(legal, &tim);
		legalPage = ui_LoadSpriteData(&tim);
		free(legal);
		stateCounter++;
	}
	ui_DrawSprite(legalPage, norm);
}