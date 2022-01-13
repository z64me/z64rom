#include <lib/z64rom.h>

char* sToolName = "z64rom 0.1 alpha";

void CheckTypes();

s32 Main(s32 argc, char* argv[]) {
	Rom* rom;
	u32 parArg;
	
	printf_WinFix();
	printf_SetPrefix("");
	Dir_SetParam(DIR__MAKE_ON_ENTER);
	if (ParArg("-d"))
		printf_SetSuppressLevel(PSL_DEBUG);
	
	printf_toolinfo(sToolName, "");
	CheckTypes();
	
	rom = Rom_New("oot-debug.z64");
	if (ParArg("-D"))
		Rom_Dump(rom);
	rom = Rom_Free(rom);
	
	// printf_info("Press enter to exit.");
	// getchar();
	
	return 0;
}

void CheckTypes() {
	u32 error = 0;
	
	#define SizeTester(type, expectedSize) if (sizeof(type) != expectedSize) { \
			printf_error_align("sizeof(" # type ")", "%d > %d", sizeof(type), expectedSize); \
			error++; \
	}
	SizeTester(enum SampleMedium, 1);
	SizeTester(enum SeqPlayer, 1);
	SizeTester(struct SoundFontEntry, 16);
	SizeTester(struct Instrument, 32);
	SizeTester(struct Adsr, 4);
	SizeTester(struct Sound, 8);
	SizeTester(struct AudioEntryHead, 16);
	
	if (error) {
		exit(EXIT_FAILURE);
	}
}