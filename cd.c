#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <libcd.h>
#include <libapi.h>
#include <libetc.h>
#include "const.h"

unsigned long* cdLoad_File(const char* filename) 
{
	CdlFILE	file;
	unsigned long* buffer;


	printf("Reading file %s... ", filename);

	// Search for the file
	if (!CdSearchFile(&file, (char*)filename))
	{
		// Return value is NULL, file is not found
		printf("Not found!\n");
		buffer = NULL;
		//return NULL;
	}

	// Allocate a buffer for the file
	buffer = (unsigned long*)malloc(2048 * ((file.size + 2047) / 2048));

	// Set seek target (seek actually happens on CdRead())
	CdControl(CdlSetloc, (unsigned char*)&file.pos, 0);

	// Read sectors
	CdRead((int)BtoS(file.size), (u_long*)buffer, CdlModeSpeed);

	// Wait until read has completed
	CdReadSync(0, 0);

	printf("Done.\n");

	return buffer;
}

void cdMusic_Ready() {
	u_char result;
	uint8_t cmd = CdlModeDA | CdlModeAP;
	CdControl(CdlSetmode, &cmd, &result);
	printf("Result: %d", (int)result);
}

void cdMusic_PlayTrack(int8_t track) {
	printf("Playing track %d.\n", track);

	uint8_t cmd = itob(track);
	CdControlF(CdlPlay, &cmd);
}