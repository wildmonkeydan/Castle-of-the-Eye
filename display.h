#pragma once
#include <stdint.h>
#include "const.h"
#include "ui.h"
#include "trtypes.h"


// Initiate the display
void disp_initDisplay();

// Draw primitives on the screen
void disp_Display();

// Change the texture page used for drawing
void disp_SetDrawTPage(SPRTDATA data);

// Draw a sprite (SPRT)
void disp_DrawSprite(SPRTDATA data, uint8_t color[3]);

// Draw a 16-bit background using VRAM commands
void disp_DrawBack(u_long* tim, TIM_IMAGE* tparam);

// Draw a TILE primitive
void disp_DrawTile(TILEDATA data);

// Draw a room
void gte_DrawRoom(MATRIX* mtx, VECTOR* pos, SVECTOR* rot, tr_room_data room);

// Draw a mesh
void gte_DrawMesh(MATRIX* mtx, VECTOR* pos, SVECTOR* rot, tr4_mesh mesh);

// Draw a SPRT8
void disp_DrawSprite8(SPRTDATA data);

// Draw a button icon using a SPRT8
void ui_DrawButton(int button, int x, int y);