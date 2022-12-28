/*
 * File: z_en_zod.c
 * Overlay: ovl_En_Zod
 * Description: Zora Drummer Tijo
 */

#include "z_en_zod.h"

#define FLAGS (ACTOR_FLAG_1 | ACTOR_FLAG_8)

#define THIS ((EnZod*)thisx)

void EnZod_Init(Actor* thisx, PlayState* play);
void EnZod_Destroy(Actor* thisx, PlayState* play);
void EnZod_Update(Actor* thisx, PlayState* play);
void EnZod_Draw(Actor* thisx, PlayState* play);

void EnZod_ChangeAnim(EnZod* this, s16 nextAnimIndex, u8 mode);
void EnZod_PlayDrumsSequence(EnZod* this, PlayState* play);
void func_80BAFB84(EnZod* this, PlayState* play);
void func_80BAFDB4(EnZod* this, PlayState* play);
void func_80BAFF14(EnZod* this, PlayState* play);

#define TIJO_STATE_1 (1 << 0)
#define TIJO_STATE_2 (1 << 1)

typedef enum {
    /* 0 */ ENZOD_ANIM_PLAYING_VIVACE,
    /* 1 */ ENZOD_ANIM_READY_TO_PLAY,
    /* 2 */ ENZOD_ANIM_ARMS_FOLDED,
    /* 3 */ ENZOD_ANIM_PLAYING_LENTO,
    /* 4 */ ENZOD_ANIM_PLAYING_ANDANTINO,
    /* 5 */ ENZOD_ANIM_MAX,
} EnZodAnimation;

typedef enum {
    /* 1 */ ENZOD_INSTRUMENT_CYMBAL_1 = 1,
    /* 2 */ ENZOD_INSTRUMENT_CYMBAL_2,
    /* 3 */ ENZOD_INSTRUMENT_CYMBAL_3,
    /* 4 */ ENZOD_INSTRUMENT_DRUM_1,
    /* 5 */ ENZOD_INSTRUMENT_DRUM_2,
    /* 6 */ ENZOD_INSTRUMENT_DRUM_3,
    /* 7 */ ENZOD_INSTRUMENT_DRUM_4,
    /* 8 */ ENZOD_INSTRUMENT_DRUM_5,
    /* 9 */ ENZOD_INSTRUMENT_BASS_DRUM,
} EnZodInstrument;

const ActorInit En_Zod_InitVars = {
    ACTOR_EN_ZOD,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ZOD,
    sizeof(EnZod),
    (ActorFunc)EnZod_Init,
    (ActorFunc)EnZod_Destroy,
    (ActorFunc)EnZod_Update,
    (ActorFunc)EnZod_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 60, 40, 0, { 0, 0, 0 } },
};

static AnimationHeader* sAnimations[] = {
    &gTijoPlayingVivaceAnim, &gTijoReadyToPlayAnim,      &gTijoArmsFoldedAnim,
    &gTijoPlayingLentoAnim,  &gTijoPlayingAndantinoAnim,
};

static Vec3f D_80BB0580 = { 1300.0f, 1100.0f, 0.0f };

