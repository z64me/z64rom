#include "z64rom.h"

u32 gDmaDataTable = 0x012F70;
u32 gObjectTable = 0xB9E6C8;
u32 gActorOverlayTable = 0xB8D440;
u32 gGameStateOverlayTable = 0xB969D0;
u32 gSceneTable = 0xBA0BB0;

// ?
u32 gSequenceFontTable = 0xBCC4E0;

/* SegmentRomStart is the offset to each AudioEntry
 * entry->romAddr += romSegAddr;
 */
u32 gSequenceTable = 0xBCC6A0;
u32 _Sequence_SegmentRomStart = 0x44df0;

u32 gSoundFontTable = 0xBCC270;
u32 _SoundFont_SegmentRomStart = 0x19030;

u32 gSampleBankTable = 0xBCCD90;
u32 _SampleBank_SegmentRomStart = 0x94870;

u32 OBJECT_ID_MAX = 402;
u32 SCENE_ID_MAX = 110;
u32 ACTOR_ID_MAX = 471;
u32 GAMESTATE_ID_MAX = 6;
u32 DMA_ID_MAX = 1548;

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

static void Rom_Config_Actor(MemFile* config, ActorEntry* actorOvl, char* name, char* out) {
	char buffer[256 * 4];
	
	sprintf(buffer, "%s%s", String_GetPath(out), "config.cfg");
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Hex(vram_addr, ReadBE(actorOvl->vramStart));
	Config_WriteVar_Hex(init_vars, ReadBE(actorOvl->initInfo));
	Config_WriteVar_Int(alloc_type, ReadBE(actorOvl->allocType));
	MemFile_SaveFile(config, buffer);
}

static void Rom_Config_GameState(MemFile* config, GameStateEntry* stateOvl, char* name, char* out) {
	char buffer[256 * 4];
	
	sprintf(buffer, "%s%s", String_GetPath(out), "config.cfg");
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Hex(vram_addr, ReadBE(stateOvl->vramStart));
	Config_WriteVar_Hex(init_func, ReadBE(stateOvl->init));
	Config_WriteVar_Hex(dest_func, ReadBE(stateOvl->destroy));
	MemFile_SaveFile(config, buffer);
}

static void Rom_Config_Scene(MemFile* config, SceneEntry* sceneEntry, char* name, char* out) {
	char buffer[256 * 4];
	
	sprintf(buffer, "%s%s", String_GetPath(out), "config.cfg");
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Int(unk_a, ReadBE(sceneEntry->unk_10));
	Config_WriteVar_Int(unk_b, ReadBE(sceneEntry->unk_12));
	Config_WriteVar_Int(shader, ReadBE(sceneEntry->config));
	MemFile_SaveFile(config, buffer);
}

