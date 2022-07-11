#pragma once
#include <libgte.h>
#include <libgpu.h>

typedef struct {
	VECTOR	cam_pos;
	VECTOR	cam_rot;
	char	count;
	char	mode;
}CAMINPUT;

// Initiate the pad data buffer
void input_Init();

// Disable pad updates
void input_Disable();

// Enable pad updates
void input_Enable();

// Process inputs specfic to the menus
char input_Menu();

// Process inputs specific to the Test Suite Camera
CAMINPUT input_TestSuiteCam(SVECTOR* trot, CAMINPUT input);