/*
 * File: z_en_takaraya.c
 * Overlay: ovl_En_Takaraya
 * Description: Treasure Chest Shop Gal
 */

#include "z_en_takaraya.h"

#define FLAGS (ACTOR_FLAG_1 | ACTOR_FLAG_8 | ACTOR_FLAG_10 | ACTOR_FLAG_20)

#define THIS ((EnTakaraya*)thisx)

void EnTakaraya_Init(Actor* thisx, PlayState* play);
void EnTakaraya_Destroy(Actor* thisx, PlayState* play);
void EnTakaraya_Update(Actor* thisx, PlayState* play);
void EnTakaraya_Draw(Actor* thisx, PlayState* play);


ActorInit En_Takaraya_InitVars = {
    ACTOR_EN_TAKARAYA,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_BG,
    sizeof(EnTakaraya),
    (ActorFunc)EnTakaraya_Init,
    (ActorFunc)EnTakaraya_Destroy,
    (ActorFunc)EnTakaraya_Update,
    (ActorFunc)EnTakaraya_Draw,
};

// static InitChainEntry sInitChain[] = {
static InitChainEntry D_80ADFB20[] = {
    ICHAIN_U8(targetMode, 6, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 1000, ICHAIN_STOP),
};


extern AnimationHeader D_06001384;
extern FlexSkeletonHeader D_06008FC8;
extern AnimationHeader D_06000968;
extern AnimationHeader D_06009890;
extern AnimationHeader D_0600A280;
extern AnimationHeader D_0600AD98;

extern u32 D_80ADFB00[4];
// static u32 D_80ADFB00[] = { 0x06006F58, 0x06007358, 0x06007758, 0x06007358 };
extern u32 D_80ADFB10[4];
// static u32 D_80ADFB10[] = { 0x06007B58, 0x06007F58, 0x06007758, 0x06007F58 };
extern u16 D_80ADFB2C[6];
// static u16 D_80ADFB2C[] = { 0x076D, 0x076E, 0x076F, 0x076C, 0x076D, 0x0000 };
extern u8 D_80ADFB38[1];
// static u8 D_80ADFB38[] = { 0x05 };
extern u8 D_80ADFB39[11];
// static u8 D_80ADFB39[] = { 0x05, 0x0C, 0x05, 0x04, 0x04, 0x2A, 0x29, 0x05, 0x05, 0x00, 0x00 };
extern u16 D_80ADFB44[6]; // Cancel dialog textId's
// static u16 D_80ADFB44[] = { 0x0775, 0x0776, 0x0777, 0x0774, 0x0775, 0x0000 };
extern u16 D_80ADFB50[6];
// static u16 D_80ADFB50[] = { 0x0771, 0x0772, 0x0773, 0x0770, 0x0771, 0x0000 };