void EnZod_Init(Actor* thisx, PlayState* play) {
    s32 i;
    EnZod* this = THIS;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 60.0f);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gTijoSkel, &gTijoPlayingLentoAnim, this->morphTable, this->jointTable,
                       TIJO_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &gTijoPlayingLentoAnim);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    this->cymbalRots[0] = this->cymbalRots[1] = this->cymbalRots[2] = 0;
    this->actor.gravity = this->actor.terminalVelocity = -4.0f;
    this->cymbalRotVels[0] = this->cymbalRotVels[1] = this->cymbalRotVels[2] = 300;
    this->stateFlags = 0;
    this->nextAnimIndex = -1;
    this->curAnimIndex = -1;
    this->actor.textId = 0;
    this->unk_298 = 0;

    for (i = 0; i < ARRAY_COUNT(this->drumScales); i++) {
        this->drumScales[i] = 0.0f;
        this->drumScaleVels[i] = 0.01;
    }

    EnZod_ChangeAnim(this, ENZOD_ANIM_PLAYING_LENTO, ANIMMODE_ONCE);
    this->actionFunc = EnZod_PlayDrumsSequence;

    switch (ENZOD_GET_TYPE(thisx)) {
        case ENZOD_TYPE_1:
            if (gSaveContext.save.weekEventReg[78] & 1) {
                this->actionFunc = func_80BAFDB4;
                EnZod_ChangeAnim(this, ENZOD_ANIM_PLAYING_VIVACE, ANIMMODE_ONCE);
                this->actor.flags |= ACTOR_FLAG_10;
                ActorCutscene_SetIntentToPlay(this->actor.cutscene);
                break;
            }

            this->actionFunc = func_80BAFB84;
            if (!(gSaveContext.save.weekEventReg[55] & 0x80)) {
                Actor_Kill(&this->actor);
                break;
            }
            break;

        case ENZOD_TYPE_2:
            this->actionFunc = func_80BAFF14;
            this->fogNear = -1;
            this->stateFlags |= TIJO_STATE_2;
            break;

        default:
            if (gSaveContext.save.weekEventReg[55] & 0x80) {
                Actor_Kill(&this->actor);
            }
            this->actor.flags |= ACTOR_FLAG_10;
            break;
    }
}

void EnZod_Destroy(Actor* thisx, PlayState* play) {
    EnZod* this = THIS;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnZod_HandleRoomConversation(EnZod* this, PlayState* play) {
    u16 textId;

    if (gSaveContext.save.playerForm != PLAYER_FORM_ZORA) {
        textId = 0x1227;
        if (gSaveContext.save.weekEventReg[32] & 8) {
            textId = 0x1229;
        } else {
            gSaveContext.save.weekEventReg[32] |= 8;
        }
    } else if (this->stateFlags & TIJO_STATE_1) {
        textId = 0x1225;
    } else {
        textId = 0x1219;
        if (gSaveContext.save.weekEventReg[32] & 0x10) {
            textId = 0x1226;
        } else {
            gSaveContext.save.weekEventReg[32] |= 0x10;
        }
        this->stateFlags |= TIJO_STATE_1;
    }

    EnZod_ChangeAnim(this, ENZOD_ANIM_PLAYING_VIVACE, ANIMMODE_ONCE);
    Message_StartTextbox(play, textId, &this->actor);
}

s32 EnZod_PlayerIsFacingTijo(EnZod* this, PlayState* play) {
    if ((this->actor.playerHeightRel < 30.0f) && (this->actor.xzDistToPlayer < 200.0f) &&
        Player_IsFacingActor(&this->actor, 0x3000, play) && Actor_IsFacingPlayer(&this->actor, 0x3000)) {
        return true;
    } else {
        return false;
    }
}

void EnZod_ChangeAnim(EnZod* this, s16 nextAnimIndex, u8 mode) {
    if ((nextAnimIndex < ENZOD_ANIM_PLAYING_VIVACE) || (nextAnimIndex >= ENZOD_ANIM_MAX)) {
        nextAnimIndex = ENZOD_ANIM_PLAYING_LENTO;
    }
    Animation_Change(&this->skelAnime, sAnimations[nextAnimIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimations[nextAnimIndex]), mode, -5.0f);
    this->curAnimIndex = nextAnimIndex;
    this->nextAnimIndex = nextAnimIndex;
}

void EnZod_UpdateAnimation(EnZod* this) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->nextAnimIndex == this->curAnimIndex) {
            EnZod_ChangeAnim(this, this->curAnimIndex, ANIMMODE_ONCE);
            switch (this->curAnimIndex) {
                case ENZOD_ANIM_PLAYING_LENTO:
                    if (Rand_ZeroFloat(1.0f) < 0.2f) {
                        this->nextAnimIndex = ENZOD_ANIM_PLAYING_ANDANTINO;
                    }
                    break;

                case ENZOD_ANIM_PLAYING_ANDANTINO:
                    if (Rand_ZeroFloat(1.0f) < 0.8f) {
                        this->nextAnimIndex = ENZOD_ANIM_PLAYING_LENTO;
                    }
                    break;
            }

        } else {
            EnZod_ChangeAnim(this, this->nextAnimIndex, ANIMMODE_ONCE);
        }
        SkelAnime_Update(&this->skelAnime);
    }
}

