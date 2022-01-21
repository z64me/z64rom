#include "z64rom.h"

static s32 Rom_Extract(MemFile* mem, RomFile rom, char* name) {
	if (rom.size == 0)
		return 0;
	MemFile_Clear(mem);
	mem->dataSize = rom.size;
	MemFile_Realloc(mem, rom.size);
	MemFile_Write(mem, rom.data, rom.size);
	MemFile_SaveFile(mem, name);
	
	return 1;
}

/* / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / */

static void Rom_Config_Actor(MemFile* config, ActorEntry* actorOvl, const char* name, char* out) {
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Hex("vram_addr", ReadBE(actorOvl->vramStart));
	Config_WriteVar_Hex("init_vars", ReadBE(actorOvl->initInfo));
	Config_WriteVar_Int("alloc_type", ReadBE(actorOvl->allocType));
	MemFile_SaveFile_String(config, out);
}

static void Rom_Config_GameState(MemFile* config, GameStateEntry* stateOvl, const char* name, char* out) {
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Hex("vram_addr", ReadBE(stateOvl->vramStart));
	Config_WriteVar_Hex("init_func", ReadBE(stateOvl->init));
	Config_WriteVar_Hex("dest_func", ReadBE(stateOvl->destroy));
	MemFile_SaveFile_String(config, out);
}

static void Rom_Config_Player(Rom* rom, MemFile* config, KaleidoEntry* player, const char* name, char* out) {
	u16* dataHi;
	u16* dataLo;
	u32 init;
	u32 dest;
	u32 updt;
	u32 draw;
	
	dataHi = SegmentedToVirtual(0x0, rom->offset.table.player.init.hi);
	dataLo = SegmentedToVirtual(0x0, rom->offset.table.player.init.lo);
	init = ReadBE(dataHi[1]) << 16 | ReadBE(dataLo[1]);
	
	dataHi = SegmentedToVirtual(0x0, rom->offset.table.player.dest.hi);
	dataLo = SegmentedToVirtual(0x0, rom->offset.table.player.dest.lo);
	dest = ReadBE(dataHi[1]) << 16 | ReadBE(dataLo[1]);
	
	dataHi = SegmentedToVirtual(0x0, rom->offset.table.player.updt.hi);
	dataLo = SegmentedToVirtual(0x0, rom->offset.table.player.updt.lo);
	updt = ReadBE(dataHi[1]) << 16 | ReadBE(dataLo[1]);
	
	dataHi = SegmentedToVirtual(0x0, rom->offset.table.player.draw.hi);
	dataLo = SegmentedToVirtual(0x0, rom->offset.table.player.draw.lo);
	draw = ReadBE(dataHi[1]) << 16 | ReadBE(dataLo[1]);
	
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Hex("vram_addr", ReadBE(player->vramStart));
	Config_WriteVar_Hex("init", init);
	Config_WriteVar_Hex("dest", dest);
	Config_WriteVar_Hex("updt", updt);
	Config_WriteVar_Hex("draw", draw);
	MemFile_SaveFile_String(config, out);
}

static void Rom_Config_Scene(MemFile* config, SceneEntry* sceneEntry, const char* name, char* out) {
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Int("unk_a", ReadBE(sceneEntry->unk_10));
	Config_WriteVar_Int("unk_b", ReadBE(sceneEntry->unk_12));
	Config_WriteVar_Int("shader", ReadBE(sceneEntry->config));
	MemFile_SaveFile_String(config, out);
}

/* / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / */

char sBankFiles[1024 * 5][512];
s32 sBankNum;
SampleInfo sUnsortedSampleTbl[1024 * 5];
SampleInfo* sSortedSampleTbl[1024 * 5];
s32 sDumpID;
s32 sSortID;

