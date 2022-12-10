#ifndef Z_EN_TAKARAYA_H
#define Z_EN_TAKARAYA_H

#include "global.h"
#include "z64animation.h"

struct EnTakaraya;

typedef void (*EnTakarayaActionFunc)(struct EnTakaraya*, PlayState*);

typedef struct EnTakaraya {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable; // size 0x06
    /* 0x18E */ char unk_18E[0x8E];
    /* 0x218 */ Vec3s morphTable;
    /* 0x21E */ char unk_21E[0x8A];
    /* 0x2A8 */ EnTakarayaActionFunc actionFunc;
    /* 0x2AC */ u8 unk2AC;
    /* 0x2AD */ u8 unk2AD;
    /* 0x2AE */ s16 unk2AE_timerCounter;
    /* 0x2B0 */ u16 unk2B0;
    /* 0x2B2 */ u16 params;
    /* 0x2B4 */ Vec3s headRot;
} EnTakaraya; // size = 0x2BC

#endif // Z_EN_TAKARAYA_H