static void Rom_Config_Instrument(MemFile* config, Instrument* instrument, char* name, char* out) {
	Adsr* envelope = SegmentedToVirtual(0x1, ReadBE(instrument->envelope));
	Sample* sample;
	u32 val;
	f32* f = (f32*)&val;
	
	MemFile_Clear(config);
	Config_WriteTitle_Str("Instrument");
	Config_WriteVar_Int(loaded, instrument->loaded);
	Config_WriteVar_Int(split_lo, instrument->splitLo);
	Config_WriteVar_Int(split_hi, instrument->splitHi);
	
	Config_WriteTitle_Str("Envelope");
	Config_WriteVar_Int(attack_rate, ReadBE(envelope[0].rate));
	Config_WriteVar_Int(attack_level, ReadBE(envelope[0].level));
	Config_WriteVar_Int(hold_rate, ReadBE(envelope[1].rate));
	Config_WriteVar_Int(hold_level, ReadBE(envelope[1].level));
	Config_WriteVar_Int(decay_rate, ReadBE(envelope[2].rate));
	Config_WriteVar_Int(decay_level, ReadBE(envelope[2].level));
	Config_WriteVar_Int(release, instrument->release);
	
	Config_WriteTitle_Str("Low Sample");
	if (instrument->lo.sample != 0) {
		sample = SegmentedToVirtual(0x1, ReadBE(instrument->lo.sample));
		val = ReadBE(instrument->lo.swap32);
		Config_WriteVar_Hex(low_sample_id, ReadBE(sample->sampleAddr));
		Config_WriteVar_Flo(low_tuning, *f);
	} else {
		Config_WriteVar_Str(low_sample_id, NULL);
		Config_WriteVar_Str(low_tuning, NULL);
	}
	
	Config_WriteTitle_Str("Primary Sample");
	if (instrument->prim.sample != 0) {
		sample = SegmentedToVirtual(0x1, ReadBE(instrument->prim.sample));
		val = ReadBE(instrument->prim.swap32);
		Config_WriteVar_Hex(prim_sample_id, ReadBE(sample->sampleAddr));
		Config_WriteVar_Flo(prim_tuning, *f);
	} else {
		Config_WriteVar_Str(prim_sample_id, NULL);
		Config_WriteVar_Str(prim_tuning, NULL);
	}
	
	Config_WriteTitle_Str("High Sample");
	if (instrument->hi.sample != 0) {
		sample = SegmentedToVirtual(0x1, ReadBE(instrument->hi.sample));
		val = ReadBE(instrument->hi.swap32);
		Config_WriteVar_Hex(high_sample_id, ReadBE(sample->sampleAddr));
		Config_WriteVar_Flo(high_tuning, *f);
	} else {
		Config_WriteVar_Str(high_sample_id, NULL);
		Config_WriteVar_Str(high_tuning, NULL);
	}
	
	MemFile_SaveFile(config, out);
}

static s32 Rom_Extract(MemFile* mem, RomFile rom, char* name) {
	Lib_MakeDir(String_GetPath(name));
	if (rom.size == 0)
		return 0;
	MemFile_Clear(mem);
	mem->dataSize = rom.size;
	MemFile_Realloc(mem, rom.size);
	MemFile_Write(mem, rom.data, rom.size);
	MemFile_SaveFile(mem, name);
	
	return 1;
}

void Rom_Dump_Audio(Rom* rom, MemFile* dataFile, MemFile* config) {
	char buffer[256 * 2];
	AudioEntryHead* head = SegmentedToVirtual(0, gSoundFontTable);
	SoundFontEntry* entry;
	u32 num = ReadBE(head->numEntries);
	RomFile romFile;
	SoundFont* bank;
	Instrument* instrument;
	Sound* sfx;
	
	String_Copy(buffer, gDirOutput[DIR_SND]);
	String_Merge(buffer, "soundfont/");
	Lib_MakeDir(buffer);
	
	for (s32 i = 0; i < num; i++) {
		if (gPrintfSuppress != PSL_DEBUG)
			printf_progress("SoundFont", i + 1, num);
		
		entry = &head->entries[i];
		bank = SegmentedToVirtual(0x0, ReadBE(entry->romAddr) + _SoundFont_SegmentRomStart);
		SetSegment(0x1, bank);
		
		sprintf(buffer, "%ssoundfont/%d-%s/", gDirOutput[DIR_SND], i, gBankName[i]);
		Lib_MakeDir(buffer);
		String_Merge(buffer, "Instuments/");
		Lib_MakeDir(buffer);
		
		for (s32 j = 0; j < entry->numInst; j++) {
			printf_debug("%d / %d, %08X", j + 1, entry->numInst, ReadBE(bank->instruments[j]));
			
			if (bank->instruments[j] == 0)
				continue;
			
			String_SMerge(buffer, "%d-Inst.cfg", j);
			instrument = SegmentedToVirtual(0x1, ReadBE(bank->instruments[j]));
			
			Rom_Config_Instrument(config, instrument, "instrument", buffer);
			String_Copy(buffer, String_GetPath(buffer));
		}
	}
}