u8 D_80ADFB28;

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/EnTakaraya_Init.s")
void EnTakaraya_Init(Actor* thisx, PlayState* play) { // void *arg0, ? arg1
    EnTakaraya* this = THIS;
    s32 *var_s0;
    s32 *var_s1;
    s32 temp_v0;
    u8 temp_t4;
    s32 i;


    Actor_ProcessInitChain(&this->actor, &D_80ADFB20);
    ActorShape_Init(&this->actor.shape, 0xC2700000, 0, 0);
    SkelAnime_InitFlex(play, &this->skelAnime, &D_06008FC8, &D_06009890, &this->jointTable, &this->morphTable, 0x18);
    this->params = (s16) (((s16) &this->actor.params >> 8) & 0xFF);
    this->actor.params = (s16) (&this->actor.params & 0xFF);
    var_s1 = &D_80ADFB00; // 4 u32
    if (!D_80ADFB28) {
        var_s0 = D_80ADFB10; // 4 u32
        do {
            *var_s1 = Lib_SegmentedToVirtual(*var_s1);
            *var_s0 = Lib_SegmentedToVirtual(*var_s0);
            var_s0 += 4;
            var_s1 += 4;
        } while (var_s0 != D_80ADFB20);
        D_80ADFB28 = true;
    }


    this->unk2AC = 2;

    // 0x2810 == TREASURE_CHEST_SHOP = 0x14, spawn = 1
    if (gSaveContext.save.entrance == ENTRANCE(TREASURE_CHEST_SHOP, 1)) { 
        func_801A2C20();
        if (gSaveContext.timerStates[TIMER_ID_MINIGAME_2] == 6) {
            this->actor.textId = 0x77A;
            gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = 5U;
            func_80ADF6DC(this);
            return;
        }
        this->actor.textId = 0x77C;
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_63_01);
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_63_02)) {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_63_02);
            func_80ADEDF8(this->actor);
            return;
        }
        func_80ADF6DC(this);
        return;
    }
    func_80ADEDF8(this->actor);
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/EnTakaraya_Destroy.s")
void EnTakaraya_Destroy(Actor* thisx, PlayState* play) {
    EnTakaraya* this = THIS;
    Flags_UnsetSwitch(play, 5);
    if (this->unk2AD == 0) {
        gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = 5;
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_63_01);
    }
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADED8C.s")
void func_80ADED8C(EnTakaraya* this) {
    if (this->unk2AC != 0) {
        this->unk2AC++; // If counter has started increase every update
        if ((this->unk2AC & 0xFF) == 4) {
            this->unk2AC = 0U; // when counter is 4, go back to 0
        }
    } else if (Rand_ZeroOne() < 0.02f) {
        this->unk2AC++; // 2% chance couter will fire on it's own?
    }
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADEDF8.s")
void func_80ADEDF8(Actor actor) {
    EnTakaraya* this = THIS;
    if (&D_06001384 == actor->overlayEntry->vramEnd) {
        Animation_MorphToPlayOnce(actor->overlayEntry->vromEnd, &D_0600AD98, 5.0f, actor);
    }
    this->actionFunc = &func_80ADEE4C;
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADEE4C.s")
void func_80ADEE4C(EnTakaraya* this, PlayState* play) {
    if (SkelAnime_Update(this->skelAnime) != 0) {
        if (&D_0600A280 == this->skelAnime->animation) {
            Animation_MorphToPlayOnce(this->skelAnime, &D_0600AD98, 5.0f);
        } else {
            Animation_MorphToLoop(this->skelAnime, &D_06009890, -4.0f);
        }
    }
    if (Actor_ProcessTalkRequest(this, play) != 0) {
        if (Text_GetFaceReaction(play, 0x2D) == 0) {
            Animation_MorphToPlayOnce(this->skelAnime, &D_0600A280, -4.0f);
        }
        func_80ADF03C(this); // Attach dialog selection action check
        return;
    }
    if (Actor_IsFacingPlayer(this, 8192.0f) != 0) {
        this->actor.textId = Text_GetFaceReaction(play, 0x2D);
        if (!(this->actor.textId & 0xFFFF)) {
            this->actor.textId = D_80ADFB2C[gSaveContext.save.playerForm];
        }
        this->unk2B0 = (s16) (gSaveContext.save.playerForm + this->params);
        func_800B8614(this, play, 120.0f);
    }
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADEF74.s")
void func_80ADEF74(EnTakaraya* this, PlayState* play) {
    u8 var_v1;

    if (Flags_GetSwitch(play, this->unk2B0) != 0) {
        var_v1 = D_80ADFB39[gSaveContext.save.playerForm];
    } else {
        var_v1 = D_80ADFB38[gSaveContext.save.playerForm];
    }
    Actor_SpawnAsChildAndCutscene(
        &play->actorCtx,
        play,
        ACTOR_OBJ_TAKARAYA_WALL,
        0.0f, 0.0f, 0.0f,
        0, 0, 5,
        (var_v1 << 5) + this->actor.params + 0xB000,
        (s32) this->actor.cutscene,
        0x3FF,
        0
    );
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADF03C.s")
void func_80ADF03C(EnTakaraya* this) {
    this->actionFunc = &func_80ADF050;
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADF050.s")
void func_80ADF050(EnTakaraya* this, PlayState* play) {
    void *sp24;
    s32 temp_v0_2;
    s32 temp_v1;
    u8 temp_t1;

    if (SkelAnime_Update(&this->skelAnime) != 0) {
        if (&D_0600AD98 == this->skelAnime.animation) {
            Animation_PlayOnce(&this->skelAnime, &D_06000968);
        } else if (&D_0600A280 == this->skelAnime.animation) {
            Animation_PlayLoop(&this->skelAnime, &D_06001384);
        } else {
            Animation_PlayLoop(&this->skelAnime, &D_06009890);
        }
    }
    temp_v0_2 = Message_GetState(&play->msgCtx);
    temp_v1 = temp_v0_2 & 0xFF;
    if ((temp_v0_2 == 2) || (temp_v1 == 6)) {
        if (this->actor.textId == 0x778) {
            func_80ADF2D4(this);
            return;
        }
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_63_01);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_63_02);
        func_80ADEDF8(this->actor);
        return;
    }
    if ((temp_v1 == 1) && (this->actor.textId != 0x778)) { // Bla bla bla lots of dialog whatever
        if (Message_ShouldAdvance(play) != 0) {
            Animation_MorphToPlayOnce(&this->skelAnime, &D_0600AD98, 5.0f);
        }
    } else if ((temp_v1 == 4) && (Message_ShouldAdvance(play) != 0)) { // Has advanced dialog
        if (play->msgCtx.choiceIndex == 0) {// Yes I'd like to play
            if (gSaveContext.save.playerData.rupees < play->msgCtx.unk1206C) { // Sorry you don't have enough rupees
                this->actor.textId = 0x77BU;
                if (&D_06009890 == this->skelAnime->animation) {
                    Animation_MorphToPlayOnce(&this->skelAnime, &D_06000968, 5.0f);
                }
                play_sound(0x4806);
            } else { // Deduct rupees and begin sequence
                sp24 = play + 0x10000;
                func_8019F208();
                Rupees_ChangeBy((s16) (play->msgCtx.unk1206C * -1));
                func_80ADEF74(this, play); // Spawn player at starting point of minigame
                this->actor.textId = 0x778U;
                if (&D_06009890 != this->skelAnime->animation) {
                    Animation_MorphToLoop(&this->skelAnime, &D_06009890, 5.0f);
                }
            }
        } else { // No thanks exit out
            func_8019F230();
            this->actor.textId = D_80ADFB44[gSaveContext.save.playerForm];
            if (&D_06009890 == this->skelAnime->animation) {
                Animation_MorphToPlayOnce(&this->skelAnime, &D_06000968, 5.0f);
            }
        }
        func_80151938(play, this->actor.textId);
    }
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADF2D4.s")
void func_80ADF2D4(EnTakaraya* this) {
    func_800B8FC0(this, 0x482C);
    func_801A2BB8(0x25);
    this->unk2AE_timerCounter = 0x91;
    SET_WEEKEVENTREG(WEEKEVENTREG_63_01);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_63_02);
    this->actionFunc = &func_80ADF338;
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADF338.s")
void func_80ADF338(EnTakaraya* this, PlayState* play) {
    Actor *sp48;
    f32 sp44;
    f32 sp40;
    f32 sp3C;
    f32 sp38;
    f32 sp34;
    f32 sp30;
    f32 sp2C;
    s16 camId;
    f32 temp_ft2;
    f32 var_fv1;
    s16 temp_v1;
    Actor *temp_s1;

    temp_s1 = play->actorCtx->actorLists[136].first;
    sp48 = play->actorCtx->actorLists[16].first;
    SkelAnime_Update(this->skelAnime);
    if ((ActorCutscene_GetCurrentIndex() == this->actor.cutscene) && (temp_s1 != NULL)) {
        this->unk2AE_timerCounter = (s16) (this->unk2AE_timerCounter - 1);
        camId = ActorCutscene_GetCurrentSubCamId(this->actor.cutscene);
        temp_v1 = this->unk2AE_timerCounter;
        if (temp_v1 >= 0x1A) {
            var_fv1 = 250.0f;
        } else {
            var_fv1 = ((temp_s1->xzDistToPlayer - 250.0f) * (f32) (0x19 - temp_v1) * 0.04f) + 250.0f;
        }
        sp2C = var_fv1;
        sp3C = (Math_SinS(temp_s1->yawTowardsPlayer) * var_fv1) + temp_s1->world.pos.x;
        sp40 = sp48->world.pos.y + 120.0f;
        sp44 = (Math_CosS(temp_s1->yawTowardsPlayer) * var_fv1) + temp_s1->world.pos.z;
        temp_ft2 = sp3C - (Math_SinS(temp_s1->yawTowardsPlayer) * 250.0f);
        sp34 = sp40 - 90.0f;
        sp30 = temp_ft2;
        sp38 = sp44 - (Math_CosS(temp_s1->yawTowardsPlayer) * 250.0f);
        Play_SetCameraAtEye(play, camId, &sp30, &sp3C);
        return;
    }
    if (this->unk2AE_timerCounter < 145) {
        func_80ADF4E0(this);
    }
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADF4E0.s")
void func_80ADF4E0(EnTakaraya* this) {
    Interface_StartTimer(TIMER_ID_MINIGAME_2, 45);
    this->unk2AE_timerCounter = 1;
    this->actionFunc = &func_80ADF520;
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADF520.s")
void func_80ADF520(EnTakaraya* this, PlayState* play) {
    SkelAnime_Update(this->skelAnime);
    if (Play_InCsMode(play) == 0) {
        if (Flags_GetTreasure(play, this->params) != 0) {
            Flags_SetSwitch(play, this->unk2B0);
            play->actorCtx.sceneFlags.chest &= (s32) (~(1 << this->actor.params));
            this->unk2AE_timerCounter = 0;
            gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = 6U;
            func_80ADF608(this, play); // End minigame sequence
            return;
        }
        if ((gSaveContext.timerCurTimes[32] == 0) && (gSaveContext.timerCurTimes[36] == 0)) {
            this->unk2AE_timerCounter = 0x32;
            Message_StartTextbox(play, 0x77D, this);
            gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = 5U;
            func_80ADF608(this, play); // End minigame sequence
        }
    }
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADF608.s")
void func_80ADF608(EnTakaraya* this, PlayState* play) { // End Minigame sequence
    func_800B7298(play, &this->actor, PLAYER_CSMODE_7);
    this->unk2AD = 1;
    this->actionFunc = &func_80ADF654; // Teleport Player back to game counter
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADF654.s")
void func_80ADF654(EnTakaraya* this, PlayState* play) { // Teleport Player back to game counter
    if (this->unk2AE_timerCounter > 0) { // Run down counter
        this->unk2AE_timerCounter--;
        return;
    }
    if (gSaveContext.timerStates[4] == TIMER_STATE_6) { // `TIMER_STATE_STOP` but with extra minigame checks
        // This is probably a successful capture of treasure chest
        play->transitionType = TRANS_TYPE_80;
        gSaveContext.nextTransitionType = TRANS_TYPE_03;
    } else {
        // Timer ran out?
        play->transitionType = TRANS_TYPE_64;
        gSaveContext.nextTransitionType = TRANS_TYPE_02;
    }
    gSaveContext.nextCutsceneIndex = 0;
    play->nextEntrance = ENTRANCE(TREASURE_CHEST_SHOP, 1);
    play->transitionTrigger = TRANS_TRIGGER_START;
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADF6DC.s")
void func_80ADF6DC(EnTakaraya* this) {
    Animation_PlayLoop(&this->skelAnime, &D_06001384);
    this->unk2AC = 0;
    this->actor.flags |= ACTOR_FLAG_10000;
    this->actionFunc = &func_80ADF730;
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADF730.s")
void func_80ADF730(EnTakaraya* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Actor_ProcessTalkRequest(this, play) != 0) {
        this->actor.flags &= (u32) (~ACTOR_FLAG_10000);
        func_80ADF7B8(this);
        return;
    }
    this->unk2B0 = gSaveContext.save.playerForm + this->params;
    func_800B8614(this, play, 120.0f);
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADF7B8.s")
void func_80ADF7B8(EnTakaraya* this) {
    this->actionFunc = &func_80ADF7CC;
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADF7CC.s")
void func_80ADF7CC(EnTakaraya* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((Message_GetState(&play->msgCtx) == 5) && (Message_ShouldAdvance(play) != 0)) {
        if (this->actor.textId == 0x77A) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_63_02)) {
                func_801477B4(play, 0x77CU);
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_63_01);
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_63_02);
                func_80ADEDF8(this->actor);
                return;
            }
            this->actor.textId = 0x77CU;
            func_80151938(play, 0x77CU);
            return;
        }
        this->actor.textId = D_80ADFB50[gSaveContext.save.playerForm];
        func_80151938(play, D_80ADFB50[gSaveContext.save.playerForm]);
        Animation_MorphToPlayOnce(&this->skelAnime, &D_0600AD98, 0x40A00000);
        func_80ADF03C(this);
    }
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/EnTakaraya_Update.s")
void EnTakaraya_Update(Actor* thisx, PlayState* play) {
    EnTakaraya* this = THIS;
    Vec3s* torsoRot;

    this->actionFunc(this, play);
    Actor_TrackPlayer(play, this, &this->headRot, &torsoRot, (Vec3f *) &this->actor.focus);
    func_80ADED8C(this);
}

// typedef s32 (*OverrideLimbDrawOpa)(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, struct Actor* thisx);
// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADF94C.s")
s32 EnTakaraya_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) { // s32 arg0, s32 arg1, ? arg2, ? arg3, s16 *arg4, void *arg5
    if (limbIndex == 5) {
        rot->x += this->headRot.y;
    }
    return false;
}

// typedef void (*PostLimbDrawOpa)(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, struct Actor* thisx);
// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/func_80ADF984.s")
void EnTakaraya_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnTakaraya* this = THIS;

    if (limbIndex == 5) {
        Matrix_RotateYS(0x400 - this->headRot.x, MTXMODE_APPLY);
        Matrix_MultVecX(500.0f, &this->actor.focus.pos);
    }
}

// #pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Takaraya/EnTakaraya_Draw.s")
void EnTakaraya_Draw(Actor* thisx, PlayState* play) {
    EnTakaraya* this = THIS;
    Actor *focalActor;

    func_8012C28C(play->state.gfxCtx);
    if ((gSaveContext.save.playerForm == PLAYER_FORM_DEKU) || (gSaveContext.save.playerForm == PLAYER_FORM_HUMAN)) {
        focalActor = play->mainCamera.focalActor;
        focalActor->id = ACTOR_EN_BOOM; // id = 0020, category = 06, room = DB// ACTOR_EN_BOOM?
        focalActor->category = ACTORCAT_PROP;
        focalActor->room = 0xDB;
        focalActor->flags = D_80ADFB10[this->unk2AC];
    } else {
        focalActor = play->mainCamera.focalActor;
        focalActor->id = ACTOR_EN_BOOM; // id = 0020, category = 06, room = DB// ACTOR_EN_BOOM?
        focalActor->category = ACTORCAT_PROP;
        focalActor->room = 0xDB;
        focalActor->flags = D_80ADFB00[this->unk2AC];
    }
    SkelAnime_DrawTransformFlexOpa(play, &this->skelAnime->skeleton, &this->skelAnime->baseTransl.y, &this->skelAnime->dListCount, EnTakaraya_OverrideLimbDraw, 0, EnTakaraya_PostLimbDraw, this->actor);
}