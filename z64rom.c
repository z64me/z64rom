#include "lib/z64rom.h"

char* sToolName = PRNT_PRPL "z64rom " PRNT_GRAY "0.1 alpha";

void CheckTypes();

char* sToolUsage = {
	EXT_INFO_TITLE("Usage:")
	EXT_INFO("Dump", 12, "DragNDrop [.z64] to z64rom executable")
	PRNT_NL
	EXT_INFO_TITLE("Args:")
	EXT_INFO("--i",  12, "Input")
	EXT_INFO("--D",  12, "Debug Print")
};
s32 gExtractAudio = 0;

s32 Main(s32 argc, char* argv[]) {
	char* input = NULL;
	Rom* rom = Lib_Calloc(0, sizeof(struct Rom));
	u32 parArg = 0;
	
	#ifdef _WIN32
		printf_WinFix();
	#endif
	printf_SetPrefix("");
	
	if (ParArg("--A"))
		gExtractAudio = true;
	
	if (ParArg("--D"))
		printf_SetSuppressLevel(PSL_DEBUG);
	
	CheckTypes();
	
	if (Lib_ParseArguments(argv, "--i", &parArg))
		input = argv[parArg];
	else {
		for (s32 i = 0; i < argc; i++) {
			if (String_MemMemCase(argv[i], ".z64")) {
				input = argv[i];
				break;
			}
		}
	}
	
	if (input) {
		printf_toolinfo(sToolName, "\n");
		Rom_New(rom, input);
		Rom_Dump(rom);
		Rom_Free(rom);
	} else {
		if (Dir_Stat("rom/")) {
			printf_toolinfo(sToolName, "\n");
			Rom_New(rom, "oot-debug.z64");
			Rom_Build(rom);
			Rom_Free(rom);
			
			return 0;
		}
		printf_toolinfo(sToolName, sToolUsage);
	}
	
	#ifdef _WIN32
		if (argc == 1) {
			printf_info("Press enter to exit.");
			getchar();
		}
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