void EnZod_UpdateInstruments(EnZod* this) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->cymbalRots); i++) {
        this->cymbalRots[i] += this->cymbalRotVels[i];
        this->cymbalRotVels[i] -= (s16)(this->cymbalRots[i] * 0.1f);

        if (ABS_ALT(this->cymbalRotVels[i]) > 100) {
            this->cymbalRotVels[i] *= 0.9f;
        }

        switch (i) {
            case 0:
                if ((this->curAnimIndex == ENZOD_ANIM_PLAYING_ANDANTINO) && ((s32)this->skelAnime.curFrame == 7)) {
                    this->cymbalRotVels[i] = -1000;
                }
                break;

            case 1:
                if ((this->curAnimIndex == ENZOD_ANIM_PLAYING_ANDANTINO) && ((s32)this->skelAnime.curFrame == 19)) {
                    this->cymbalRotVels[i] = -1000;
                }
                break;

            default:
                break;
        }
    }

    for (i = 0; i < ARRAY_COUNT(this->drumScales); i++) {
        this->drumScales[i] += this->drumScaleVels[i];
        this->drumScaleVels[i] -= (this->drumScales[i] * 0.8f);

        if (fabsf(this->drumScaleVels[i]) > 0.01f) {
            this->drumScaleVels[i] *= 0.5f;
        }

        switch (i) {
            case 0:
                if (((this->curAnimIndex == ENZOD_ANIM_PLAYING_LENTO) ||
                     (this->curAnimIndex == ENZOD_ANIM_PLAYING_ANDANTINO)) &&
                    ((s32)this->skelAnime.curFrame == 1)) {
                    this->drumScaleVels[i] = 0.1f;
                }
                break;

            case 2:
                if (((this->curAnimIndex == ENZOD_ANIM_PLAYING_LENTO) && ((s32)this->skelAnime.curFrame == 19)) ||
                    ((this->curAnimIndex == ENZOD_ANIM_PLAYING_ANDANTINO) && ((s32)this->skelAnime.curFrame == 8))) {
                    this->drumScaleVels[i] = 0.1f;
                }
                break;

            case 3:
                switch (this->curAnimIndex) {
                    case 3:
                        switch ((s32)this->skelAnime.curFrame) {
                            case 1:
                            case 7:
                            case 12:
                            case 19:
                                this->drumScaleVels[i] = 0.03f;
                                break;
                        }
                        break;

                    case 4:
                        if ((s32)this->skelAnime.curFrame == 1) {
                            this->drumScaleVels[i] = 0.1f;
                        }
                        break;
                }
                break;

            case 4:
                if ((this->curAnimIndex == ENZOD_ANIM_PLAYING_ANDANTINO) && ((s32)this->skelAnime.curFrame == 19)) {
                    this->drumScaleVels[i] = 0.15f;
                }

                break;

            default:
                break;
        }
    }
}

