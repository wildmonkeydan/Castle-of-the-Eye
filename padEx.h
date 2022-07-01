#pragma once
#include <stdint.h>

typedef enum {
	// Standard pads, analog joystick, Jogcon
	PAD_SELECT		= 1 << 0,
	PAD_L3			= 1 << 1,
	PAD_R3			= 1 << 2,
	PAD_START		= 1 << 3,
	PAD_UP			= 1 << 4,
	PAD_RIGHT		= 1 << 5,
	PAD_DOWN		= 1 << 6,
	PAD_LEFT		= 1 << 7,
	PAD_L2			= 1 << 8,
	PAD_R2			= 1 << 9,
	PAD_L1			= 1 << 10,
	PAD_R1			= 1 << 11,
	PAD_TRIANGLE	= 1 << 12,
	PAD_CIRCLE		= 1 << 13,
	PAD_CROSS		= 1 << 14,
	PAD_SQUARE		= 1 << 15,

	// Mouse
	MOUSE_LEFT		= 1 << 10,
	MOUSE_RIGHT		= 1 << 11,

	// neGcon
	NCON_START		= 1 << 3,
	NCON_UP			= 1 << 4,
	NCON_RIGHT		= 1 << 5,
	NCON_DOWN		= 1 << 6,
	NCON_LEFT		= 1 << 7,
	NCON_R			= 1 << 8,
	NCON_B			= 1 << 9,
	NCON_A			= 1 << 10,

	// Guncon
	GCON_A			= 1 << 3,
	GCON_TRIGGER	= 1 << 13,
	GCON_B			= 1 << 14
} PadButton;

typedef struct _PADTYPE
{
	unsigned char	stat;
	unsigned char	len : 4;
	unsigned char	type : 4;
	unsigned short	btn;
	unsigned char	rs_x, rs_y;
	unsigned char	ls_x, ls_y;
} PADTYPE;