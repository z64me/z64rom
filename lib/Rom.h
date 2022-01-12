#ifndef __Z64_ROM_H__
#define __Z64_ROM_H__

#include "ExtLib.h"

struct Rom;

struct Rom* Rom_New(char* romName);
struct Rom* Rom_Free(struct Rom* rom);
void Rom_Dump(struct Rom* rom);

#endif