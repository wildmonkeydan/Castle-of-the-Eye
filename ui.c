#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <libgpu.h>
#include "display.h"
#include "ui.h"

void ui_LoadTexture(u_long* tim, TIM_IMAGE* tparam) {
	OpenTIM(tim);
	ReadTIM(tparam);

	// Upload pixel data to framebuffer
	LoadImage(tparam->prect, tparam->paddr);
	DrawSync(0);

	// Upload CLUT to framebuffer
	if (tparam->mode & 0x8) {
		LoadImage(tparam->crect, tparam->caddr);
		DrawSync(0);
	}
}

SPRTDATA ui_LoadSpriteData(TIM_IMAGE* tparam) {
	SPRTDATA data = { 0 };
	data.prect = *tparam->prect;
	data.crect = *tparam->crect;
	data.mode = tparam->mode;
	return data;
}

void ui_DrawSprite(SPRTDATA data, uint8_t color[3]) {
	SPRT* sprt = (SPRT*)db_nextpri;

	setSprt(sprt);
	setWH(sprt, data.w, data.h);                  // Initialize the primitive (very important)
	setXY0(sprt, data.x, data.y);           // Position the sprite at (48,48)
	setUV0(sprt,                    // Set UV coordinates
		data.u,
		data.v);
	setClut(sprt,                   // Set CLUT coordinates to sprite
		data.crect.x,
		data.crect.y);
	setRGB0(sprt,                   // Set primitive color
		color[0], color[1], color[2]);

	addPrim(db[db_active].ot, sprt);
	sprt++;
	db_nextpri = (char*)sprt;
}

void ui_SetTPage(SPRTDATA data) {
	DR_TPAGE* tprit = (DR_TPAGE*)db_nextpri;

	setDrawTPage(tprit, 0, 0, getTPage(data.mode & 0x3, 0, data.prect.x, data.prect.y));
	addPrim(db[db_active].ot, tprit);
	tprit++;

	db_nextpri = (char*)tprit;
}

void ui_DrawBack(u_long* tim, TIM_IMAGE* tparam) {
	OpenTIM(tim);
	ReadTIM(tparam);

	RECT pos;
	pos.h = 240;
	pos.w = 320;
	pos.x = db[db_active].disp.screen.x;
	pos.y = 0;

	// Upload pixel data to framebuffer
	LoadImage(&pos, tparam->paddr);
	DrawSync(0);
}