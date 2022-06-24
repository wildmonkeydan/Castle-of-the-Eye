#pragma once
#include "const.h"

// Double buffer structure
typedef struct {
	DISPENV	disp;			// Display environment
	DRAWENV	draw;			// Drawing environment
	u_long 	ot[OT_LEN];		// Ordering table
	char 	p[PACKET_LEN];	// Packet buffer
} DB;

// Double buffer variables
static DB	db[2];
static int	db_active = 0;
static char* db_nextpri;
static RECT	screen_clip;

// Initiate the display
void disp_initDisplay();

// Draw primitives on the screen
void disp_Display();