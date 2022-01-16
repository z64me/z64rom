#include "z64rom.h"

N64AudioInfo gSampleInfo[] = {
	/* 000 */ 16000, 0, NULL, "Bird_Chirp1",
	/* 001 */ 22050, 0, NULL, "Bird_Chirp2",
	/* 002 */ 22050, 0, NULL, "Bird_Chirp3",
	/* 003 */ 22050, 0, NULL, "Bird_Chirp4",
	/* 004 */ 22050, 0, NULL, "Sample_4",
	/* 005 */ 22050, 0, NULL, "Bird_Crow1",
	/* 006 */ 22050, 0, NULL, "Sample_6",
	/* 007 */ 22050, 0, NULL, "Sample_7",
	/* 008 */ 22050, 0, NULL, "Sample_8",
	/* 009 */ 22050, 0, NULL, "Bird_Owl",
	/* 010 */ 22050, 0, NULL, "Sample_10",
	/* 011 */ 22050, 0, NULL, "Sample_11",
	/* 012 */ 16000, 0, NULL, "Amb_RainHeavy",
	/* 013 */ 16000, 0, NULL, "Amb_WindStorm",
	/* 014 */ 16000, 0, NULL, "Amb_Rain",
	/* 015 */ 16000, 0, NULL, "Amb_DistantStorm",
	/* 016 */ 16000, 0, NULL, "Generic_Thunder",
	/* 017 */ 16000, 0, NULL, "Amb_Wind",
	/* 018 */ 16000, 0, NULL, "Bird_Rooster",
	/* 019 */ 16000, 0, NULL, "Bird_Crow2",
	/* 020 */ 16000, 0, NULL, "Step_Ground",
	/* 021 */ 16000, 0, NULL, "Step_Sand",
	/* 022 */ 16000, 0, NULL, "Step_Concrete",
	/* 023 */ 16000, 0, NULL, "Step_Wet",
	/* 024 */ 16000, 0, NULL, "Step_Water",
	/* 025 */ 16000, 0, NULL, "Step_Grass",
	/* 026 */ 16000, 0, NULL, "Sample_26", // Thud
	/* 027 */ 16000, 0, NULL, "Sample_27", // Greenery_Rattle
	/* 028 */ 12000, 0, NULL, "Sample_28",
	/* 029 */ 16000, 0, NULL, "Sample_29",
	/* 030 */ 16000, 0, NULL, "Sample_30",
	/* 031 */ 12000, 0, NULL, "Step_Wood",
	/* 032 */ 12000, 0, NULL, "Water_Noise1",
	/* 033 */ 12000, 0, NULL, "Water_Noise2",
	/* 034 */ 12000, 0, NULL, "Water_Noise3",
	/* 035 */ 12000, 0, NULL, "Water_Noise4",
	/* 036 */ 12000, 0, NULL, "Water_Noise5",
	/* 037 */ 12000, 0, NULL, "Water_Noise6",
	/* 038 */ 16000, 0, NULL, "Water_Splash1",
	/* 039 */ 16000, 0, NULL, "Water_Swim",
	/* 040 */ 13000, 0, NULL, "Sample_40", // MM Fireworks
	/* 041 */ 13000, 0, NULL, "Step_WoodBridge",
	/* 042 */ 16000, 0, NULL, "Sample_42",
	/* 043 */ 16000, 0, NULL, "Metal_ClongDrop",
	/* 044 */ 20000, 0, NULL, "Step_Ice", // Stone_Something
	/* 045 */ 22050, 0, NULL, "Stone_Scrape",
	/* 046 */ 16000, 0, NULL, "Metal_Cling1",
	/* 047 */ 16000, 0, NULL, "Generic_Swipe1",
	/* 048 */ 22050, 0, NULL, "Metal_Rattle1",
	/* 049 */ 22050, 0, NULL, "Metal_Draw",
	/* 050 */ 22050, 0, NULL, "Generic_Swipe2",
	/* 051 */ 22050, 0, NULL, "Generic_Swoosh1",
	/* 052 */ 22050, 0, NULL, "Wood_CreakBend",
	/* 053 */ 22050, 0, NULL, "Metal_HitHard",
	/* 054 */ 22050, 0, NULL, "Metal_Rattle2",
	/* 055 */ 22050, 0, NULL, "Generic_Hiss",
	/* 056 */ 22050, 0, NULL, "Sample_56",
	/* 057 */ 22050, 0, NULL, "Sample_57",
	/* 058 */ 22050, 0, NULL, "Arrow_Land",
	/* 059 */ 16000, 0, NULL, "Generic_Swoosh2",
	/* 060 */ 16000, 0, NULL, "Sample_60",
	/* 061 */ 16000, 0, NULL, "Generic_Whip",
	/* 062 */ 16000, 0, NULL, "Sample_62_Lazer",
	/* 063 */ 22050, 0, NULL, "Sample_63",
	/* 064 */ 16000, 0, NULL, "Metal_Hit",
	/* 065 */ 22050, 0, NULL, "String_LensOfTruth",
	/* 066 */ 8000, 0, NULL, "String_Pluck",
	/* 067 */ 22000, 0, NULL, "String_Wacky",
	/* 068 */ 8000, 0, NULL, "Sample_68",
	/* 069 */ 16000, 0, NULL, "Sample_69",
	/* 070 */ 16000, 0, NULL, "Door_Open",
	/* 071 */ 16000, 0, NULL, "Door_Close",
	/* 072 */ 16000, 0, NULL, "Generic_Explosion",
	/* 073 */ 16000, 0, NULL, "Horse_Gallop",
	/* 074 */ 16000, 0, NULL, "Horse_Neigh",
	/* 075 */ 12000, 0, NULL, "Water_Stream",
	/* 076 */ 12000, 0, NULL, "Water_Waterfall",
	/* 077 */ 12000, 0, NULL, "Stone_Move",
	/* 078 */ 16000, 0, NULL, "Amb_Lava",
	/* 079 */ 12000, 0, NULL, "Metal_ChainWind",
	/* 080 */ 16000, 0, NULL, "Bird_Cucco_Idle",
	/* 081 */ 12000, 0, NULL, "Generic__Slam",
	/* 082 */ 12000, 0, NULL, "Horse_Snort",
	/* 083 */ 16000, 0, NULL, "Water_Splash2",
	/* 084 */ 12000, 0, NULL, "Wood_Knock",
	/* 085 */ 16000, 0, NULL, "Sample_85",
	/* 086 */ 16000, 0, NULL, "Generic_Beam",
	/* 087 */ 16000, 0, NULL, "Stone_Slam",
	/* 088 */ 16000, 0, NULL, "Wood_Creak1",
	/* 089 */ 16000, 0, NULL, "Wood_Creak2",
	/* 090 */ 16000, 0, NULL, "Fire_Ignite",
	/* 091 */ 16000, 0, NULL, "Warp_Shine",
	/* 092 */ 16000, 0, NULL, "Sys_WarpSong",
	/* 093 */ 12000, 0, NULL, "Amb_WaterDripping",
	/* 094 */ 12000, 0, NULL, "Amb_Underwater",
	/* 095 */ 12000, 0, NULL, "Fire_Rattle",
	/* 096 */ 12000, 0, NULL, "Metal_Whir",
	/* 097 */ 12000, 0, NULL, "Metal_Slam",
	/* 098 */ 16000, 0, NULL, "Cloth_Cape",
	/* 099 */ 22050, 0, NULL, "Amb_Gibberish",
	/* 100 */ 16000, 3, NULL, "Sample_100",
	/* 101 */ 16000, 0, NULL, "Sample_101",
	/* 102 */ 16000, 0, NULL, "Sample_102",
	/* 103 */ 16000, 0, NULL, "Sample_103",
	/* 104 */ 16000, 0, NULL, "Sample_104",
	/* 105 */ 12000, 0, NULL, "Wolf_Howl",
	/* 106 */ 12000, 0, NULL, "Frog_Ribbit",
	/* 107 */ 16000, 0, NULL, "Dog_Woof",
	/* 108 */ 16000, 0, NULL, "Door_Metal_Close",
	/* 109 */ 16000, 3, NULL, "Cow_Moo",
	/* 110 */ 32000, 0, NULL, "Inst_Harp_Prim",
	/* 111 */ 22050, 0, NULL, "Sys_WrongNote",
	/* 112 */ 22050, 0, NULL, "GuardWhistle",
	/* 113 */ 22050, 0, NULL, "BellFanfare",
	/* 114 */ 32000, 0, NULL, "Inst_Whistle",
	/* 115 */ 32000, 0, NULL, "Inst_Ocarina",
	/* 116 */ 32000, 0, NULL, "Inst_Accordion",
	/* 117 */ 32000, 0, NULL, "Inst_Harp_Hi",
	/* 118 */ 32000, 0, NULL, "Inst_Malon",
	/* 119 */ 32000, 0, NULL, "Inst_Flute",
	/* 120 */ 32000, 0, NULL, "Perc_TambourineKnock",
	/* 121 */ 32000, 0, NULL, "Perc_TambourineChink",
	/* 122 */ 16000, 0, NULL, "Sys_FocusDangeon",
	/* 123 */ 16000, 0, NULL, "Sys_LockOn",
	
	/* 124 */ 20000, 0, NULL, "Lick_Adult_Vo_Lash1",
	/* 125 */ 20000, 0, NULL, "Link_Adult_Vo_Lash2",
	
	/* 126 */ 20000, 0, NULL, "Link_Adult_Vo_Attack1",
	/* 127 */ 20000, 0, NULL, "Link_Adult_Vo_Attack2",
	/* 128 */ 20000, 0, NULL, "Link_Adult_Vo_Attack3",
	/* 129 */ 20000, 0, NULL, "Link_Adult_Vo_Attack4",
	/* 130 */ 20000, 0, NULL, "Link_Adult_Vo_Jump1",
	/* 131 */ 20000, 0, NULL, "Link_Adult_Vo_Jump2",
	/* 132 */ 20000, 0, NULL, "Link_Adult_Vo_JumpAttack1",
	/* 133 */ 20000, 0, NULL, "Link_Adult_Vo_JumpAttack2",
	/* 134 */ 20000, 0, NULL, "Link_Adult_Vo_Hang1",
	/* 135 */ 20000, 0, NULL, "Link_Adult_Vo_Hang2",
	/* 136 */ 20000, 0, NULL, "Link_Adult_Vo_ClimbUp1",
	/* 137 */ 20000, 0, NULL, "Link_Adult_Vo_ClimbUp2",
	/* 138 */ 20000, 0, NULL, "Link_Adult_Vo_Damage1",
	/* 139 */ 20000, 0, NULL, "Link_Adult_Vo_Damage2",
	/* 140 */ 20000, 0, NULL, "Link_Adult_Vo_Damage3",
	/* 141 */ 20000, 0, NULL, "Link_Adult_Vo_Knocked1",
	/* 142 */ 20000, 0, NULL, "Link_Adult_Vo_Knocked2",
	/* 143 */ 20000, 0, NULL, "Link_Adult_Vo_Knocked3",
	/* 144 */ 20000, 0, NULL, "Link_Adult_Vo_FallScream1",
	/* 145 */ 20000, 0, NULL, "Link_Adult_Vo_FallScream2",
	/* 146 */ 20000, 0, NULL, "Link_Adult_Vo_FallGasp1",
	/* 147 */ 20000, 0, NULL, "Link_Adult_Vo_FallGasp2",
	/* 148 */ 20000, 0, NULL, "Link_Adult_Vo_LowHealth1",
	/* 149 */ 20000, 0, NULL, "Link_Adult_Vo_LowHealth2",
	/* 150 */ 20000, 0, NULL, "Link_Adult_Vo_DrinkGulp",
	/* 151 */ 20000, 0, NULL, "Link_Adult_Vo_DrinkFinish",
	/* 152 */ 20000, 0, NULL, "Link_Adult_Vo_BreatheIn",
	/* 153 */ 20000, 0, NULL, "Link_Adult_Vo_DinsFire",
	/* 154 */ 20000, 0, NULL, "Link_Adult_Vo_FallDamage",
	/* 155 */ 20000, 0, NULL, "Link_Adult_Vo_Shocked",
	/* 156 */ 20000, 0, NULL, "Link_Adult_Vo_Die1",
	/* 157 */ 20000, 0, NULL, "Link_Adult_Vo_Die2",
	/* 158 */ 20000, 0, NULL, "Link_Adult_Vo_Die3",
	/* 159 */ 20000, 0, NULL, "Link_Adult_Vo_IdleCold1",
	/* 160 */ 20000, 0, NULL, "Link_Adult_Vo_IdleCold2",
	/* 161 */ 20000, 0, NULL, "Link_Adult_Vo_IdleCold3",
	/* 162 */ 20000, 0, NULL, "Link_Adult_Vo_IdleHot1",
	/* 163 */ 20000, 0, NULL, "Link_Adult_Vo_IdleHot2",
	/* 164 */ 20000, 0, NULL, "Link_Adult_Vo_IdleHot3",
	/* 165 */ 20000, 0, NULL, "Link_Adult_Vo_IdleYawn1",
	/* 166 */ 20000, 0, NULL, "Link_Adult_Vo_IdleYawn2",
	/* 167 */ 20000, 0, NULL, "Link_Adult_Vo_IdleYawn3",
	
	/* 168 */ 20000, 0, NULL, "Link_Child_Vo_Attack1",
	/* 169 */ 20000, 0, NULL, "Link_Child_Vo_Attack2",
	/* 170 */ 20000, 0, NULL, "Link_Child_Vo_Attack3",
	/* 171 */ 20000, 0, NULL, "Link_Child_Vo_Attack4",
	/* 172 */ 20000, 0, NULL, "Link_Child_Vo_Jump1",
	/* 173 */ 20000, 0, NULL, "Link_Child_Vo_Jump2",
	/* 174 */ 20000, 0, NULL, "Link_Child_Vo_JumpAttack1",
	/* 175 */ 20000, 0, NULL, "Link_Child_Vo_JumpAttack2",
	/* 176 */ 20000, 0, NULL, "Link_Child_Vo_Hang1",
	/* 177 */ 20000, 0, NULL, "Link_Child_Vo_Hang2",
	/* 178 */ 20000, 0, NULL, "Link_Child_Vo_ClimbUp1",
	/* 179 */ 20000, 0, NULL, "Link_Child_Vo_ClimbUp2",
	/* 180 */ 20000, 0, NULL, "Link_Child_Vo_Damage1",
	/* 181 */ 20000, 0, NULL, "Link_Child_Vo_Damage2",
	/* 182 */ 20000, 0, NULL, "Link_Child_Vo_Damage3",
	/* 183 */ 20000, 0, NULL, "Link_Child_Vo_Knocked1",
	/* 184 */ 20000, 0, NULL, "Link_Child_Vo_Knocked2",
	/* 185 */ 20000, 0, NULL, "Link_Child_Vo_Knocked3",
	/* 186 */ 20000, 0, NULL, "Link_Child_Vo_FallScream1",
	/* 187 */ 20000, 0, NULL, "Link_Child_Vo_FallScream2",
	/* 188 */ 20000, 0, NULL, "Link_Child_Vo_FallGasp1",
	/* 189 */ 20000, 0, NULL, "Link_Child_Vo_FallGasp2",
	/* 190 */ 20000, 0, NULL, "Link_Child_Vo_LowHealth1",
	/* 191 */ 20000, 0, NULL, "Link_Child_Vo_LowHealth2",
	/* 192 */ 20000, 0, NULL, "Link_Child_Vo_DrinkGulp",
	/* 193 */ 20000, 0, NULL, "Link_Child_Vo_DrinkFinish",
	/* 194 */ 20000, 0, NULL, "Link_Child_Vo_BreatheIn",
	/* 195 */ 20000, 0, NULL, "Link_Child_Vo_DinsFire",
	/* 196 */ 20000, 0, NULL, "Link_Child_Vo_FallDamage",
	/* 197 */ 20000, 0, NULL, "Link_Child_Vo_Shocked",
	/* 198 */ 20000, 0, NULL, "Link_Child_Vo_Die1",
	/* 199 */ 20000, 0, NULL, "Link_Child_Vo_Die2",
	/* 200 */ 20000, 0, NULL, "Link_Child_Vo_Die3",
	/* 201 */ 20000, 0, NULL, "Link_Child_Vo_IdleCold1",
	/* 202 */ 20000, 0, NULL, "Link_Child_Vo_IdleCold2",
	/* 203 */ 20000, 0, NULL, "Link_Child_Vo_IdleCold3",
	/* 204 */ 20000, 0, NULL, "Link_Child_Vo_IdleHot1",
	/* 205 */ 20000, 0, NULL, "Link_Child_Vo_IdleHot2",
	/* 206 */ 20000, 0, NULL, "Link_Child_Vo_IdleHot3",
	/* 207 */ 20000, 0, NULL, "Link_Child_Vo_IdleYawn1",
	/* 208 */ 20000, 0, NULL, "Link_Child_Vo_IdleYawn2",
	/* 209 */ 20000, 0, NULL, "Link_Child_Vo_IdleYawn3",
	
	/* 210 */ 16000, 0, NULL, "Navi_Vo_WatchOut",
	/* 211 */ 16000, 0, NULL, "Navi_Vo_Look",
	/* 212 */ 16000, 0, NULL, "Navi_Vo_Hey",
	/* 213 */ 16000, 0, NULL, "Navi_Vo_Listen",
	/* 214 */ 16000, 0, NULL, "Navi_Vo_Hello",
	/* 215 */ 16000, 0, NULL, "Talon_Vo_Snore",
	/* 216 */ 16000, 0, NULL, "Talon_Vo_WakeUp",
	/* 217 */ 16000, 0, NULL, "Talon_Vo_What",
	/* 218 */ 16000, 0, NULL, "Talon_Vo_Cry",
	/* 219 */ 16000, 0, NULL, "GreatFairy_Vo_Laugh",
	/* 220 */ 16000, 0, NULL, "GreatFairy_Vo_LaughSoft",
	/* 221 */ 16000, 0, NULL, "Ingo_Vo_Charge",
	/* 222 */ 16000, 0, NULL, "CarpenterBoss_Vo_Khaa",
	/* 223 */ 16000, 0, NULL, "Ingo_Vo_Lash",
	/* 224 */ 16000, 0, NULL, "Ingo_Vo_LashShort",
	/* 225 */ 16000, 0, NULL, "Sample_225",
	/* 226 */ 16000, 0, NULL, "Nabooru_Vo_Scream",
	/* 227 */ 16000, 0, NULL, "Nabooru_Vo_Hm",
	/* 228 */ 16000, 0, NULL, "Ruto_Vo_Kwee",
	/* 229 */ 16000, 0, NULL, "Ruto_Vo_Scream",
	/* 230 */ 16000, 0, NULL, "Ruto_Vo_Picked1",
	/* 231 */ 16000, 0, NULL, "Ruto_Vo_Laugh",
	/* 232 */ 16000, 0, NULL, "Ruto_Vo_Picked2",
	/* 233 */ 16000, 0, NULL, "Ruto_Vo_Thrown",
	/* 234 */ 16000, 0, NULL, "Ruto_Vo_RareIdle",
	/* 235 */ 16000, 0, NULL, "Sheik_Vo_Scream1",
	/* 236 */ 16000, 0, NULL, "Sheik_Vo_Scream2",
	/* 237 */ 16000, 0, NULL, "Sheik_Vo_Hurt",
	/* 238 */ 16000, 0, NULL, "Sheik_Vo_Nut",
	/* 239 */ 16000, 0, NULL, "Zelda_Child_Vo_UnkBreath",
	/* 240 */ 16000, 0, NULL, "Zelda_Child_Vo_Huh1",
	/* 241 */ 16000, 0, NULL, "Zelda_Child_Vo_Huh2",
	/* 242 */ 16000, 0, NULL, "Zelda_Child_Vo_Dissapointed",
	/* 243 */ 16000, 0, NULL, "Zelda_Child_Vo_Laugh",
	/* 244 */ 16000, 0, NULL, "Zelda_Child_Vo_Gasp",
	/* 245 */ 16000, 0, NULL, "Zelda_Child_Vo_Throw",
	/* 246 */ 16000, 0, NULL, "Zelda_Adult_Vo_Scream",
	/* 247 */ 16000, 0, NULL, "Zelda_Adult_Vo_Shocked",
	/* 248 */ 16000, 0, NULL, "Zelda_Adult_Vo_OpenGate",
	/* 249 */ 16000, 0, NULL, "Zelda_Adult_Vo_Surprised",
	/* 250 */ 16000, 0, NULL, "Zelda_Adult_Vo_Gasp",
	/* 251 */ 16000, 0, NULL, "CursedHuman_Vo_Hurt",
	/* 252 */ 16000, 0, NULL, "CursedHuman_Vo_Spin",
	/* 253 */ 16000, 0, NULL, "KingZora_Vo_Mweep",
	/* 254 */ 22050, 0, NULL, "Fire_Shoot",
	/* 255 */ 22050, 0, NULL, "Dodongo_Vo_Fire",
	/* 256 */ 16000, 3, NULL, "Fire_Rumble",
	/* 257 */ 16000, 3, NULL, "Stick_Clang_Moblin",
	/* 258 */ 16000, 3, NULL, "Ganondorf_LightningCharge",
	/* 259 */ 16000, 3, NULL, "Generic_Swoosh",
	/* 260 */ 16000, 0, NULL, "Generic_Shatter1",
	/* 261 */ 16000, 0, NULL, "Generic_Shatter2",
	/* 262 */ 16000, 3, NULL, "Dodongo_Step",
	/* 263 */ 16000, 3, NULL, "Dodongo_Breathe",
	/* 264 */ 16000, 3, NULL, "Dodongo_Swallow",
	/* 265 */ 16000, 3, NULL, "Gohma_CeilMove",
	/* 266 */ 16000, 3, NULL, "Generic_Crackling",
	/* 267 */ 16000, 3, NULL, "Sample_267",
	/* 268 */ 16000, 3, NULL, "Floormaster_Vanish",
	/* 269 */ 16000, 3, NULL, "Keese_HitRattling",
	/* 270 */ 16000, 3, NULL, "Generic_Flapping",
	/* 271 */ 16000, 3, NULL, "Guay_Scream",
	/* 272 */ 16000, 3, NULL, "Tektite_Move",
	/* 273 */ 16000, 3, NULL, "Unk_Vo_Oh",
	/* 274 */ 16000, 3, NULL, "Lizalfos_Vo_Attack",
	/* 275 */ 16000, 3, NULL, "Lizalfos_Step",
	/* 276 */ 16000, 3, NULL, "Unk_Vo_Laugh",
	/* 277 */ 16000, 3, NULL, "Unk_Vo_Whine2",
	/* 278 */ 16000, 3, NULL, "Generic_Shatter3",
	/* 279 */ 22050, 3, NULL, "Stalfos_Vo_Attack",
	/* 280 */ 22050, 3, NULL, "Stalfos_Vo_Die",
	/* 281 */ 16000, 3, NULL, "Stalfos_Vo_Laugh",
	/* 282 */ 16000, 3, NULL, "Stalfos_Swoosh",
	/* 283 */ 16000, 3, NULL, "DekuBaba_Clap",
	/* 284 */ 16000, 3, NULL, "DekuBaba_Attack",
	/* 285 */ 16000, 3, NULL, "Generic_Buzz",
	/* 286 */ 16000, 3, NULL, "Ganondorf_LightningAttack",
	/* 287 */ 16000, 3, NULL, "Ganondorf_Float",
	/* 288 */ 16000, 3, NULL, "Ganondorf_Charge",
	/* 289 */ 16000, 3, NULL, "Ganondorf_LightningExplosion",
	/* 290 */ 16000, 3, NULL, "Phantom_Vo_Die",
	/* 291 */ 22050, 3, NULL, "Phantom_Vo_Shout",
	/* 292 */ 22050, 3, NULL, "Unk_Vo_Die",
	/* 293 */ 22050, 3, NULL, "Unk_Vo_Howl",
	/* 294 */ 16000, 3, NULL, "Unk_Vo_Whine",
	/* 295 */ 16000, 3, NULL, "Deadhand_Vo_Bite",
	/* 296 */ 11025, 3, NULL, "Goron_Vo_Notice",
	/* 297 */ 14800, 3, NULL, "Goron_Vo_StandUpDown",
	/* 298 */ 16000, 3, NULL, "Goron_Vo_Happy",
	/* 299 */ 16000, 3, NULL, "KotakeKoume_Vo_Laugh_Hihihi",
	/* 300 */ 16000, 3, NULL, "KotakeKoume_Vo_Laugh_Huhuhu",
	/* 301 */ 16000, 3, NULL, "KotakeKoume_Vo_Cry",
	/* 302 */ 16000, 3, NULL, "KotakeKoume_Vo_Attack",
	/* 303 */ 16000, 3, NULL, "KotakeKoume_Vo_Questioning",
	/* 304 */ 16000, 3, NULL, "KotakeKoume_Vo_LastCry",
	/* 305 */ 16000, 3, NULL, "KotakeKoume_Vo_Argue",
	/* 306 */ 16000, 3, NULL, "Twinrova_Vo_Fall",
	/* 307 */ 16000, 3, NULL, "Twinrova_Vo_Hurt",
	/* 308 */ 16000, 3, NULL, "Twinrova_Vo_Unk",
	/* 309 */ 16000, 3, NULL, "Twinrova_Vo_Laugh",
	/* 310 */ 16000, 3, NULL, "Twinrova_Vo_Die",
	/* 311 */ 16000, 3, NULL, "Twinrova_Vo_Wink",
	/* 312 */ 16000, 3, NULL, "Unk_Vo_Whine1",
	/* 313 */ 16000, 3, NULL, "IronKnuckle_Snap",
	/* 314 */ 14800, 3, NULL, "IronKnuckle_Vo_Churg",
	/* 315 */ 14800, 3, NULL, "IronKnuckle_Vo_Hurt",
	/* 316 */ 14800, 3, NULL, "IronKnuckle_Vo_Charge",
	/* 317 */ 14800, 3, NULL, "IronKnuckle_Vo_Die",
	/* 318 */ 14800, 3, NULL, "IronKnuckle_Vo_RiseAxe",
	/* 319 */ 14800, 3, NULL, "IronKnuckle_Vo_RiseFromChair",
	/* 320 */ 16000, 3, NULL, "BongoBongo_Bongo",
	/* 321 */ 8000, 3, NULL, "BongoBongo_Vo_Sing",
	/* 322 */ 14800, 3, NULL, "Gerudo_Vo_Attack",
	/* 323 */ 16000, 3, NULL, "Gerudo_Vo_Breathe",
	/* 324 */ 14800, 3, NULL, "Gerudo_Vo_Hurt",
	/* 325 */ 13000, 3, NULL, "Gerudo_Vo_Die",
	/* 326 */ 18000, 0, NULL, "Monster_Damage",
	/* 327 */ 13000, 3, NULL, "Ganondorf_Vo_Laugh",
	/* 328 */ 14800, 3, NULL, "Ganondorf_Vo_Shout",
	/* 329 */ 14800, 3, NULL, "Ganondorf_Vo_OhYeah",
	/* 330 */ 14800, 3, NULL, "Ganondorf_Vo_Repel",
	/* 331 */ 14800, 3, NULL, "Ganondorf_Vo_WhamFloor",
	/* 332 */ 14800, 3, NULL, "Ganondorf_Vo_HurtLightArrow",
	/* 333 */ 14800, 3, NULL, "Ganondorf_Vo_HurtSword",
	/* 334 */ 14800, 3, NULL, "Ganondorf_Vo_Vulnerable",
	/* 335 */ 14800, 3, NULL, "Ganondorf_Vo_Recover",
	/* 336 */ 14800, 3, NULL, "Ganondorf_Vo_Die",
	/* 337 */ 14800, 3, NULL, "Ganondorf_Vo_LastBreaths",
	/* 338 */ 14800, 3, NULL, "Ganondorf_Vo_SpitBlood",
	/* 339 */ 14800, 3, NULL, "Ganondorf_Vo_LastScream",
	/* 340 */ 14800, 3, NULL, "Ganondorf_Vo_BigMagic1",
	/* 341 */ 14800, 3, NULL, "Ganondorf_Vo_BigMagic2",
	/* 342 */ 14800, 3, NULL, "Ganondorf_Vo_Laugh",
	/* 343 */ 16000, 0, NULL, "Redead_Vo_Scream",
	/* 344 */ 16000, 0, NULL, "Monster_Roar",
	/* 345 */ 14800, 0, NULL, "Skulltula_Creek",
	/* 346 */ 16000, 0, NULL, "Monster_Scream",
	/* 347 */ 16000, 0, NULL, "Phantom_Vo_Laugh",
	/* 348 */ 16000, 0, NULL, "Metal_Clang",
	/* 349 */ 22050, 0, NULL, "Metal_Cling2",
	
	/* 350 */ 32000, 0, NULL, "Inst_Dungeon_DarkDrone",
	/* 351 */ 32000, 0, NULL, "Inst_Dungeon_Shine",
	/* 352 */ 16000, 0, NULL, "Inst_Dungeon_Drip",
	/* 353 */ 16000, 0, NULL, "Inst_ChoirMale_Low",
	/* 354 */ 16000, 0, NULL, "Inst_ChoirMale_Prim",
	/* 355 */ 16000, 0, NULL, "Inst_Dungeon_Scream",
	/* 356 */ 16000, 0, NULL, "Inst_Piano_Low",
	/* 357 */ 32000, 0, NULL, "Inst_Piano_Prim",
	/* 358 */ 32000, 0, NULL, "Inst_Piano_Hi",
	/* 359 */ 23600, 0, NULL, "Inst_ChoirFemale_Prim",
	/* 360 */ 23600, 0, NULL, "Inst_ChoirFemale_Hi",
	/* 361 */ 32000, 0, NULL, "Inst_Harp",
	/* 362 */ 32000, 0, NULL, "Inst_Glockenspiel",
	/* 363 */ 14500, 0, NULL, "Inst_Dungeon_MetalDrone",
	/* 364 */ 32000, 0, NULL, "Inst_Trumpet_Hi",
	/* 365 */ 32000, 0, NULL, "Inst_Trumpet_Prim",
	/* 366 */ 32000, 0, NULL, "Inst_Trombone",
	/* 367 */ 32000, 0, NULL, "Perc_LowPerc",
	/* 368 */ 32000, 0, NULL, "Perc_Snare",
	/* 369 */ 32000, 0, NULL, "Perc_SnareSoft",
	/* 370 */ 32000, 0, NULL, "Perc_Cymbal",
	/* 371 */ 32000, 0, NULL, "Perc_Timpani",
	/* 372 */ 32000, 0, NULL, "Inst_String_Low",
	/* 373 */ 32000, 0, NULL, "Inst_String_Prim",
	/* 374 */ 32000, 0, NULL, "Inst_String_Hi",
	/* 375 */ 32000, 0, NULL, "Inst_Harmonica",
	/* 376 */ 16000, 0, NULL, "Inst_Guitar_Prim",
	/* 377 */ 16000, 0, NULL, "Inst_Guitar_Hi",
	/* 378 */ 32000, 0, NULL, "Perc_TambourineOpen",
	/* 379 */ 32000, 0, NULL, "Perc_TambourineKnock", // Dublicate
	/* 380 */ 32000, 0, NULL, "Perc_TambourineChink", // Dublicate
	/* 381 */ 32000, 0, NULL, "Inst_Clarinet",
	/* 382 */ 32000, 0, NULL, "Inst_Bassoon",
	/* 383 */ 32000, 0, NULL, "Inst_Pizzicato_Prim",
	/* 384 */ 32000, 0, NULL, "Inst_Pizzicato_Hi",
	/* 385 */ 32000, 0, NULL, "Inst_Organ_Prim",
	/* 386 */ 32000, 0, NULL, "Inst_Organ_Hi",
	/* 387 */ 32000, 0, NULL, "Inst_Horn",
	/* 388 */ 32000, 0, NULL, "Inst_Doublebass",
	/* 389 */ 32000, 0, NULL, "Perc_Clap",
	/* 390 */ 32000, 0, NULL, "Perc_Kilimba",
	/* 391 */ 32000, 0, NULL, "Inst_Wind1",
	/* 392 */ 32000, 0, NULL, "Perc_MetalBowl",
	/* 393 */ 32000, 0, NULL, "Perc_LuteTambourine",
	/* 394 */ 22050, 0, NULL, "Perc_LuteKickFlam",
	/* 395 */ 22050, 0, NULL, "Inst_45",
	/* 396 */ 32000, 0, NULL, "Inst_Harpsichord",
	/* 397 */ 32000, 0, NULL, "Inst_Marimba_Prim",
	/* 398 */ 32000, 0, NULL, "Inst_Marimba_Hi",
	/* 399 */ 32000, 0, NULL, "Inst_BellPad",
	/* 400 */ 32000, 0, NULL, "Inst_Bouzouki_Prim",
	/* 401 */ 32000, 0, NULL, "Inst_Bouzouki_Hi",
	/* 402 */ 32000, 0, NULL, "Perc_EthnicSoft",
	/* 403 */ 32000, 0, NULL, "Perc_EthnicHard",
	/* 404 */ 32000, 0, NULL, "Perc_EthnicMuted",
	/* 405 */ 32000, 0, NULL, "Perc_Gong",
	/* 406 */ 32000, 0, NULL, "Perc_WindChime",
	/* 407 */ 16000, 0, NULL, "Perc_BentDrum",
	/* 408 */ 32000, 0, NULL, "Inst_WoodChime",
	/* 409 */ 32000, 0, NULL, "Inst_VoicePad",
	/* 410 */ 32000, 0, NULL, "Inst_ChantFlute_Staccato",
	/* 411 */ 32000, 0, NULL, "Inst_ChantFlute_Flute",
	/* 412 */ 32000, 0, NULL, "Inst_ChantFlute_Vocal",
	/* 413 */ 32000, 0, NULL, "Perc_CongaOpen",
	/* 414 */ 32000, 0, NULL, "Perc_CongaSoft",
	/* 415 */ 32000, 0, NULL, "Perc_CongaMute",
	/* 416 */ 32000, 0, NULL, "Perc_Shaker",
	/* 417 */ 32000, 0, NULL, "Perc_CowBell",
	/* 418 */ 32000, 0, NULL, "Inst_JazzGuitar_Low",
	/* 419 */ 32000, 0, NULL, "Inst_JazzGuitar_Prim",
	/* 420 */ 16000, 0, NULL, "Inst_JazzGuitar_Hi",
	/* 421 */ 32000, 0, NULL, "Inst_Fiddle",
	/* 422 */ 22050, 0, NULL, "Perc_Bell",
	/* 423 */ 16000, 0, NULL, "Perc_CuicaOpen",
	/* 424 */ 16000, 0, NULL, "Perc_CuicaMute",
	/* 425 */ 32000, 0, NULL, "Inst_Banjo_Prim",
	/* 426 */ 32000, 0, NULL, "Inst_Banjo_Hi",
	/* 427 */ 16000, 0, NULL, "Inst_Chant",
	/* 428 */ 32000, 0, NULL, "Inst_SteelDrum_Prim",
	/* 429 */ 32000, 0, NULL, "Inst_SteelDrum_Hi",
	/* 430 */ 32000, 0, NULL, "Inst_FluteSynth",
	/* 431 */ 32000, 0, NULL, "Inst_Wind2",
	/* 432 */ 32000, 0, NULL, "Perc_CajonBass",
	/* 433 */ 32000, 0, NULL, "Perc_CajonSnare",
	/* 434 */ 32000, 0, NULL, "Inst_SynthPad",
	/* 435 */ 16000, 0, NULL, "Inst_Creek",
	/* 436 */ 0, 0, &gSampleInfo[407], "Perc_BentDrum", // Dublicate
	/* 437 */ 0, 0, &gSampleInfo[413], "Perc_CongaOpen", // Dublicate
	/* 438 */ 0, 0, &gSampleInfo[415], "Perc_CongaMute", // Dublicate
	/* 439 */ 0, 0, &gSampleInfo[423], "Perc_CuicaA", // Dublicate
	/* 440 */ 0, 0, &gSampleInfo[424], "Perc_CuicaB", // Dublicate
	/* 441 */ 0, 0, &gSampleInfo[390], "Perc_Kilimba", // Dublicate
	/* 442 */ 0, 0, &gSampleInfo[434], "Inst_SynthPad", // Dublicate
	/* 443 */ 32000, 0, NULL, "Inst_EthnicFlute",
	/* 444 */ 0, 0, &gSampleInfo[407], "Perc_CongaOpen", // Dublicate
	/* 445 */ 0, 0, &gSampleInfo[415], "Perc_CongaMute", // Dublicate
	/* 446 */ 0, 0, &gSampleInfo[405], "Perc_Gong", // Dublicate
	/* 447 */ 0, 0, &gSampleInfo[406], "Perc_WindChime", // Dublicate
	/* 448 */ 0, 0, &gSampleInfo[409], "Inst_VoicePad", // Dublicate
};