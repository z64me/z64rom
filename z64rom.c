#include "lib/z64rom.h"

char* sToolName = "z64rom 0.1 alpha";

void CheckTypes();

char* messages[] = {
	"Run [" PRNT_YELW "z64rom *.z64" PRNT_RSET "] or drag'n'drop a [" PRNT_YELW "*.z64" PRNT_RSET "] rom\n",
};

s32 Main(s32 argc, char* argv[]) {
	Rom* rom;
	u32 parArg;
	time_t tm;
	
	time(&tm);
	printf_WinFix();
	printf_SetPrefix("");
	Dir_SetParam(DIR__MAKE_ON_ENTER);
	if (ParArg("--D"))
		printf_SetSuppressLevel(PSL_DEBUG);
	
	CheckTypes();
	
	if (argc > 1 && (String_MemMemCase(argv[1], ".z64"))) {
		printf_toolinfo(sToolName, "");
		rom = Rom_New(argv[1]);
		Rom_Dump(rom);
		rom = Rom_Free(rom);
	} else {
		printf_toolinfo(sToolName, messages[tm % (ArrayCount(messages))]);
	}
	
	#ifdef _WIN32
		printf_info("Press enter to exit.");
		getchar();
	#endif
	
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