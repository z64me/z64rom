#include "lib/z64rom.h"

char* sToolName = "z64rom 0.1 alpha";

void CheckTypes();

char* messages[] = {
	#ifndef _WIN32
		"Feed me [.z64] rom please! 🙏\n\n",
		"I demand [.z64], now! 👺\n\n",
		"Either you give me [.z64] or you owe me 🍕\n\n",
	#else
		"Feed me [.z64] rom please!\n\n",
		"I demand [.z64], now!\n\n",
		"Either you give me [.z64] or you owe me pizza.\n\n",
	#endif
};

s32 Main(s32 argc, char* argv[]) {
	Rom* rom;
	u32 parArg;
	time_t tm;
	
	time(&tm);
	
	printf_WinFix();
	printf_SetPrefix("");
	Dir_SetParam(DIR__MAKE_ON_ENTER);
	if (ParArg("-d"))
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