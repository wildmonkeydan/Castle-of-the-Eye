#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <libapi.h>
#include "input.h"
#include "padEx.h"


PADTYPE* pad;
// Pad data buffer
char pad_buff[2][34];

void input_Init() {
	// Init BIOS pad driver and set pad buffers (buffers are updated
	// automatically on every V-Blank)
	InitPAD(&pad_buff[0][0], 34, &pad_buff[1][0], 34);

	// Start pad
	StartPAD();

	// Don't make pad driver acknowledge V-Blank IRQ (recommended)
	ChangeClearPAD(0);
}

void input_Disable() {
	DisablePAD();
}

void input_Enable() {
	EnablePAD();
}

char input_Menu() {
	pad = (PADTYPE*)&pad_buff[0][0];
	char out = 0;

	if (pad->stat == 0) {
		// For digital pad, dual-analog and dual-shock
		if ((pad->type == 0x4) || (pad->type == 0x5) || (pad->type == 0x7)) {
			if (!(pad->btn & PAD_UP)) {
				out = -1;
			}
			if (!(pad->btn & PAD_DOWN)) {
				out = 1;
			}
			if (!(pad->btn & PAD_CROSS)) {
				out = 2;
			}
		}
	}

	return out;
}

CAMINPUT input_TestSuiteCam(SVECTOR* trot, CAMINPUT input) {

	// Set pad buffer data to pad pointer
	pad = (PADTYPE*)&pad_buff[0][0];

	if (pad->stat == 0) {

		// For digital pad, dual-analog and dual-shock
		if ((pad->type == 0x4) || (pad->type == 0x5) || (pad->type == 0x7)) {

			// The button status bits are inverted,
			// so 0 means pressed in this case

			// Look controls
			if (!(pad->btn & PAD_UP)) {

				// Look up
				input.cam_rot.vx -= ONE * 8;

			}
			else if (!(pad->btn & PAD_DOWN)) {

				// Look down
				input.cam_rot.vx += ONE * 8;

			}

			if (!(pad->btn & PAD_LEFT)) {

				// Look left
				input.cam_rot.vy += ONE * 8;

			}
			else if (!(pad->btn & PAD_RIGHT)) {

				// Look right
				input.cam_rot.vy -= ONE * 8;

			}

			// Movement controls
			if (!(pad->btn & PAD_TRIANGLE)) {

				// Move forward
				input.cam_pos.vx -= ((csin(trot->vy) * ccos(trot->vx)) >> 12) << 4;
				input.cam_pos.vy += csin(trot->vx) << 4;
				input.cam_pos.vz += ((ccos(trot->vy) * ccos(trot->vx)) >> 12) << 4;
			}
			else if (!(pad->btn & PAD_CROSS)) {

				// Move backward
				input.cam_pos.vx += ((csin(trot->vy) * ccos(trot->vx)) >> 12) << 2;
				input.cam_pos.vy -= csin(trot->vx) << 2;
				input.cam_pos.vz -= ((ccos(trot->vy) * ccos(trot->vx)) >> 12) << 2;

			}

			if (!(pad->btn & PAD_SQUARE)) {

				// Slide left
				input.cam_pos.vx -= ccos(trot->vy) << 2;
				input.cam_pos.vz -= csin(trot->vy) << 2;

			}
			else if (!(pad->btn & PAD_CIRCLE)) {

				// Slide right
				input.cam_pos.vx += ccos(trot->vy) << 2;
				input.cam_pos.vz += csin(trot->vy) << 2;

			}

			if (!(pad->btn & PAD_R1)) {

				// Slide up
				input.cam_pos.vx -= ((csin(trot->vy) * csin(trot->vx)) >> 12) << 2;
				input.cam_pos.vy -= ccos(trot->vx) << 2;
				input.cam_pos.vz += ((ccos(trot->vy) * csin(trot->vx)) >> 12) << 2;

			}

			if (!(pad->btn & PAD_R2)) {

				// Slide down
				input.cam_pos.vx += ((csin(trot->vy) * csin(trot->vx)) >> 12) << 2;
				input.cam_pos.vy += ccos(trot->vx) << 2;
				input.cam_pos.vz -= ((ccos(trot->vy) * csin(trot->vx)) >> 12) << 2;

			}

			input.count = 0;

			if (!(pad->btn & PAD_L1)) {
				input.count = 1;
			}

			if (!(pad->btn & PAD_L2)) {
				input.count = -1;
			}


			if (!(pad->btn & PAD_START)) {

				_boot();

			}

			input.mode = 0;

			if (!(pad->btn & PAD_SELECT)) {
				input.mode++;
			}

		}

		// For dual-analog and dual-shock (analog input)
		if ((pad->type == 0x5) || (pad->type == 0x7)) {

			// Moving forwards and backwards
			if (((pad->ls_y - 128) < -16) || ((pad->ls_y - 128) > 16)) {

				input.cam_pos.vx += (((csin(trot->vy) * ccos(trot->vx)) >> 12) * (pad->ls_y - 128)) >> 5;
				input.cam_pos.vy -= (csin(trot->vx) * (pad->ls_y - 128)) >> 5;
				input.cam_pos.vz -= (((ccos(trot->vy) * ccos(trot->vx)) >> 12) * (pad->ls_y - 128)) >> 5;

			}

			// Strafing left and right
			if (((pad->ls_x - 128) < -16) || ((pad->ls_x - 128) > 16)) {
				input.cam_pos.vx += (ccos(trot->vy) * (pad->ls_x - 128)) >> 5;
				input.cam_pos.vz += (csin(trot->vy) * (pad->ls_x - 128)) >> 5;
			}

			// Look up and down
			if (((pad->rs_y - 128) < -16) || ((pad->rs_y - 128) > 16)) {
				input.cam_rot.vx += (pad->rs_y - 128) << 9;
			}

			// Look left and right
			if (((pad->rs_x - 128) < -16) || ((pad->rs_x - 128) > 16)) {
				input.cam_rot.vy -= (pad->rs_x - 128) << 9;
			}
		}
		FntPrint("BUTTONS=%04x\n", pad->btn);
	}
	return input;
}