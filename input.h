#pragma once
#include <libgte.h>
#include <libgpu.h>

typedef struct {
	VECTOR	cam_pos;
	VECTOR	cam_rot;
	char	count;
}CAMINPUT;


// Pad data buffer
static char pad_buff[2][34];

// Initiate the pad data buffer
void input_Init();

// Process inputs specfic to the menus
char input_Menu();

// Process inputs specific to the Test Suite Camera
CAMINPUT input_TestSuiteCam(SVECTOR* trot, CAMINPUT input);