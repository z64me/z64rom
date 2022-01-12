#include <lib/z64rom.h>

s32 Main(s32 argc, char* argv[]) {
	Rom* rom;
	
	printf_WinFix();
	printf_SetPrefix("");
	printf_SetSuppressLevel(PSL_DEBUG);
	
	rom = Rom_New("oot-debug.z64");
	Rom_Dump(rom);
	rom = Rom_Free(rom);
}