#define __Config_Sample(wow, sampletype) \
	Config_WriteVar_Hex(# wow "_sample", ReadBE(sample->sampleAddr) + rom->offset.segment.smplRom + off); \
	Config_WriteVar_Flo(# wow "_tuning", *f); \
	if (sBankNum < 0) { printf("\a\n"); exit(1); /* "go intentionally bonkers" */ } \
	sUnsortedSampleTbl[sDumpID].tuning = *f; \
	sUnsortedSampleTbl[sDumpID].data = sample->data; \
	sUnsortedSampleTbl[sDumpID].sampleAddr = ReadBE(sample->sampleAddr) + rom->offset.segment.smplRom + off; \
	sUnsortedSampleTbl[sDumpID].loop = VirtualToSegmented(0x0, SegmentedToVirtual(0x1, ReadBE(sample->loop))); \
	sUnsortedSampleTbl[sDumpID++].book = VirtualToSegmented(0x0, SegmentedToVirtual(0x1, ReadBE(sample->book))); \
	Assert(sDumpID < 1024 * 5);

#define __Config_Sample_NULL(wow) \
	Config_WriteVar_Str(# wow "_sample", "NULL"); \
	Config_WriteVar_Str(# wow "_tuning", "NULL");

static void Rom_Config_Instrument(Rom* rom, MemFile* config, Instrument* instrument, char* name, char* out, u32 off) {
	Adsr* envelope;
	Sample* sample;
	u32 val;
	f32* f = (f32*)&val;
	Instrument tempI = { .splitHi = 127 };
	Adsr tempE[4] = { 0 };
	
	if (instrument == NULL) {
		envelope = tempE;
		instrument = &tempI;
	} else {
		envelope = SegmentedToVirtual(0x1, ReadBE(instrument->envelope));
	}
	
	MemFile_Clear(config);
	Config_WriteTitle_Str("Instrument");
	Config_WriteVar_Int("loaded", instrument->loaded);
	Config_WriteVar_Int("split_lo", instrument->splitLo);
	Config_WriteVar_Int("split_hi", instrument->splitHi);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Envelope");
	Config_WriteVar_Int("attack_rate", ReadBE(envelope[0].rate));
	Config_WriteVar_Int("attack_level", ReadBE(envelope[0].level));
	Config_WriteVar_Int("hold_rate", ReadBE(envelope[1].rate));
	Config_WriteVar_Int("hold_level", ReadBE(envelope[1].level));
	Config_WriteVar_Int("decay_rate", ReadBE(envelope[2].rate));
	Config_WriteVar_Int("decay_level", ReadBE(envelope[2].level));
	Config_WriteVar_Int("release", instrument->release);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Low Sample");
	if (instrument->lo.sample != 0) {
		sample = SegmentedToVirtual(0x1, ReadBE(instrument->lo.sample));
		val = ReadBE(instrument->lo.swap32);
		__Config_Sample(low, ins);
	} else {
		__Config_Sample_NULL(low);
	}
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Primary Sample");
	if (instrument->prim.sample != 0) {
		sample = SegmentedToVirtual(0x1, ReadBE(instrument->prim.sample));
		val = ReadBE(instrument->prim.swap32);
		sUnsortedSampleTbl[sDumpID].isPrim = true;
		sUnsortedSampleTbl[sDumpID].splitLo = instrument->splitLo;
		sUnsortedSampleTbl[sDumpID].splitHi = instrument->splitHi;
		__Config_Sample(prim, ins);
	} else {
		__Config_Sample_NULL(prim);
	}
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("High Sample");
	if (instrument->hi.sample != 0) {
		sample = SegmentedToVirtual(0x1, ReadBE(instrument->hi.sample));
		val = ReadBE(instrument->hi.swap32);
		__Config_Sample(hi, ins);
	} else {
		__Config_Sample_NULL(hi);
	}
	
	MemFile_SaveFile_String(config, out);
}

static void Rom_Config_Sfx(Rom* rom, MemFile* config, Sound* sfx, char* name, char* out, u32 off) {
	u32 val;
	f32* f = (f32*)&val;
	
	MemFile_Clear(config);
	Config_WriteTitle_Str("Sfx");
	if (sfx->sample != 0) {
		Sample* sample = SegmentedToVirtual(0x1, ReadBE(sfx->sample));
		val = ReadBE(sfx->swap32);
		__Config_Sample(prim, sfx);
	} else {
		__Config_Sample_NULL(prim);
	}
	MemFile_SaveFile_String(config, out);
}

static void Rom_Config_Drum(Rom* rom, MemFile* config, Drum* drum, char* name, char* out, u32 off) {
	u32 val;
	f32* f = (f32*)&val;
	
	MemFile_Clear(config);
	Config_WriteTitle_Str("Drum");
	Config_WriteVar_Int("loaded", drum->loaded);
	Config_WriteVar_Int("pan", drum->pan);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Envelope");
	Config_WriteVar_Int("attack_rate", ReadBE(drum->envelope[0].rate));
	Config_WriteVar_Int("attack_level", ReadBE(drum->envelope[0].level));
	Config_WriteVar_Int("hold_rate", ReadBE(drum->envelope[1].rate));
	Config_WriteVar_Int("hold_level", ReadBE(drum->envelope[1].level));
	Config_WriteVar_Int("decay_rate", ReadBE(drum->envelope[2].rate));
	Config_WriteVar_Int("decay_level", ReadBE(drum->envelope[2].level));
	Config_WriteVar_Int("release", drum->release);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Sample");
	if (drum->sound.sample != 0) {
		Sample* sample = SegmentedToVirtual(0x1, ReadBE(drum->sound.sample));
		val = ReadBE(drum->sound.swap32);
		__Config_Sample(prim, drm);
	} else {
		__Config_Sample_NULL(prim);
	}
	
	MemFile_SaveFile_String(config, out);
}

static void Rom_Config_Sample(Rom* rom, MemFile* config, Sample* sample, char* name, char* out) {
	AdpcmLoop* loop = SegmentedToVirtual(0x0, sample->loop);
	
	MemFile_Clear(config);
	Config_WriteTitle_Str(name);
	Config_WriteVar_Int("codec", ReadBE(sample->data) >> (32 - 4));
	Config_WriteVar_Int("medium", (ReadBE(sample->data) >> (32 - 6)) & 2);
	Config_WriteVar_Int("bitA", (ReadBE(sample->data) >> (32 - 7)) & 1);
	Config_WriteVar_Int("bitB", (ReadBE(sample->data) >> (32 - 8)) & 1);
	
	Config_SPrintf("\n");
	Config_WriteTitle_Str("Loop");
	Config_WriteVar_Int("loop_start", ReadBE(loop->start));
	Config_WriteVar_Int("loop_end", ReadBE(loop->end));
	Config_WriteVar_Int("loop_count", ReadBE(loop->count));
	Config_WriteVar_Int("tail_end", ReadBE(loop->origSpls));
	
	MemFile_SaveFile_String(config, out);
}

/* / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / */

static void Rom_Dump_SoundFont(Rom* rom, MemFile* dataFile, MemFile* config) {
	AudioEntryHead* head = SegmentedToVirtual(0, rom->offset.table.fontTable);
	AudioEntryHead* sampHead = SegmentedToVirtual(0, rom->offset.table.sampleTable);
	SoundFontEntry* entry;
	u32 num = ReadBE(head->numEntries);
	SoundFont* bank;
	Instrument* instrument;
	Sound* sfx;
	Drum* drum;
	u32 off = 0;
	
	printf_debugExt_align("Entry Num", "%d", num);
	
	Dir_Enter("soundfont/");
	for (s32 i = 0; i < num; i++) {
		printf_progress("SoundFont", i + 1, num);
		
		entry = &head->entries[i];
		
		#ifndef NDEBUG
			printf_debug_align("numInst", "%d", ReadBE(entry->numInst));
			printf_debug_align("numSfx", "%d", ReadBE(entry->numSfx));
			printf_debug_align("numDrum", "%d", ReadBE(entry->numDrum));
			printf_debug_align("bank", "%08X", ReadBE(entry->romAddr) + rom->offset.segment.fontRom);
			printf_debug_align("size", "%08X", ReadBE(entry->size));
		#endif
		
		bank = SegmentedToVirtual(0x0, ReadBE(entry->romAddr) + rom->offset.segment.fontRom);
		off = ReadBE(sampHead->entries[entry->audioTable1].romAddr);
		if (off & 0xF) {
			printf_warning("audioTable Segment %08X id %d", off, entry->audioTable1);
			off = off & 0xFFFFFFF0;
		}
		SetSegment(0x1, bank);
		
		Dir_Enter("0x%02X-%s/", i, gBankName[i]);
		
		if (entry->numInst) {
			Dir_Enter("instrument/");
			
			#ifndef NDEBUG
				printf_debug_align("dump", "instruments");
			#endif
			
			for (s32 j = 0; j < entry->numInst; j++) {
				char* output = Dir_File("%d-Inst.cfg", j);
				
				#ifndef NDEBUG
					if (gPrintfSuppress == PSL_DEBUG)
						printf_progress("inst", j + 1, entry->numInst);
				#endif
				
				if (bank->instruments[j] == 0)
					instrument = NULL;
				else
					instrument = SegmentedToVirtual(0x1, ReadBE(bank->instruments[j]));
				Rom_Config_Instrument(rom, config, instrument, "instrument", output, off);
				strcpy(sBankFiles[sBankNum++], output);
				Assert(sBankNum < 1024 * 5);
			}
			
			Dir_Leave();
		}
		
		if (entry->numSfx) {
			Dir_Enter("sfx/");
			
			#ifndef NDEBUG
				printf_debug_align("dump", "sfx");
			#endif
			
			for (s32 j = 0; j < ReadBE(entry->numSfx); j++) {
				char* output = Dir_File("%d-Sfx.cfg", j);
				sfx = SegmentedToVirtual(0x1, ReadBE(bank->sfx));
				
				#ifndef NDEBUG
					if (gPrintfSuppress == PSL_DEBUG)
						printf_progress("sfx", j + 1, ReadBE(entry->numSfx));
				#endif
				
				Rom_Config_Sfx(rom, config, &sfx[j], "Sound Effect", output, off);
				strcpy(sBankFiles[sBankNum++], output);
				Assert(sBankNum < 1024 * 5);
			}
			
			Dir_Leave();
		}
		
		if (entry->numDrum) {
			Dir_Enter("drum/");
			
			#ifndef NDEBUG
				printf_debug_align("dump", "drums");
			#endif
			
			for (s32 j = 0; j < entry->numDrum; j++) {
				char* output = Dir_File("%d-Drum.cfg", j);
				u32* wow = SegmentedToVirtual(0x1, ReadBE(bank->drums));
				
				#ifndef NDEBUG
					if (gPrintfSuppress == PSL_DEBUG)
						printf_progress("drum", j + 1, entry->numDrum);
				#endif
				
				if (wow[j] == 0) {
					continue;
				}
				
				drum = SegmentedToVirtual(0x1, ReadBE(wow[j]));
				
				Rom_Config_Drum(rom, config, drum, "Drum", output, off);
				strcpy(sBankFiles[sBankNum++], output);
				Assert(sBankNum < 1024 * 5);
			}
			
			Dir_Leave();
		}
		
		SetSegment(0x1, NULL);
		Dir_Leave();
	}
	Dir_Leave();
}

static void Rom_Dump_Sequences(Rom* rom, MemFile* dataFile, MemFile* config) {
	AudioEntryHead* head = SegmentedToVirtual(0x0, rom->offset.table.seqTable);
	u16* seqFontTable;
	SoundFontEntry* entry;
	RomFile romFile;
	u32 num = ReadBE(head->numEntries);
	
	Dir_Enter("sequence/");
	SetSegment(0x1, SegmentedToVirtual(0x0, rom->offset.table.seqFontTbl));
	
	MemFile_Clear(config);
	for (s32 i = 0; i < num; i++) {
		seqFontTable = SegmentedToVirtual(0x0, rom->offset.table.seqFontTbl);
		entry = &head->entries[i];
		romFile.data = SegmentedToVirtual(0x0, ReadBE(entry->romAddr) + rom->offset.segment.seqRom);
		romFile.size = ReadBE(entry->size);
		
		if (romFile.size == 0)
			continue;
		
		printf_progress("Sequence", i + 1, num);
		Rom_Extract(dataFile, romFile, Dir_File("0x%02X-%s.seq", i, gSequenceName[i]));
		
		seqFontTable = SegmentedToVirtual(0x1, ReadBE(seqFontTable[i]));
		
		Config_WriteTitle_Str(gSequenceName[i]);
		Config_WriteVar_Hex("sequence_id", i);
		Config_WriteVar_Hex("bank_id", ReadBE(seqFontTable[0]) & 0xFF);
		Config_SPrintf("\n");
	}
	MemFile_SaveFile_String(config, Dir_File("config.cfg"));
	
	SetSegment(0x1, NULL);
	
	Dir_Leave();
}

static void Rom_Dump_Samples_PatchWavFiles(MemFile* dataFile, MemFile* config) {
	#define NOTE(note, octave) (note + (12 * (octave)))
	u8* instInfo;
	u32* smplInfo;
	const struct {
		u8 basenote;
		s8 finetune;
		const N64AudioInfo* info;
	} info[] = {
		{ NOTE(0, 3), 0, &gSampleInfo[367] }, // LowPerc
		{ NOTE(1, 3), 0, &gSampleInfo[368] }, // Snare
		{ NOTE(3, 3), 0, &gSampleInfo[369] }, // SoftSnare
		{ NOTE(1, 4), 0, &gSampleInfo[370] }, // Cymbal
		{ NOTE(0, 6), 0, &gSampleInfo[371] }, // Timpani
		{ NOTE(5, 3), 0, &gSampleInfo[405] }, // Gong
		{ NOTE(0, 6), 0, &gSampleInfo[406] }, // WindChimes
		
		{ NOTE(0, 5), 0, &gSampleInfo[413] }, // CongaOpen
		{ NOTE(0, 5), 0, &gSampleInfo[414] }, // CongaSoft
		{ NOTE(8, 4), 0, &gSampleInfo[415] }, // CongaMute
		
		{ NOTE(5, 3), 0, &gSampleInfo[393] }, // LuteA
		{ NOTE(7, 3), 0, &gSampleInfo[394] }, // LuteB
		
		{ NOTE(11, 3), 0, &gSampleInfo[378] }, // Tambourine
		{ NOTE(0, 4), 0, &gSampleInfo[120] }, // Tambourine
		{ NOTE(2, 4), 0, &gSampleInfo[121] }, // Tambourine
		
		{ NOTE(0, 3), 0, &gSampleInfo[432] }, // Cajon
		{ NOTE(2, 3), 0, &gSampleInfo[433] }, // Cajon
	};
	
	for (s32 i = 0; i < ArrayCount(info); i++) {
		printf_progress("Update Sample", i + 1, ArrayCount(info));
		char* file = Dir_File("%s/Sample.wav", info[i].info->name);
		
		MemFile_Clear(dataFile);
		MemFile_LoadFile(dataFile, file);
		instInfo = Lib_MemMem(dataFile->data, dataFile->dataSize, "inst", 4);
		smplInfo = Lib_MemMem(dataFile->data, dataFile->dataSize, "smpl", 4);
		
		/* basenote */ instInfo[8] = info[i].basenote;
		/* finetune */ instInfo[9] = info[i].finetune;
		/* basenote */ smplInfo[5] = info[i].basenote;
		/* finetune */ smplInfo[6] = info[i].finetune;
		MemFile_SaveFile(dataFile, file);
	}
	#undef NOTE
}

static void Rom_Dump_Samples(Rom* rom, MemFile* dataFile, MemFile* config) {
	SampleInfo* smallest = sUnsortedSampleTbl;
	SampleInfo* largest = sUnsortedSampleTbl;
	RomFile rf;
	SampleInfo** tbl;
	AdpcmLoop* loop;
	AdpcmBook* book;
	char buff[16];
	char* name;
	u32 sampRate;
	char sysbuf[256 * 2];
	
	printf_debug("sort I");
	
	for (s32 i = 0; i < sDumpID; i++) {
		if (smallest->sampleAddr > sUnsortedSampleTbl[i].sampleAddr) {
			smallest = &sUnsortedSampleTbl[i];
		}
		if (largest->sampleAddr < sUnsortedSampleTbl[i].sampleAddr) {
			largest = &sUnsortedSampleTbl[i];
		}
	}
	sSortedSampleTbl[sSortID++] = smallest;
	printf_debug("sort II");
	
	while (1) {
		smallest = largest;
		
		for (s32 i = 0; i < sDumpID; i++) {
			if (sUnsortedSampleTbl[i].sampleAddr < smallest->sampleAddr) {
				if (sUnsortedSampleTbl[i].sampleAddr > sSortedSampleTbl[sSortID - 1]->sampleAddr) {
					smallest = &sUnsortedSampleTbl[i];
				}
			}
		}
		sSortedSampleTbl[sSortID++] = smallest;
		if (smallest->sampleAddr == largest->sampleAddr)
			break;
	}
	printf_debug("sort OK");
	
	tbl = sSortedSampleTbl;
	
	Dir_Enter("sample/"); {
		for (s32 i = 0; i < sSortID; i++) {
			printf_progress("Sample", i + 1, sSortID);
			name = gSampleInfo[i].dublicate == NULL ? gSampleInfo[i].name : gSampleInfo[i].dublicate->name;
			sampRate = gSampleInfo[i].dublicate == NULL ? gSampleInfo[i].sampleRate : gSampleInfo[i].dublicate->sampleRate;
			
			if (name == NULL)
				printf_error("Sample ID [%D] is missing name", i);
			
			if (sampRate == 0)
				printf_error("Sample [%s] is missing samplerate", name);
			
			Dir_Enter("%s/", name); {
				book = SegmentedToVirtual(0x0, tbl[i]->book);
				loop = SegmentedToVirtual(0x0, tbl[i]->loop + 0x10);
				
				printf_debug("Book %08X", tbl[i]->book);
				printf_debug("Loop %08X", tbl[i]->book);
				printf_debug("Smpl %08X", tbl[i]->sampleAddr);
				
				rf.size = ReadBE(tbl[i]->data) & 0x00FFFFFF;
				rf.data = SegmentedToVirtual(0x0, tbl[i]->sampleAddr);
				Rom_Extract(dataFile, rf, Dir_File("vadpcm.bin"));
				
				rf.size = sizeof(s16) * 8 * ReadBE(book->order) * ReadBE(book->npredictors) + 8;
				rf.data = book;
				Rom_Extract(dataFile, rf, Dir_File("book.bin"));
				
				Rom_Config_Sample(rom, config, (Sample*)tbl[i], name, Dir_File("config.cfg"));
				
				if (loop->count) {
					rf.size = 0x20;
					rf.data = loop;
					Rom_Extract(dataFile, rf, Dir_File("loopbook.bin"));
				}
				
				#ifndef _WIN32
					strcpy(sysbuf, "./tools/z64audio --i ");
				#else
					strcpy(sysbuf, "tools\\z64audio.exe --i ");
				#endif
				strcat(sysbuf, Dir_File("vadpcm.bin"));
				strcat(sysbuf, " --o ");
				strcat(sysbuf, Dir_File("Sample.wav"));
				
				#ifndef NDEBUG
					if (gPrintfSuppress == PSL_DEBUG)
						strcat(sysbuf, " --D");
					else
						strcat(sysbuf, " --S");
				#else
					strcat(sysbuf, " --S");
				#endif
				
				strcat(sysbuf, " --srate ");
				strcat(sysbuf, tprintf("%d", sampRate));
				strcat(sysbuf, " --tuning ");
				strcat(sysbuf, tprintf("%f", tbl[i]->tuning));
				
				if (tbl[i]->isPrim && (tbl[i]->splitHi != 127 || tbl[i]->splitLo != 0)) {
					strcat(sysbuf, " --split-hi ");
					strcat(sysbuf, tprintf("%d", tbl[i]->splitHi + 21));
					if (tbl[i]->splitLo) {
						strcat(sysbuf, " --split-lo ");
						strcat(sysbuf, tprintf("%d", tbl[i]->splitLo + 21));
					}
				}
				
				if (system(sysbuf))
					printf_error(sysbuf);
				
				MemFile_Clear(dataFile);
				s8* instInfo;
				
				if (MemFile_LoadFile(dataFile, Dir_File("Sample.wav"))) {
					printf_warning_align("Sample not found", "%s", Dir_File("Sample.wav"));
					getchar();
				}
				
				instInfo = Lib_MemMem(dataFile->data, dataFile->dataSize, "inst", 4);
				
				if (instInfo) {
					Config_SPrintf("\n # Instrument Info\n");
					Config_WriteVar_Int("basenote", instInfo[8]);
					Config_WriteVar_Int("finetune", instInfo[9]);
					MemFile_SaveFile(config, Dir_File("config.cfg"));
				} else {
					if (dataFile->dataSize == 0)
						printf_warning_align("Audio", "Empty File [%s]", Dir_File("Sample.wav"));
				}
			} Dir_Leave();
		}
		
		for (s32 j = 0; j < sBankNum; j++) {
			char* replacedName = NULL;
			printf_progress("Update SoundFont", j + 1, sBankNum);
			
			MemFile_Clear(config);
			MemFile_LoadFile_String(config, sBankFiles[j]);
			for (s32 i = 0; i < sSortID; i++) {
				name = gSampleInfo[i].dublicate == NULL ? gSampleInfo[i].name : gSampleInfo[i].dublicate->name;
				sprintf(buff, "0x%X", sSortedSampleTbl[i]->sampleAddr);
				if (String_Replace(config->data, buff, name)) {
					replacedName = name;
				}
			}
			
			config->dataSize = strlen(config->data);
			MemFile_SaveFile_String(config, sBankFiles[j]);
			
			// Rename SFX To their samples
			if (String_MemMem(sBankFiles[j], "-Sfx")) {
				char* tempName = tprintf("%s%d-%s.cfg", String_GetPath(sBankFiles[j]), String_GetInt(String_GetBasename(sBankFiles[j])), replacedName);
				printf_debug_align(
					"Rename",
					"[%s] -> [%s]",
					sBankFiles[j],
					tempName
				);
				renamer_remove(sBankFiles[j], tempName);
			}
			
			// Rename Inst to their primary sample
			if ((String_MemMem(sBankFiles[j], "-Inst") || String_MemMem(sBankFiles[j], "-Drum")) && (String_MemMem(config->data, "Inst_") || String_MemMem(config->data, "Perc_"))) {
				char instName[256] = { 0 };
				char* tempName;
				
				strcpy(instName, Config_GetString(config, "prim_sample"));
				String_Remove(instName, strlen("Inst_"));
				String_Replace(instName, "_Prim", "");
				String_Replace(instName, "Soft", "");
				String_Replace(instName, "Hard", "");
				String_Replace(instName, "Mute", "");
				String_Replace(instName, "Open", "");
				String_Replace(instName, "_Hi", "Var");
				
				if (instName[0] == 0)
					printf_error("String maniplation failed for instrument");
				
				tempName = tprintf("%s%d-%s.cfg", String_GetPath(sBankFiles[j]), String_GetInt(String_GetBasename(sBankFiles[j])), instName);
				
				printf_debug_align(
					"Rename",
					"[%s] -> [%s]",
					sBankFiles[j],
					tempName
				);
				renamer_remove(sBankFiles[j], tempName);
			}
		}
		
		Rom_Dump_Samples_PatchWavFiles(dataFile, config);
	} Dir_Leave();
}

void Rom_Dump(Rom* rom) {
	MemFile dataFile = MemFile_Initialize();
	MemFile config = MemFile_Initialize();
	RomFile rf;
	
	Dir_SetParam(DIR__MAKE_ON_ENTER);
	MemFile_Malloc(&dataFile, 0x460000); // Slightly larger than audiotable
	MemFile_Malloc(&config, 0x25000);
	
	printf_info_align("Dump Rom", PRNT_YELW "%s", rom->file.info.name);
	Dir_Enter("rom/");
	#if 1
		Dir_Enter("actor/"); {
			for (s32 i = 0; i < rom->table.num.actor; i++) {
				rf = Dma_RomFile_Actor(rom, i);
				
				if (rf.size == 0)
					continue;
				
				printf_progress("Actor", i + 1, rom->table.num.actor);
				Dir_Enter("0x%04X-%s/", i, gActorName[i]); {
					if (Rom_Extract(&dataFile, rf, Dir_File("actor.zovl")))
						Rom_Config_Actor(&config, &rom->table.actor[i], gActorName[i], Dir_File("config.cfg"));
				} Dir_Leave();
			}
		} Dir_Leave();
		
		Dir_Enter("object/"); {
			for (s32 i = 0; i < rom->table.num.obj; i++) {
				rf = Dma_RomFile_Object(rom, i);
				
				if (rf.size == 0)
					continue;
				
				printf_progress("Object", i + 1, rom->table.num.obj);
				Dir_Enter("0x%04X-%s/", i, gObjectName[i]); {
					Rom_Extract(&dataFile, rf, Dir_File("object.zobj"));
				} Dir_Leave();
			}
		} Dir_Leave();
		
		Dir_Enter("system/"); {
			for (s32 i = 0; i < rom->table.num.state; i++) {
				rf = Dma_RomFile_GameState(rom, i);
				
				if (rf.size == 0)
					continue;
				
				printf_progress("System", i + 1, rom->table.num.state);
				Dir_Enter("GameState_%s/", gStateName[i]); {
					if (Rom_Extract(&dataFile, rf, Dir_File("state.zovl")))
						Rom_Config_GameState(&config, &rom->table.state[i], gStateName[i], Dir_File("config.cfg"));
				} Dir_Leave();
			}
			
			printf_info("Player");
			Dir_Enter("Player/"); {
				rf.size = ReadBE(rom->table.kaleido[1].vromEnd) - ReadBE(rom->table.kaleido[1].vromStart);
				rf.data = SegmentedToVirtual(0x0, ReadBE(rom->table.kaleido[1].vromStart));
				
				Rom_Extract(&dataFile, rf, Dir_File("EnPlayer.zovl"));
				Rom_Config_Player(rom, &config, &rom->table.kaleido[1], "Player", Dir_File("config.cfg"));
			} Dir_Leave();
		} Dir_Leave();
		
		Dir_Enter("scene/"); {
			for (s32 i = 0; i < rom->table.num.scene; i++) {
				rf = Dma_RomFile_Scene(rom, i);
				
				if (rf.size == 0)
					continue;
				
				printf_progress("Scene", i + 1, rom->table.num.scene);
				Dir_Enter("0x%02X-%s/", i, gSceneName[i]); {
					if (Rom_Extract(&dataFile, rf, Dir_File("scene.zscene"))) {
						u32* seg;
						u32 roomNum;
						u32 roomListSeg;
						u32* vromSeg;
						
						Rom_Config_Scene(&config, &rom->table.scene[i], gSceneName[i], Dir_File("config.cfg"));
						SetSegment(0x2, rf.data);
						seg = dataFile.data;
						
						while (1) {
							if ((seg[0] & 0xFF) == 0x04) {
								break;
							}
							if ((seg[0] & 0xFF) == 0x14) {
								printf_warning_align("Scene", "Failed finding room list");
								seg = 0;
								break;
							}
							seg++;
						}
						
						if (seg) {
							roomNum = (seg[0] & 0xFF00) >> 8;
							roomListSeg = ReadBE(seg[1]) & 0xFFFFFF;
							
							for (s32 j = 0; j < roomNum; j++) {
								char* out = Dir_File("room_%d.zroom", j);
								
								vromSeg = SegmentedToVirtual(0x2, roomListSeg + 8 * j);
								printf_debugExt_align("Room Extract", "Scene Segment %X", VirtualToSegmented(0x2, vromSeg));
								Rom_Extract(
									&dataFile,
									Rom_GetRomFile(rom, vromSeg[0], vromSeg[1]),
									out
								);
							}
						}
					}
				} Dir_Leave();
			}
		} Dir_Leave();
		SetSegment(0x2, NULL);
	#endif
	
	Dir_Enter("sound/"); {
		Rom_Dump_SoundFont(rom, &dataFile, &config);
		Rom_Dump_Sequences(rom, &dataFile, &config);
		Rom_Dump_Samples(rom, &dataFile, &config);
	} Dir_Leave();
	
	Dir_Leave();
	
	MemFile_Free(&dataFile);
	MemFile_Free(&config);
}

/* / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / */

struct {
	char name[32];
	u32  segment;
	u32  size;
} sSampleTbl[1024 * 5];
s32 sSampleTblNum;

static void Rom_Build_SoundFont_Book(u32* numBookListPtr, u32* crcBookList, u32* seekBookList, u32* bookIndex, MemFile* dataFile, MemFile* memBook, char* sample) {
	u32 numBookList = *numBookListPtr;
	
	if (numBookList == 0) {
		MemFile_Clear(dataFile);
		if (MemFile_LoadFile(dataFile, Dir_File("%s/book.bin", sample)))
			printf_error("Could not load file [%s]", Dir_File("%s/book.bin", sample));
		crcBookList[numBookList] = memBook->info.crc32;
		seekBookList[numBookList] = memBook->seekPoint;
		bookIndex[numBookList] = numBookList;
		MemFile_Append(memBook, dataFile);
		numBookList++;
	} else {
		u32 new = 1;
		MemFile_Clear(dataFile);
		if (MemFile_LoadFile(dataFile, Dir_File("%s/book.bin", sample)))
			printf_error("Could not load file [%s]", Dir_File("%s/book.bin", sample));
		
		for (s32 l = 0; l < numBookList; l++) {
			if (memBook->info.crc32 == crcBookList[l]) {
				new = 0;
				bookIndex[numBookList] = l;
			}
		}
		
		if (new) {
			crcBookList[numBookList] = memBook->info.crc32;
			seekBookList[numBookList] = memBook->seekPoint;
			bookIndex[numBookList] = numBookList;
			MemFile_Append(memBook, dataFile);
			numBookList++;
		}
	}
	
	*numBookListPtr = numBookList;
}

static void Rom_Build_SoundFont(Rom* rom, MemFile* dataFile, MemFile* config) {
	ItemList itemList;
	MemFile memBank = MemFile_Initialize();
	MemFile memBook = MemFile_Initialize();
	MemFile memLoopBook = MemFile_Initialize();
	MemFile memInst = MemFile_Initialize();
	MemFile memEnv = MemFile_Initialize();
	MemFile memSample = MemFile_Initialize();
	MemFile memSfx = MemFile_Initialize();
	MemFile memDrum = MemFile_Initialize();
	AudioEntryHead* head = SegmentedToVirtual(0x0, rom->offset.table.fontTable);
	
	rom->file.seekPoint = rom->offset.segment.fontRom;
	
	MemFile_Malloc(&memBank, MbToBin(0.25));
	MemFile_Malloc(&memBook, MbToBin(0.25));
	MemFile_Malloc(&memLoopBook, MbToBin(0.25));
	MemFile_Malloc(&memInst, MbToBin(0.25));
	MemFile_Malloc(&memEnv, MbToBin(0.25));
	MemFile_Malloc(&memSample, MbToBin(0.25));
	MemFile_Malloc(&memSfx, MbToBin(0.25));
	MemFile_Malloc(&memDrum, MbToBin(0.25));
	
	MemFile_Params(dataFile, MEM_CLEAR, MEM_END);
	MemFile_Params(dataFile, MEM_CRC32, true, MEM_ALIGN, 16, MEM_END);
	Dir_ItemList(&itemList, true);
	
	head->numEntries = itemList.num;
	SwapBE(head->numEntries);
	
	for (s32 i = 0; i < itemList.num; i++) {
		ItemList listInst = { 0 };
		ItemList listSfx = { 0 };
		ItemList listDrum = { 0 };
		printf_progress("Build SoundFont", i + 1, itemList.num);
		
		MemFile_Clear(&memBank);
		MemFile_Clear(&memBook);
		MemFile_Clear(&memLoopBook);
		MemFile_Clear(&memInst);
		MemFile_Clear(&memEnv);
		MemFile_Clear(&memSample);
		MemFile_Clear(&memSfx);
		MemFile_Clear(&memDrum);
		MemFile_Clear(dataFile);
		MemFile_Clear(config);
		
		SetSegment(0x4, memBank.data);
		
		Dir_Enter(itemList.item[i]); {
			u32 numEnvList = 0;
			u16* envIndex;
			
			u32 numBookList = 0;
			u32* bookIndex;
			u32* seekBookList;
			u32* crcBookList;
			
			u32 smplNum = 0;
			Adsr* envList;
			char* restoreDir = Graph_Alloc(strlen(Dir_Current()));
			
			strcpy(restoreDir, Dir_Current());
			
			if (Dir_Stat("instrument/")) {
				Dir_Enter("instrument/");
				Dir_ItemList(&listInst, false);
				ItemList_NumericalSort(&listInst);
				Dir_Leave();
			}
			
			if (Dir_Stat("sfx/")) {
				Dir_Enter("sfx/");
				Dir_ItemList(&listSfx, false);
				ItemList_NumericalSort(&listSfx);
				Dir_Leave();
			}
			
			if (Dir_Stat("drum/")) {
				Dir_Enter("drum/");
				Dir_ItemList(&listDrum, false);
				ItemList_NumericalSort(&listDrum);
				Dir_Leave();
			}
			
			MemFile_Write(&memBank, "\0\0\0\0", 4);
			MemFile_Write(&memBank, "\0\0\0\0", 4);
			
			envIndex = Graph_Alloc(2 * listInst.num);
			envList = Graph_Alloc(16 * listInst.num);
			seekBookList = Graph_Alloc(4 * 3 * listInst.num + 4 * listSfx.num + 4 * listDrum.num);
			crcBookList = Graph_Alloc(4 * 3 * listInst.num + 4 * listSfx.num + 4 * listDrum.num);
			bookIndex = Graph_Alloc(4 * 3 * listInst.num + 4 * listSfx.num + 4 * listDrum.num);
			
			for (s32 j = 0; j < listInst.num; j++) {
				char* restoreDir = Graph_Alloc(strlen(Dir_Current()));
				Instrument instruments = { 0 };
				Adsr confEnv[4] = { 0 };
				u32 req = 3;
				
				strcpy(restoreDir, Dir_Current());
				
				MemFile_Clear(config);
				MemFile_LoadFile(config, Dir_File("instrument/%s", listInst.item[j]));
				
				char* low = Config_GetString(config, "low_sample");
				char* prim = Config_GetString(config, "prim_sample");
				char* hi = Config_GetString(config, "hi_sample");
				
				if (!String_IsDiff(low, "NULL"))
					req--;
				if (!String_IsDiff(prim, "NULL"))
					req--;
				if (!String_IsDiff(hi, "NULL"))
					req--;
				
				if (req == 0) {
					SwapBE(memInst.seekPoint);
					MemFile_Write(&memBank, &memInst.seekPoint, sizeof(u32));
					SwapBE(memInst.seekPoint);
					
					MemFile_Write(&memInst, &instruments, sizeof(struct Instrument));
					continue;
				}
				
				for (s32 k = 0; k < sSampleTblNum; k++) {
					if (low && !String_IsDiff(sSampleTbl[k].name, low)) {
						instruments.lo.tuning = Config_GetFloat(config, "low_tuning");
						SwapBE(instruments.lo.swap32);
						req--;
						low = NULL;
					}
					if (prim && !String_IsDiff(sSampleTbl[k].name, prim)) {
						instruments.prim.tuning = Config_GetFloat(config, "prim_tuning");
						SwapBE(instruments.prim.swap32);
						req--;
						prim = NULL;
					}
					if (hi && !String_IsDiff(sSampleTbl[k].name, hi)) {
						instruments.hi.tuning = Config_GetFloat(config, "hi_tuning");
						SwapBE(instruments.hi.swap32);
						req--;
						hi = NULL;
					}
					if (req == 0)
						break;
				}
				
				instruments.loaded = Config_GetInt(config, "loaded");
				instruments.splitLo = Config_GetInt(config, "split_lo");
				instruments.splitHi = Config_GetInt(config, "split_hi");
				instruments.release = Config_GetInt(config, "release");
				
				confEnv[0].rate = Config_GetInt(config, "attack_rate"); SwapBE(confEnv[0].rate);
				confEnv[0].level = Config_GetInt(config, "attack_level"); SwapBE(confEnv[0].level);
				confEnv[1].rate = Config_GetInt(config, "hold_rate"); SwapBE(confEnv[1].rate);
				confEnv[1].level = Config_GetInt(config, "hold_level"); SwapBE(confEnv[1].level);
				confEnv[2].rate = Config_GetInt(config, "decay_rate"); SwapBE(confEnv[2].rate);
				confEnv[2].level = Config_GetInt(config, "decay_level"); SwapBE(confEnv[2].level);
				
				if (numEnvList == 0) {
					memcpy(&envList[0], confEnv, 16);
					envIndex[numEnvList] = numEnvList;
					MemFile_Write(&memEnv, &envList[numEnvList * 4], sizeof(struct Adsr) * 4);
					numEnvList++;
				} else {
					u32 new = 1;
					for (s32 k = 0; k < numEnvList; k++) {
						if (memcmp(&envList[k * 4], confEnv, 16) == 0) {
							envIndex[numEnvList] = k;
							new = 0;
							break;
						}
					}
					
					if (new) {
						memcpy(&envList[numEnvList * 4], confEnv, 16);
						envIndex[numEnvList] = numEnvList;
						MemFile_Write(&memEnv, &envList[numEnvList * 4], sizeof(struct Adsr) * 4);
						numEnvList++;
					}
				}
				
				Dir_Leave(); // soundfont/
				Dir_Leave(); // sound/;
				Dir_Enter("sample/");
				for (s32 k = 0; k < 3; k++) {
					char* sample;
					Sample smpl = { 0 };
					
					switch (k) {
						case 0:
							sample = Config_GetString(config, "low_sample");
							break;
						case 1:
							sample = Config_GetString(config, "prim_sample");
							break;
						case 2:
							sample = Config_GetString(config, "hi_sample");
							break;
					}
					
					if (!String_IsDiff(sample, "NULL"))
						continue;
					
					MemFile_Clear(dataFile);
					if (MemFile_LoadFile(dataFile, Dir_File("%s/book.bin", sample)))
						printf_error("Could not load file [%s]", Dir_File("%s/book.bin", sample));
					
					Rom_Build_SoundFont_Book(&numBookList, crcBookList, seekBookList, bookIndex, dataFile, &memBook, sample);
					
					MemFile_Clear(dataFile);
					MemFile_LoadFile(dataFile, Dir_File("%s/config.cfg", sample));
					
					s32 l = 0;
					for (; l < sSampleTblNum; l++) {
						if (!String_IsDiff(sSampleTbl[l].name, sample))
							break;
					}
					
					smpl.sampleAddr = ReadBE(sSampleTbl[l].segment);
					smpl.data = sSampleTbl[l].size;
					smpl.data |= Config_GetInt(dataFile, "codec") << (32 - 4);
					smpl.data |= Config_GetInt(dataFile, "medium") << (32 - 6);
					smpl.data |= Config_GetInt(dataFile, "bitA") << (32 - 7);
					smpl.data |= Config_GetInt(dataFile, "bitB") << (32 - 8);
					SwapBE(smpl.data);
					
					switch (k) {
						case 0:
							instruments.lo.sample = memSample.seekPoint;
							break;
						case 1:
							instruments.prim.sample = memSample.seekPoint;
							break;
						case 2:
							instruments.hi.sample = memSample.seekPoint;
							break;
					}
					
					AdpcmLoop loop = {
						Config_GetInt(dataFile, "loop_start"),
						Config_GetInt(dataFile, "loop_end"),
						Config_GetInt(dataFile, "loop_count"),
						Config_GetInt(dataFile, "tail_end"),
					};
					SwapBE(loop.start);
					SwapBE(loop.end);
					SwapBE(loop.count);
					SwapBE(loop.origSpls);
					smpl.loop = memLoopBook.seekPoint;
					if (!Lib_MemMem(memLoopBook.data, memLoopBook.dataSize, &loop, 4 * 4)) {
						MemFile_Write(&memLoopBook, &loop, sizeof(u32) * 4);
						if (loop.count != 0) {
							MemFile_Clear(dataFile);
							MemFile_LoadFile(dataFile, Dir_File("%s/loopbook.bin", sample));
							MemFile_Append(&memLoopBook, dataFile);
						}
					} else {
						void* ptr = Lib_MemMem(memLoopBook.data, memLoopBook.dataSize, &loop, 4 * 4);
						smpl.loop = (uPtr)memLoopBook.data - (uPtr)ptr;
					}
					MemFile_Write(&memSample, &smpl, sizeof(struct Sample));
					smplNum++;
				}
				Dir_Set(restoreDir);
				
				SwapBE(memInst.seekPoint);
				MemFile_Write(&memBank, &memInst.seekPoint, sizeof(u32));
				SwapBE(memInst.seekPoint);
				
				MemFile_Write(&memInst, &instruments, sizeof(struct Instrument));
			}
			
			for (s32 j = 0; j < listSfx.num; j++) {
				char* restoreDir = Graph_Alloc(strlen(Dir_Current()));
				Sound sfx = { 0 };
				
				strcpy(restoreDir, Dir_Current());
				
				MemFile_Clear(config);
				MemFile_Params(config, MEM_CLEAR, MEM_END);
				MemFile_LoadFile(config, Dir_File("sfx/%s", listSfx.item[j]));
				
				if (!String_IsDiff(Config_GetString(config, "prim_sample"), "NULL")) {
					MemFile_Write(&memSfx, &sfx, sizeof(struct Sound));
					continue;
				}
				
				sfx.tuning = Config_GetFloat(config, "prim_tuning");
				SwapBE(sfx.swap32);
				
				Dir_Leave(); // soundfont/
				Dir_Leave(); // sound/;
				Dir_Enter("sample/"); {
					char* sample = Config_GetString(config, "prim_sample");
					Sample smpl = { 0 };
					
					Rom_Build_SoundFont_Book(&numBookList, crcBookList, seekBookList, bookIndex, dataFile, &memBook, sample);
					
					MemFile_Clear(config);
					MemFile_LoadFile(config, Dir_File("%s/config.cfg", sample));
					
					s32 l = 0;
					for (; l < sSampleTblNum; l++) {
						if (!String_IsDiff(sSampleTbl[l].name, sample))
							break;
					}
					
					smpl.sampleAddr = ReadBE(sSampleTbl[l].segment);
					smpl.data = sSampleTbl[l].size;
					smpl.data |= Config_GetInt(config, "codec") << (32 - 4);
					smpl.data |= Config_GetInt(config, "medium") << (32 - 6);
					smpl.data |= Config_GetInt(config, "bitA") << (32 - 7);
					smpl.data |= Config_GetInt(config, "bitB") << (32 - 8);
					SwapBE(smpl.data);
					sfx.sample = memSample.seekPoint;
					
					AdpcmLoop loop = {
						Config_GetInt(config, "loop_start"),
						Config_GetInt(config, "loop_end"),
						Config_GetInt(config, "loop_count"),
						Config_GetInt(config, "tail_end"),
					};
					SwapBE(loop.start);
					SwapBE(loop.end);
					SwapBE(loop.count);
					SwapBE(loop.origSpls);
					smpl.loop = memLoopBook.seekPoint;
					if (!Lib_MemMem(memLoopBook.data, memLoopBook.dataSize, &loop, 4 * 4)) {
						MemFile_Write(&memLoopBook, &loop, sizeof(u32) * 4);
						if (loop.count != 0) {
							MemFile_Clear(dataFile);
							MemFile_LoadFile(dataFile, Dir_File("%s/loopbook.bin", sample));
							MemFile_Append(&memLoopBook, dataFile);
						}
					} else {
						void* ptr = Lib_MemMem(memLoopBook.data, memLoopBook.dataSize, &loop, 4 * 4);
						smpl.loop = (uPtr)memLoopBook.data - (uPtr)ptr;
					}
					MemFile_Write(&memSample, &smpl, sizeof(struct Sample));
					smplNum++;
				} Dir_Set(restoreDir);
				
				MemFile_Write(&memSfx, &sfx, sizeof(struct Sound));
			}
			
			MemFile_Params(&memDrum, MEM_CLEAR, MEM_END);
			for (s32 j = 0; j < listDrum.num; j++) {
				u32 val = 0x10 * j;
				MemFile_Write(&memDrum, &val, 4);
			}
			
			MemFile_Align(&memDrum, 16);
			MemFile_Params(&memDrum, MEM_ALIGN, 16, MEM_END);
			
			for (s32 j = 0; j < listDrum.num; j++) {
				memDrum.cast.u32[j] += memDrum.seekPoint;
			}
			
			for (s32 j = 0; j < listDrum.num; j++) {
				char* restoreDir = Graph_Alloc(strlen(Dir_Current()));
				DrumAlt drum = { 0 };
				Adsr confEnv[4] = { 0 };
				char currentConf[512];
				
				strcpy(currentConf, Dir_File("drum/%s", listDrum.item[j]));
				strcpy(restoreDir, Dir_Current());
				
				MemFile_Clear(config);
				MemFile_LoadFile(config, Dir_File("drum/%s", listDrum.item[j]));
				
				if (!String_IsDiff(Config_GetString(config, "prim_sample"), "NULL")) {
					MemFile_Write(&memDrum, &drum, sizeof(struct DrumAlt));
					continue;
				}
				
				drum.sound.tuning = Config_GetFloat(config, "prim_tuning");
				drum.loaded = Config_GetInt(config, "loaded");
				drum.pan = Config_GetInt(config, "pan");
				drum.release = Config_GetInt(config, "release");
				SwapBE(drum.sound.swap32);
				
				confEnv[0].rate = Config_GetInt(config, "attack_rate"); SwapBE(confEnv[0].rate);
				confEnv[0].level = Config_GetInt(config, "attack_level"); SwapBE(confEnv[0].level);
				confEnv[1].rate = Config_GetInt(config, "hold_rate"); SwapBE(confEnv[1].rate);
				confEnv[1].level = Config_GetInt(config, "hold_level"); SwapBE(confEnv[1].level);
				confEnv[2].rate = Config_GetInt(config, "decay_rate"); SwapBE(confEnv[2].rate);
				confEnv[2].level = Config_GetInt(config, "decay_level"); SwapBE(confEnv[2].level);
				
				drum.envelope = numEnvList;
				
				if (numEnvList == 0) {
					memcpy(&envList[0], confEnv, 16);
					envIndex[numEnvList] = numEnvList;
					MemFile_Write(&memEnv, &envList[numEnvList * 4], sizeof(struct Adsr) * 4);
					numEnvList++;
				} else {
					u32 new = 1;
					for (s32 k = 0; k < numEnvList; k++) {
						if (memcmp(&envList[k * 4], confEnv, 16) == 0) {
							envIndex[numEnvList] = k;
							new = 0;
							break;
						}
					}
					
					if (new) {
						memcpy(&envList[numEnvList * 4], confEnv, 16);
						envIndex[numEnvList] = numEnvList;
						MemFile_Write(&memEnv, &envList[numEnvList * 4], sizeof(struct Adsr) * 4);
						numEnvList++;
					}
				}
				
				Dir_Leave(); // soundfont/
				Dir_Leave(); // sound/;
				Dir_Enter("sample/"); {
					char* sample = Config_GetString(config, "prim_sample");
					Sample smpl = { 0 };
					
					Rom_Build_SoundFont_Book(&numBookList, crcBookList, seekBookList, bookIndex, dataFile, &memBook, sample);
					
					MemFile_Clear(config);
					MemFile_LoadFile(config, Dir_File("%s/config.cfg", sample));
					
					s32 l = 0;
					for (; l < sSampleTblNum; l++) {
						if (!String_IsDiff(sSampleTbl[l].name, sample))
							break;
					}
					
					smpl.sampleAddr = ReadBE(sSampleTbl[l].segment);
					smpl.data = sSampleTbl[l].size;
					smpl.data |= Config_GetInt(config, "codec") << (32 - 4);
					smpl.data |= Config_GetInt(config, "medium") << (32 - 6);
					smpl.data |= Config_GetInt(config, "bitA") << (32 - 7);
					smpl.data |= Config_GetInt(config, "bitB") << (32 - 8);
					SwapBE(smpl.data);
					
					drum.sound.sample = memSample.seekPoint;
					AdpcmLoop loop = {
						Config_GetInt(config, "loop_start"),
						Config_GetInt(config, "loop_end"),
						Config_GetInt(config, "loop_count"),
						Config_GetInt(config, "tail_end"),
					};
					SwapBE(loop.start);
					SwapBE(loop.end);
					SwapBE(loop.count);
					SwapBE(loop.origSpls);
					smpl.loop = memLoopBook.seekPoint;
					if (!Lib_MemMem(memLoopBook.data, memLoopBook.dataSize, &loop, 4 * 4)) {
						MemFile_Write(&memLoopBook, &loop, sizeof(u32) * 4);
						if (loop.count != 0) {
							MemFile_Clear(dataFile);
							MemFile_LoadFile(dataFile, Dir_File("%s/loopbook.bin", sample));
							MemFile_Append(&memLoopBook, dataFile);
						}
					} else {
						void* ptr = Lib_MemMem(memLoopBook.data, memLoopBook.dataSize, &loop, 4 * 4);
						
						smpl.loop = (uPtr)ptr - (uPtr)memLoopBook.data;
					}
					MemFile_Write(&memSample, &smpl, sizeof(struct Sample));
					smplNum++;
					MemFile_Clear(config);
					MemFile_LoadFile(config, currentConf);
				} Dir_Set(restoreDir);
				
				MemFile_Write(&memDrum, &drum, sizeof(struct DrumAlt));
				MemFile_Align(&memDrum, 16);
			}
			
			MemFile_Align(&memBank, 16);
			for (s32 j = 0; j < listInst.num; j++) {
				SwapBE(memBank.cast.u32[2 + j]);
				memBank.cast.u32[2 + j] += memBank.seekPoint;
				SwapBE(memBank.cast.u32[2 + j]);
			}
			MemFile_Append(&memBank, &memInst);
			MemFile_Align(&memBank, 16);
			
			for (s32 j = 0; j < listInst.num; j++) {
				Instrument* inst = SegmentedToVirtual(0x4, ReadBE(memBank.cast.u32[2 + j]));
				
				inst->envelope = memBank.seekPoint + sizeof(struct Adsr) * 4 * envIndex[j];
				
				SwapBE(inst->envelope);
			}
			for (s32 j = 0; j < listDrum.num; j++) {
				SetSegment(0x5, memDrum.data);
				DrumAlt* drum = SegmentedToVirtual(0x5, memDrum.cast.u32[j]);
				u32 envID = drum->envelope;
				
				drum->envelope = memBank.seekPoint + sizeof(struct Adsr) * 4 * envIndex[envID];
				SwapBE(drum->envelope);
			}
			MemFile_Append(&memBank, &memEnv);
			MemFile_Align(&memBank, 16);
			
			for (s32 l = 0; l < smplNum; l++) {
				Sample* smpl = memSample.data;
				
				smpl[l].book = memBook.seekPoint + seekBookList[bookIndex[l]];
				SwapBE(smpl[l].book);
			}
			
			MemFile_Append(&memBank, &memBook);
			MemFile_Align(&memBank, 16);
			
			for (s32 l = 0; l < smplNum; l++) {
				Sample* smpl = memSample.data;
				
				smpl[l].loop += memBank.seekPoint;
				SwapBE(smpl[l].loop);
			}
			
			MemFile_Append(&memBank, &memLoopBook);
			MemFile_Align(&memBank, 16);
			
			for (s32 j = 0; j < listInst.num; j++) {
				Instrument* inst = SegmentedToVirtual(0x4, ReadBE(memBank.cast.u32[2 + j]));
				
				if (inst->lo.tuning) {
					inst->lo.sample += memBank.seekPoint;
				}
				if (inst->prim.tuning) {
					inst->prim.sample += memBank.seekPoint;
				}
				if (inst->hi.tuning) {
					inst->hi.sample += memBank.seekPoint;
				}
				
				SwapBE(inst->lo.sample);
				SwapBE(inst->prim.sample);
				SwapBE(inst->hi.sample);
			}
			
			for (s32 j = 0; j < listSfx.num; j++) {
				Sound* sound = memSfx.data;
				
				sound[j].sample += memBank.seekPoint;
				SwapBE(sound[j].sample);
			}
			
			for (s32 j = 0; j < listDrum.num; j++) {
				SetSegment(0x5, memDrum.data);
				DrumAlt* drum = SegmentedToVirtual(0x5, (memDrum.cast.u32[j]));
				
				drum->sound.sample += memBank.seekPoint;
				SwapBE(drum->sound.sample);
				SetSegment(0x5, NULL);
			}
			
			MemFile_Append(&memBank, &memSample);
			MemFile_Align(&memBank, 16);
			
			if (listSfx.num) {
				memBank.cast.u32[1] = ReadBE(memBank.seekPoint);
				MemFile_Append(&memBank, &memSfx);
				MemFile_Align(&memBank, 16);
			}
			
			if (listDrum.num) {
				for (s32 j = 0; j < listDrum.num; j++) {
					memDrum.cast.u32[j] += memBank.seekPoint;
					SwapBE(memDrum.cast.u32[j]);
				}
				memBank.cast.u32[0] = ReadBE(memBank.seekPoint);
				MemFile_Append(&memBank, &memDrum);
				MemFile_Align(&memBank, 16);
			}
		} Dir_Leave();
		
		MemFile_Align(&memBank, 16);
		
		u32 seekPoint = rom->file.seekPoint - rom->offset.segment.fontRom;
		MemFile_SaveFile(&memBank, Dir_File("%02X-Bank.bin", i));
		MemFile_Append(&rom->file, &memBank);
		MemFile_Align(&rom->file, 16);
		
		head->entries[i].romAddr = ReadBE(seekPoint);
		head->entries[i].size = memBank.dataSize;
		head->entries[i].numDrum = listDrum.num;
		head->entries[i].numInst = listInst.num;
		head->entries[i].numSfx = listSfx.num;
		// head->entries[i].audioTable1 = 0;
		SwapBE(head->entries[i].size);
		SwapBE(head->entries[i].numSfx);
	}
	
	MemFile_Free(&memBank);
	MemFile_Free(&memBook);
	MemFile_Free(&memInst);
	MemFile_Free(&memEnv);
	MemFile_Free(&memSample);
	MemFile_Free(&memSfx);
	MemFile_Free(&memDrum);
}

static void Rom_Build_SampleTable(Rom* rom, MemFile* dataFile, MemFile* config) {
	ItemList itemList;
	MemFile sample = MemFile_Initialize();
	char* statList[] = {
		"vadpcm.bin",
		"RawSound.bin",
		"sample.bin",
	};
	
	MemFile_Malloc(&sample, MbToBin(0.25));
	MemFile_Clear(config);
	MemFile_Clear(dataFile);
	Dir_ItemList(&itemList, true);
	MemFile_Params(dataFile, MEM_ALIGN, 16, MEM_END);
	
	for (s32 i = 0; i < itemList.num; i++) {
		Dir_Enter(itemList.item[i]); {
			s32 fileId = 0;
			char* file;
			
			printf_progress("Append Sample", i + 1, itemList.num);
			
			for (;;) {
				if (Dir_Stat(statList[fileId])) {
					break;
				}
				
				if (fileId >= ArrayCount(statList)) {
					printf_error_align("Could not locate sample in", "%s", Dir_Current());
				}
				
				fileId++;
			}
			
			file = Dir_File("%s", statList[fileId]);
			
			if (MemFile_LoadFile(&sample, file))
				printf_error_align("Failed to load file", "%s", file);
			
			Config_WriteVar_Hex(String_GetFolder(file), dataFile->seekPoint);
			sSampleTbl[sSampleTblNum].segment = dataFile->seekPoint;
			sSampleTbl[sSampleTblNum].size = sample.dataSize;
			strcpy(sSampleTbl[sSampleTblNum++].name, String_GetFolder(file));
			MemFile_Append(dataFile, &sample);
		} Dir_Leave();
	}
	
	rom->file.seekPoint = rom->offset.segment.smplRom;
	
	AudioEntryHead* head = SegmentedToVirtual(0x0, rom->offset.table.sampleTable);
	
	head->numEntries = 1;
	SwapBE(head->numEntries);
	for (s32 i = 0; i < 7 ; i++) {
		head->entries[i].romAddr = 0;
		head->entries[i].size = ReadBE(dataFile->dataSize);
	}
	
	MemFile_Append(&rom->file, dataFile);
	MemFile_Free(&sample);
}

void Rom_Build(Rom* rom) {
	MemFile dataFile = MemFile_Initialize();
	MemFile config = MemFile_Initialize();
	
	MemFile_Malloc(&dataFile, 0x460000);
	MemFile_Malloc(&config, 0x25000);
	
	Dir_Enter("rom/"); {
		Dir_Enter("sound/"); {
			Dir_Enter("sample/"); {
				Rom_Build_SampleTable(rom, &dataFile, &config);
			} Dir_Leave();
			
			Dir_Enter("soundfont/"); {
				Rom_Build_SoundFont(rom, &dataFile, &config);
			} Dir_Leave();
		} Dir_Leave();
	} Dir_Leave();
	
	fix_crc(rom->file.data);
	MemFile_SaveFile(&rom->file, "build.z64");
}

/* / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / * / */

void Rom_New(Rom* rom, char* romName) {
	u32* hdr;
	
	rom->file = MemFile_Initialize();
	MemFile_Params(&rom->file, MEM_FILENAME, true, MEM_END);
	if (MemFile_LoadFile(&rom->file, romName)) {
		printf_error_align("Load Rom", "%s", romName);
	}
	
	if (rom->file.dataSize < MbToBin(64)) {
		printf_warning_align("Rom Size", "[%.2fMb / %.2fMb]", BinToMb(rom->file.dataSize), 64.0f);
		printf_warning("Bad things " PRNT_REDD "might " PRNT_RSET "happen...");
		printf_warning("Do you want to continue the process? [y/n]");
		
		if (!printf_get_answer()) {
			exit(0);
		}
	}
	
	SetSegment(0x0, rom->file.data);
	hdr = SegmentedToVirtual(0x0, 0xDB70);
	
	if (hdr[0] != 0) {
		u16* addr;
		
		rom->type = Zelda_OoT_Debug;
		
		rom->offset.table.dmaTable = 0x012F70;
		rom->offset.table.objTable = 0xB9E6C8;
		rom->offset.table.actorTable = 0xB8D440;
		rom->offset.table.stateTable = 0xB969D0;
		rom->offset.table.sceneTable = 0xBA0BB0;
		rom->offset.table.kaleidoTable = 0xBA4340;
		
		rom->offset.table.seqFontTbl = 0xBCC4E0;
		rom->offset.table.seqTable = 0xBCC6A0;
		rom->offset.table.fontTable = 0xBCC270;
		rom->offset.table.sampleTable = 0xBCCD90;
		
		addr = SegmentedToVirtual(0x0, 0xB5A4AE);
		rom->offset.segment.seqRom = (ReadBE(addr[0]) - (ReadBE(addr[2]) > 0x7FFF)) << 16;
		rom->offset.segment.seqRom |= ReadBE(addr[2]);
		// rom->offset.segment.seqRom = 0x44DF0;
		printf_debug_align("SequenceRom", "%08X", rom->offset.segment.seqRom);
		
		addr = SegmentedToVirtual(0x0, 0xB5A4C2);
		rom->offset.segment.fontRom = (ReadBE(addr[0]) - (ReadBE(addr[2]) > 0x7FFF)) << 16;
		rom->offset.segment.fontRom |= ReadBE(addr[2]);
		// rom->offset.segment.fontRom = 0x19030;
		printf_debug_align("FontRom", "%08X", rom->offset.segment.fontRom);
		
		addr = SegmentedToVirtual(0x0, 0xB5A4D6);
		rom->offset.segment.smplRom = (ReadBE(addr[0]) - (ReadBE(addr[2]) > 0x7FFF)) << 16;
		rom->offset.segment.smplRom |= ReadBE(addr[2]);
		// rom->offset.segment.smplRom = 0x94870;
		printf_debug_align("SampleRom", "%08X", rom->offset.segment.smplRom);
		
		rom->table.num.dma = 1548;
		rom->table.num.obj = 402;
		rom->table.num.actor = 471;
		rom->table.num.state = 6;
		rom->table.num.scene = 110;
		rom->table.num.kaleido = 2;
		
		rom->offset.table.player.init = (HiLo) {
			0x00B288F8, 0x00B28900
		};
		rom->offset.table.player.dest = (HiLo) {
			0x00B28908, 0x00B28914
		};
		rom->offset.table.player.updt = (HiLo) {
			0x00B2891C, 0x00B28928
		};
		rom->offset.table.player.draw = (HiLo) {
			0x00B28930, 0x00B2893C
		};
	} else {
		rom->type = Zelda_OoT_1_0;
		
		rom->offset.table.dmaTable = 0x00007430;
		rom->offset.table.objTable = 0x00B6EF58;
		rom->offset.table.actorTable = 0x00B5E490;
		rom->offset.table.stateTable = 0x00B672A0;
		rom->offset.table.sceneTable = 0x00B71440;
		rom->offset.table.kaleidoTable = 0x00B743E0;
		
		rom->offset.table.seqFontTbl = 0x00B89910;
		
		rom->offset.table.seqTable = 0x00B89AD0;
		rom->offset.table.fontTable = 0x00B896A0;
		rom->offset.table.sampleTable = 0x00B8A1C0;
		
		rom->offset.segment.seqRom = 0x00029DE0;
		rom->offset.segment.fontRom = 0x0000D390;
		rom->offset.segment.smplRom = 0x00079470;
		
		rom->table.num.dma = 1526;
		rom->table.num.obj = 402;
		rom->table.num.actor = 471;
		rom->table.num.state = 6;
		rom->table.num.scene = 101;
		rom->table.num.kaleido = 2;
		
		rom->offset.table.player.init = (HiLo) {
			0x00B0D5B8, 0x00B0D5C0
		};
		rom->offset.table.player.dest = (HiLo) {
			0x00B0D5C8, 0x00B0D5D4
		};
		rom->offset.table.player.updt = (HiLo) {
			0x00B0D5DC, 0x00B0D5E8
		};
		rom->offset.table.player.draw = (HiLo) {
			0x00B0D5F0, 0x00B0D5FC
		};
	}
	
	rom->table.dma = SegmentedToVirtual(0x0, rom->offset.table.dmaTable);
	rom->table.object = SegmentedToVirtual(0x0, rom->offset.table.objTable);
	rom->table.actor = SegmentedToVirtual(0x0, rom->offset.table.actorTable);
	rom->table.state = SegmentedToVirtual(0x0, rom->offset.table.stateTable);
	rom->table.scene = SegmentedToVirtual(0x0, rom->offset.table.sceneTable);
	rom->table.kaleido = SegmentedToVirtual(0x0, rom->offset.table.kaleidoTable);
	
	#if 0 // rezimodnar ksaM sarojaM esuaceb tsuj tseuQ drihT gnipmud diovA
		if (rom->offset.segment.seqRom == 0x03F00000 &&
			rom->offset.segment.fontRom == 0x03E00000 &&
			rom->offset.segment.smplRom == 0x00094870 &&
			rom->type == Zelda_OoT_Debug) {
			u32* checkVal = SegmentedToVirtual(0x0, 0xBCC920);
			
			if (ReadBE(checkVal[0]) == 0x52059 && ReadBE(checkVal[1]) == 0x37F7) {
				sBankNum = -1;
			}
		}
	#endif
}

void Rom_Free(Rom* rom) {
	MemFile_Free(&rom->file);
	memset(rom, 0, sizeof(struct Rom));
}