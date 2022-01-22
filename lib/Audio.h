#include "ExtLib.h"

typedef struct AdpcmLoop {
	u32 start;
	u32 end;
	u32 count;
	u32 origSpls;
	s16 state[]; // elements: count != 0 ? 16 : 0.
} AttAligned(16) AdpcmLoop; // size = 0x10 or 0x30

typedef struct AdpcmBook {
	s32 order;
	s32 npredictors;
	s16 book[]; // elements: 8 * order * npredictors
} AttAligned(16) AdpcmBook;

typedef struct Sample {
	union {
		struct {
			u32 codec     : 4;
			u32 medium    : 2;
			u32 unk_bit26 : 1;
			u32 unk_bit25 : 1;
			u32 size      : 24;
		} info;
		u32 data;
	};
	void32 sampleAddr; // u8*
	void32 loop; // AdpcmLoop*
	void32 book; // AdpcmBook*
} AttAligned(16) Sample; // size = 0x10

typedef struct Sound {
	void32 sample; // Sample*
	union {
		f32 tuning;
		u32 swap32;
	};
} Sound; // size = 0x8

typedef struct Adsr {
	s16 rate;
	s16 level;
} Adsr; // size = 0x4

typedef struct Instrument {
	u8     loaded;
	u8     splitLo;
	u8     splitHi;
	u8     release;
	void32 envelope; // AdsrEnvelopePoint*
	Sound  lo;
	Sound  prim;
	Sound  hi;
} Instrument; // size >= 0x20

typedef struct Drum {
	u8     release;
	u8     pan;
	u8     loaded;
	Sound  sound;
	void32 envelope;
} Drum; // size >= 0x14

typedef struct SoundFont {
	void32 drums;
	void32 sfx;
	void32 instruments[];
} SoundFont;