void func_80BAF7CC(EnZod* this, PlayState* play) {
    EnZod_UpdateAnimation(this);
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play) && (play->msgCtx.currentTextId == 0x121F)) {
                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        func_8019F208();
                        func_80151938(play, 0x1220);
                        break;

                    case 1:
                        func_8019F230();
                        func_80151938(play, 0x1223);
                        break;
                }
            }
            break;

        case TEXT_STATE_5:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x121A:
                    case 0x121B:
                    case 0x121C:
                    case 0x1220:
                    case 0x1221:
                    case 0x1227:
                        func_80151938(play, play->msgCtx.currentTextId + 1);
                        break;

                    case 0x1219:
                        func_80151938(play, play->msgCtx.currentTextId + 1);
                        this->nextAnimIndex = ENZOD_ANIM_ARMS_FOLDED;
                        break;

                    case 0x121D:
                        func_80151938(play, play->msgCtx.currentTextId + 1);
                        this->nextAnimIndex = ENZOD_ANIM_READY_TO_PLAY;
                        break;

                    case 0x1223:
                        func_80151938(play, play->msgCtx.currentTextId + 1);
                        this->nextAnimIndex = ENZOD_ANIM_PLAYING_VIVACE;
                        break;

                    case 0x121E:
                    case 0x1226:
                        func_80151938(play, 0x121F);
                        this->nextAnimIndex = ENZOD_ANIM_READY_TO_PLAY;
                        break;

                    default:
                        func_801477B4(play);
                        this->actionFunc = EnZod_PlayDrumsSequence;
                        EnZod_ChangeAnim(this, ENZOD_ANIM_PLAYING_LENTO, ANIMMODE_ONCE);
                        break;
                }
            }
    }
}

void EnZod_PlayDrumsSequence(EnZod* this, PlayState* play) {
    s32 pad;
    Vec3f seqPos;

    EnZod_UpdateAnimation(this);

    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        EnZod_HandleRoomConversation(this, play);
        this->actionFunc = func_80BAF7CC;
    } else if (EnZod_PlayerIsFacingTijo(this, play)) {
        func_800B8614(&this->actor, play, 210.0f);
    }

    seqPos.x = this->actor.projectedPos.x;
    seqPos.y = this->actor.projectedPos.y;
    seqPos.z = this->actor.projectedPos.z;

    func_801A1FB4(3, &seqPos, NA_BGM_DRUMS_PLAY, 700.0f);
}

void func_80BAFA44(EnZod* this, PlayState* play) {
    u16 textId;

    if (gSaveContext.save.playerForm == PLAYER_FORM_ZORA) {
        if (gSaveContext.save.weekEventReg[79] & 1) {
            textId = 0x1253;
        } else {
            textId = 0x1251;
            if (gSaveContext.save.weekEventReg[78] & 0x20) {
                textId = 0x1252;
            } else {
                gSaveContext.save.weekEventReg[78] |= 0x20;
            }
        }
    } else {
        textId = 0x1250;
    }

    EnZod_ChangeAnim(this, ENZOD_ANIM_PLAYING_VIVACE, ANIMMODE_ONCE);
    Message_StartTextbox(play, textId, &this->actor);
}

void func_80BAFADC(EnZod* this, PlayState* play) {
    u8 talkState;

    EnZod_UpdateAnimation(this);
    talkState = Message_GetState(&play->msgCtx);
    if (talkState != TEXT_STATE_CLOSING) {
        if ((talkState == TEXT_STATE_5) && Message_ShouldAdvance(play)) {
            func_801477B4(play);
            this->actionFunc = func_80BAFB84;
            EnZod_ChangeAnim(this, ENZOD_ANIM_PLAYING_LENTO, ANIMMODE_ONCE);
        }
    } else {
        this->actionFunc = func_80BAFB84;
        EnZod_ChangeAnim(this, ENZOD_ANIM_PLAYING_LENTO, ANIMMODE_ONCE);
    }
}

void func_80BAFB84(EnZod* this, PlayState* play) {
    EnZod_UpdateAnimation(this);

    if (Actor_ProcessTalkRequest(&this->actor, &play->state)) {
        func_80BAFA44(this, play);
        this->actionFunc = func_80BAFADC;
    } else if (EnZod_PlayerIsFacingTijo(this, play)) {
        func_800B8614(&this->actor, play, 210.0f);
    }
}

void EnZod_DoNothing(EnZod* this, PlayState* play) {
}

