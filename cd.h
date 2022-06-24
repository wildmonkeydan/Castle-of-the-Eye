#pragma once
#include <stdint.h>

// Load a file from the CD
unsigned long* cdLoad_File(const char* filename);

// Ready the CD drive to play CDDA music
void cdMusic_Ready();

// Play the CDDA track specified (Call cdMusic_Ready first)
void cdMusic_PlayTrack(int8_t track);