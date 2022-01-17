#ifndef __Z64_ROM_H__
#define __Z64_ROM_H__

#include "ExtLib.h"

struct Rom;

void Rom_New(struct Rom* rom, char* romName);
void Rom_Free(struct Rom* rom);
void Rom_Dump(struct Rom* rom);

#endif