#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <libgte.h>
#include <libgpu.h>
#include "ui.h"
#include "display.h"
#include "cd.h"
#include "colorLUT.h"
#include "input.h"

int stateCounter = 0;
static SPRTDATA legalPage;
static TILEDATA point;
u_long* back;
uint8_t menuPos = 0;

uint8_t menu_ShowLegal() {
	if (stateCounter == 0) {
		u_long* legal = cdLoad_File("\\LEGAL.TIM;1");
		TIM_IMAGE tim;
		ui_LoadTexture(legal, &tim);
		legalPage = ui_LoadSpriteData(&tim);
		legalPage.u = 0;
		legalPage.v = 0;
		legalPage.x = 32;
		legalPage.y = -32;
		legalPage.w = 255;
		legalPage.h = 255;
		free(legal);
		u_long* font = cdLoad_File("\\TYPE.TIM;1");
		ui_LoadTexture(font, &tim);
		free(font);
		stateCounter++;
	}
	disp_DrawSprite(legalPage, norm);
	disp_SetDrawTPage(legalPage);
	if (stateCounter > 300) {
		stateCounter = 0;
		return 1;
	}
	else{
		stateCounter++;
		return 0;
	}
}

uint8_t menu_MainMenu() {
	if (stateCounter == 0) {
		back = cdLoad_File("\\CASTLE.TIM;1");
		cdMusic_Ready();
		cdMusic_PlayTrack(2);
		stateCounter++;
	}
	point.size.w = 8; point.size.h = 8;
	point.size.y = 150 + (menuPos * 8);
	point.size.x = 72;
	point.color[0] = 255; point.color[1] = 255; point.color[3] = 255;
	TIM_IMAGE tim;
	FntPrint("START GAME\nTEST SUITE\nOPTIONS");
	FntFlush(-1);
	disp_DrawTile(point);
	
	char input;
	if (stateCounter == 10) {
		 input = input_Menu();
		 stateCounter = 1;
	}

	if (input == 2) {
		return menuPos + 2;
	}
	else {
		if (menuPos >= 0 && menuPos <= 2) {
			menuPos += input;
		}
		stateCounter++;
		return 1;
	}
}

void menu_CleanUp() {
	free(back);
}