Rom* Rom_New(char* romName) {
	Rom* rom = Lib_Calloc(0, sizeof(struct Rom));
	
	Assert(rom != NULL);
	
	if (MemFile_LoadFile(&rom->file, romName)) {
		printf_error_align("Load Rom", "%s", romName);
	}
	
	SetSegment(0x0, rom->file.data);
	rom->dmaTable = SegmentedToVirtual(0x0, gDmaDataTable);
	rom->objectTable = SegmentedToVirtual(0x0, gObjectTable);
	rom->actorTable = SegmentedToVirtual(0x0, gActorOverlayTable);
	rom->stateTable = SegmentedToVirtual(0x0, gGameStateOverlayTable);
	rom->sceneTable = SegmentedToVirtual(0x0, gSceneTable);
	
	printf_debugExt_align("Load Rom", "%s", romName);
	
	return rom;
}

Rom* Rom_Free(Rom* rom) {
	MemFile_Free(&rom->file);
	memset(rom, 0, sizeof(struct Rom));
	
	return NULL;
}

void Rom_Dump(Rom* rom) {
	MemFile dataFile;
	MemFile config;
	RomFile romFile;
	char buffer[256 * 4];
	
	MemFile_Malloc(&dataFile, 0x460000); // Slightly larger than audiotable
	MemFile_Malloc(&config, 0x25000);
	
	Lib_MakeDir(gDirOutput[DIR_ROOT]);
	printf_info_align("Dump Rom", PRNT_YELW "%s", rom->file.info.name);
	
	Lib_MakeDir(gDirOutput[DIR_ACT]);
	for (s32 i = 0; i < ACTOR_ID_MAX; i++) {
		printf_progress("Actor", i + 1, ACTOR_ID_MAX);
		sprintf(buffer, "%s%d-%s/actor.zovl", gDirOutput[DIR_ACT], i, gActorName[i]);
		if (Rom_Extract(&dataFile, Dma_RomFile_Actor(rom, i), buffer))
			Rom_Config_Actor(&config, &rom->actorTable[i], gActorName[i], buffer);
	}
	
	Lib_MakeDir(gDirOutput[DIR_OBJ]);
	for (s32 i = 0; i < OBJECT_ID_MAX; i++) {
		printf_progress("Object", i + 1, OBJECT_ID_MAX);
		sprintf(buffer, "%s%d-%s/object.zobj", gDirOutput[DIR_OBJ], i, gObjectName[i]);
		Rom_Extract(&dataFile, Dma_RomFile_Object(rom, i), buffer);
	}
	
	Lib_MakeDir(gDirOutput[DIR_STT]);
	for (s32 i = 0; i < GAMESTATE_ID_MAX; i++) {
		printf_progress("GameState", i + 1, GAMESTATE_ID_MAX);
		sprintf(buffer, "%s%d-%s/gamestate.zovl", gDirOutput[DIR_STT], i, gStateName[i]);
		if (Rom_Extract(&dataFile, Dma_RomFile_GameState(rom, i), buffer))
			Rom_Config_GameState(&config, &rom->stateTable[i], gStateName[i], buffer);
	}
	
	Lib_MakeDir(gDirOutput[DIR_SCN]);
	for (s32 i = 0; i < SCENE_ID_MAX; i++) {
		printf_progress("Scene", i + 1, SCENE_ID_MAX);
		sprintf(buffer, "%s%d-%s/scene.zscene", gDirOutput[DIR_SCN], i, gSceneName[i]);
		if (Rom_Extract(&dataFile, Dma_RomFile_Scene(rom, i), buffer))
			Rom_Config_Scene(&config, &rom->sceneTable[i], gSceneName[i], buffer);
	}
	
	Lib_MakeDir(gDirOutput[DIR_SND]);
	Rom_Dump_Audio(rom, &dataFile, &config);
	
	MemFile_Free(&dataFile);
	MemFile_Free(&config);
}