void EnZod_Rehearse(EnZod* this, PlayState* play) {
    EnZod_UpdateAnimation(this);
    if (ActorCutscene_GetCanPlayNext(this->actor.cutscene)) {
        ActorCutscene_Start(this->actor.cutscene, &this->actor);
        this->actor.cutscene = ActorCutscene_GetAdditionalCutscene(this->actor.cutscene);
        if (this->actor.cutscene == -1) {
            this->actionFunc = EnZod_DoNothing;
            play->nextEntrance = play->setupExitList[ENZOD_GET_ENTRANCE_INDEX(&this->actor)];
            play->transitionType = TRANS_TYPE_05;
            play->transitionTrigger = TRANS_TRIGGER_START;
            gSaveContext.save.weekEventReg[78] &= (u8)~1;
        } else {
            ActorCutscene_SetIntentToPlay(this->actor.cutscene);
        }
    } else {
        ActorCutscene_SetIntentToPlay(this->actor.cutscene);
    }
}

void EnZod_SetupRehearse(EnZod* this, PlayState* play) {
    EnZod_UpdateAnimation(this);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_5) && Message_ShouldAdvance(play)) {
        func_801477B4(play);
        EnZod_ChangeAnim(this, ENZOD_ANIM_PLAYING_LENTO, ANIMMODE_ONCE);
        this->actionFunc = EnZod_Rehearse;
        ActorCutscene_Stop(this->actor.cutscene);
        this->actor.cutscene = ActorCutscene_GetAdditionalCutscene(this->actor.cutscene);
        ActorCutscene_SetIntentToPlay(this->actor.cutscene);
        gSaveContext.save.weekEventReg[79] |= 1;
        Audio_QueueSeqCmd(NA_BGM_INDIGO_GO_SESSION | 0x8000);
    }
}

void func_80BAFDB4(EnZod* this, PlayState* play) {
    EnZod_UpdateAnimation(this);
    if (ActorCutscene_GetCanPlayNext(this->actor.cutscene)) {
        ActorCutscene_Start(this->actor.cutscene, &this->actor);
        func_800B7298(play, NULL, 0x44);
        Message_StartTextbox(play, 0x103A, &this->actor);
        this->actionFunc = EnZod_SetupRehearse;
    } else {
        ActorCutscene_SetIntentToPlay(this->actor.cutscene);
    }
}

void func_80BAFE34(EnZod* this, PlayState* play) {
    EnZod_UpdateAnimation(this);
    if (this->fogNear < 799) {
        this->fogNear += 200;
    } else {
        this->fogNear += 30;
    }

    if (this->fogNear > 999) {
        this->fogNear = 999;
    }

    if (Cutscene_CheckActorAction(play, 0x203)) {
        if (play->csCtx.actorActions[Cutscene_GetActorActionIndex(play, 0x203)]->action == 1) {
            this->actionFunc = func_80BAFF14;
            this->fogNear = -1;
        }
    } else {
        this->actionFunc = func_80BAFF14;
        this->fogNear = -1;
    }
}

void func_80BAFF14(EnZod* this, PlayState* play) {
    EnZod_UpdateAnimation(this);
    if (Cutscene_CheckActorAction(play, 0x203) &&
        (play->csCtx.actorActions[Cutscene_GetActorActionIndex(play, 0x203)]->action == 4)) {
        this->actionFunc = func_80BAFE34;
    }
}

void EnZod_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZod* this = THIS;
    Vec3s torsoRot;

    Actor_MoveWithGravity(&this->actor);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 10.0f, 4);
    this->actionFunc(this, play);
    EnZod_UpdateInstruments(this);

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }

    this->eyeIndex = this->blinkTimer;

    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }

    torsoRot.z = 0;
    torsoRot.y = 0;
    torsoRot.x = 0;

    if (EnZod_PlayerIsFacingTijo(this, play)) {
        Actor_TrackPlayer(play, &this->actor, &this->headRot, &torsoRot, this->actor.focus.pos);
        if (this->headRot.x > 0xBB8) {
            this->headRot.x = 0xBB8;
        } else if (this->headRot.x < -0xBB8) {
            this->headRot.x = -0xBB8;
        }
    } else {
        Math_SmoothStepToS(&this->headRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->headRot.y, 0, 6, 0x1838, 0x64);
    }
}

