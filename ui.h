#pragma once
#include <libgte.h>
#include <libgpu.h>

typedef struct {
	RECT prect;
	RECT crect;
	u_long mode;
	short x, y, u, v;
	u_char w, h;
}SPRTDATA;

typedef struct {
	RECT size;
	uint8_t color[3];
}TILEDATA;

// Load a .tim into VRAM 
void ui_LoadTexture(u_long* tim, TIM_IMAGE* tparam);

// Load a tim's info into useable data and discard TIM_IMAGE
SPRTDATA ui_LoadSpriteData(TIM_IMAGE* tparam);

// Draw a sprite
void ui_DrawSprite(SPRTDATA data, uint8_t color[3]);

// Set the texture page for drawing UI elements
void ui_SetTPage(SPRTDATA data);

// Draw a 16-bit menu background
//void ui_DrawBack(u_long* tim, TIM_IMAGE* tparam);
