#include "z64rom.h"

char* gDirOutput[] = {
	/* 00 */ "rom/",
	/* 01 */ "rom/object/",
	/* 02 */ "rom/actor/",
	/* 03 */ "rom/gamestate/",
	/* 04 */ "rom/scene/",
	/* 05 */ "rom/audio/",
};

enum {
	DIR_ROOT = 0,
	DIR_OBJ,
	DIR_ACT,
	DIR_STT,
	DIR_SCN,
	DIR_SND,
};

static void Rom_Config_WriteActor(MemFile* config, ActorOverlay* actorOvl, char* name) {
	Config_WriteTitle_Str(name);
	Config_WriteVar_Hex(vram_addr, ReadBE(actorOvl->vramStart));
	Config_WriteVar_Hex(init_vars, ReadBE(actorOvl->initInfo));
	Config_WriteVar_Hex(alloc_type, ReadBE(actorOvl->allocType));
}

static void Rom_Config_WriteState(MemFile* config, GameStateOverlay* stateOvl, char* name) {
	Config_WriteTitle_Str(name);
	Config_WriteVar_Hex(vram_addr, ReadBE(stateOvl->vramStart));
	Config_WriteVar_Hex(init_func, ReadBE(stateOvl->init));
	Config_WriteVar_Hex(dest_func, ReadBE(stateOvl->destroy));
}

Rom* Rom_New(char* romName) {
	Rom* rom = Lib_Calloc(0, sizeof(struct Rom));
	
	Assert(rom != NULL);
	
	if (MemFile_LoadFile(&rom->file, romName)) {
		printf_error_al("Load Rom", "%s", romName);
	}
	
	Lib_SetSeg(rom->file.data);
	
	rom->dmaTable = Lib_SegToPtr(ROM_DMA_TABLE);
	rom->objectTable = Lib_SegToPtr(ROM_OBJECT_TABLE);
	rom->actorTable = Lib_SegToPtr(ROM_ACTOR_TABLE);
	rom->stateTable = Lib_SegToPtr(ROM_STATE_TABLE);
	
	printf_debugExt_al("Load Rom", "%s", romName);
	
	return rom;
}

Rom* Rom_Free(Rom* rom) {
	MemFile_Free(&rom->file);
	memset(rom, 0, sizeof(struct Rom));
	
	return NULL;
}

void Rom_Dump(Rom* rom) {
	MemFile object;
	MemFile config;
	RomFile romFile;
	char buffer[256 * 4];
	
	MemFile_Malloc(&object, 0x460000);
	MemFile_Malloc(&config, 0x25000);
	
	Lib_MakeDir(gDirOutput[DIR_ROOT]);
	Lib_MakeDir(gDirOutput[DIR_OBJ]);
	printf_debugExt("Dump Objects");
	for (s32 i = 0; i < TABLESIZE_OBJECT; i++) {
		sprintf(buffer, "%s0x%04X-%s/object.zobj", gDirOutput[DIR_OBJ], i, gObjectName[i]);
		Lib_MakeDir(String_GetPath(buffer));
		
		romFile = Dma_RomFile_GetObject(rom, i);
		
		if (romFile.size == 0) {
			continue;
		}
		
		MemFile_Clear(&object);
		object.dataSize = romFile.size;
		MemFile_Realloc(&object, romFile.size);
		MemFile_Write(&object, romFile.data, romFile.size);
		
		MemFile_SaveFile(&object, buffer);
	}
	
	Lib_MakeDir(gDirOutput[DIR_ACT]);
	printf_debugExt("Dump Actors");
	for (s32 i = 0; i < TABLESIZE_ACTOR; i++) {
		sprintf(buffer, "%s0x%04X-%s/actor.zovl", gDirOutput[DIR_ACT], i, gActorName[i]);
		Lib_MakeDir(String_GetPath(buffer));
		
		romFile = Dma_RomFile_GetActor(rom, i);
		
		if (romFile.size == 0) {
			continue;
		}
		
		MemFile_Clear(&object);
		MemFile_Clear(&config);
		Rom_Config_WriteActor(&config, &rom->actorTable[i], gActorName[i]);
		object.dataSize = romFile.size;
		MemFile_Realloc(&object, romFile.size);
		MemFile_Write(&object, romFile.data, romFile.size);
		
		MemFile_SaveFile(&object, buffer);
		sprintf(buffer, "%s0x%04X-%s/config.cfg", gDirOutput[DIR_ACT], i, gActorName[i]);
		MemFile_SaveFile(&config, buffer);
	}
	
	Lib_MakeDir(gDirOutput[DIR_STT]);
	printf_debugExt("Dump GameStates");
	for (s32 i = 0; i < TABLESIZE_STATE; i++) {
		sprintf(buffer, "%s0x%04X-%s/gamestate.zovl", gDirOutput[DIR_STT], i, gStateName[i]);
		Lib_MakeDir(String_GetPath(buffer));
		
		romFile = Dma_RomFile_GetState(rom, i);
		
		if (romFile.size == 0) {
			continue;
		}
		
		MemFile_Clear(&object);
		MemFile_Clear(&config);
		Rom_Config_WriteState(&config, &rom->stateTable[i], gStateName[i]);
		object.dataSize = romFile.size;
		MemFile_Realloc(&object, romFile.size);
		MemFile_Write(&object, romFile.data, romFile.size);
		
		MemFile_SaveFile(&object, buffer);
		sprintf(buffer, "%s0x%04X-%s/config.cfg", gDirOutput[DIR_STT], i, gStateName[i]);
		MemFile_SaveFile(&config, buffer);
	}
	
	Lib_MakeDir(gDirOutput[DIR_SND]);
	printf_debugExt("Dump Sound");
	for (s32 i = 0; i < 3; i++) {
		char* dataName[] = {
			"Banks.bin",
			"Sequences.bin",
			"SampleTable"
		};
		sprintf(buffer, "%s%s", gDirOutput[DIR_SND], dataName[i]);
		Lib_MakeDir(String_GetPath(buffer));
		
		romFile = Dma_RomFile_GetDmaEntry(rom, i + 3);
		
		if (romFile.size != 0) {
			MemFile_Clear(&object);
			object.dataSize = romFile.size;
			MemFile_Realloc(&object, romFile.size);
			MemFile_Write(&object, romFile.data, romFile.size);
			
			MemFile_SaveFile(&object, buffer);
		}
	}
	
	MemFile_Free(&object);
}