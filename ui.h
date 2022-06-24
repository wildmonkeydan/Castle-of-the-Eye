#pragma once
#include <libgpu.h>

typedef struct {
	RECT prect;
	RECT crect;
	u_long mode;
	short x, y, u, v;
}SPRTDATA;

// Load a .tim into VRAM 
void ui_LoadTexture(u_long* tim, TIM_IMAGE* tparam);

// Load a tim's info into useable data and discard TIM_IMAGE
SPRTDATA ui_LoadSpriteData(TIM_IMAGE* tparam);

// Draw a sprite
void ui_DrawSprite(SPRTDATA data, uint8_t color[3]);