s32 EnZod_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnZod* this = THIS;

    if (limbIndex == TIJO_LIMB_HEAD) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;
    }

    return false;
}

void EnZod_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    if (limbIndex == TIJO_LIMB_HEAD) {
        Matrix_MultVec3f(&D_80BB0580, &thisx->focus.pos);
    }
}

void EnZod_DrawDrums(EnZod* this, PlayState* play) {
    static Gfx* sTijoDrumsDLs[] = {
        gTijoDrumFrameDL, gTijoRideCymbalDL, gTijoCrashCymbalDL, gTijoHiHatDL, gTijoDrum1DL,
        gTijoDrum2DL,     gTijoDrum3DL,      gTijoDrum4DL,       gTijoDrum5DL, gTijoBassDrumDL,
    };
    s32 i;
    f32 instrumentPosXs[] = { 0.0f, -2690.0f, 2310.0f, 3888.0f, -4160.0f, -2200.0f, -463.0f, 1397.0f, 3413.0f, 389.0f };
    f32 instrumentPosYs[] = { 0.0f, 6335.0f, 6703.0f, 5735.0f, 3098.0f, 3349.0f, 3748.0f, 3718.0f, 2980.0f, 1530.0f };
    f32 instrumentPosZs[] = { 0.0f, 4350.0f, 3200.0f, 1555.0f, 2874.0f, 3901.0f, 4722.0f, 4344.0f, 3200.0f, 3373.0f };
    f32 scale;

    OPEN_DISPS(play->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(sTijoDrumsDLs); i++) {
        Matrix_Push();
        Matrix_Translate(instrumentPosXs[i], instrumentPosYs[i], instrumentPosZs[i], MTXMODE_APPLY);

        switch (i) {
            case ENZOD_INSTRUMENT_CYMBAL_1:
            case ENZOD_INSTRUMENT_CYMBAL_2:
            case ENZOD_INSTRUMENT_CYMBAL_3:
                Matrix_RotateXS(this->cymbalRots[i - 1], MTXMODE_APPLY);
                break;

            case ENZOD_INSTRUMENT_DRUM_1:
            case ENZOD_INSTRUMENT_DRUM_2:
            case ENZOD_INSTRUMENT_DRUM_3:
            case ENZOD_INSTRUMENT_DRUM_4:
            case ENZOD_INSTRUMENT_DRUM_5:
            case ENZOD_INSTRUMENT_BASS_DRUM:
                scale = this->drumScales[9 - i] + 1.0f;
                Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
                break;

            default:
                break;
        }

        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, sTijoDrumsDLs[i]);
        Matrix_Pop();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnZod_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sTijoEyesTextures[] = { &gTijoEyesOpen, &gTijoEyesHalfOpen, &gTijoEyesClosed };
    EnZod* this = THIS;
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    func_8012C28C(play->state.gfxCtx);

    if (this->stateFlags & TIJO_STATE_2) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 0, 0, 0, 0, this->fogNear, 1000);
    }

    gfx = POLY_OPA_DISP;

    gSPSegment(&gfx[0], 0x08, Lib_SegmentedToVirtual(sTijoEyesTextures[this->eyeIndex]));
    gSPSegment(&gfx[1], 0x09, Lib_SegmentedToVirtual(&gTijoMouthClosedTex));

    POLY_OPA_DISP = &gfx[2];

    EnZod_DrawDrums(this, play);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnZod_OverrideLimbDraw, EnZod_PostLimbDraw, &this->actor);
    if (this->stateFlags & TIJO_STATE_2) {
        POLY_OPA_DISP = func_801660B8(play, POLY_OPA_DISP);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
