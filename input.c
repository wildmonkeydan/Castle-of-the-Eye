#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <libapi.h>
#include "padEx.h"
#include "input.h"

void input_Init() {
	// Init BIOS pad driver and set pad buffers (buffers are updated
	// automatically on every V-Blank)
	InitPAD(&pad_buff[0][0], 34, &pad_buff[1][0], 34);

	// Start pad
	StartPAD();

	// Don't make pad driver acknowledge V-Blank IRQ (recommended)
	ChangeClearPAD(0);
}