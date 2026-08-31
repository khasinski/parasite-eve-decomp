/* CC1_FLAGS: -g3 -G1 */
/* MASPSX_FLAGS: --expand-div -G1 */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;

#define NULL ((void *)0)

#include <m2c_macros.h>

M2C_UNK MenuWidget_InitPool();                      /* extern */
M2C_UNK Entity_SetActionMode();                            /* extern */
M2C_UNK Battle_ReturnToIdle();                            /* extern */
s8 Pad_GetMenuPressedBitOrDisabled();                                 /* extern */
M2C_UNK Battle_FlushScriptSounds();                            /* extern */
M2C_UNK Battle_DrawStatusPanel();                            /* extern */
M2C_UNK Tbl_ResetAll();                            /* extern */
M2C_UNK Render_BeginSceneLoad();                            /* extern */
M2C_UNK Asset_Find08w(u16, M2C_UNK, s16, s16, s32); /* extern */
M2C_UNK Asset_Find08Alt(M2C_UNK, M2C_UNK, s16, s16, s32); /* extern */
M2C_UNK Akao_SendTableCommand(s32, M2C_UNK, M2C_UNK, M2C_UNK, s32); /* extern */
M2C_UNK Pm_SendCmd(s32, M2C_UNK, M2C_UNK, M2C_UNK, s32, s32); /* extern */
s32 rcos(s16, void *);                              /* extern */
s32 rsin(s16, void *);                              /* extern */
M2C_UNK Entity_ResolveDropTable(void *);                      /* static */
s16 Entity_ApplyHitAndSetAnim(void *);                 /* static */
void Entity_UpdateTimers(void);
extern volatile s32 D_8009CDDC;
/* Separate declarations preserve the original GCC quantities for each tail. */
extern volatile s32 draw_slot_entity0 __asm__("D_8009CDDC");
extern volatile s32 draw_slot_entity1 __asm__("D_8009CDDC");
extern volatile s32 draw_slot_entity2 __asm__("D_8009CDDC");
extern volatile s32 draw_slot_status0 __asm__("D_8009CDDC");
extern volatile s32 draw_slot_status1 __asm__("D_8009CDDC");
extern volatile s32 draw_slot_status2 __asm__("D_8009CDDC");
extern s8 D_8009CE30;
extern s8 D_8009CE34;
extern s32 D_8009D1A0;
extern s32 D_8009D1AC;
extern s8 D_8009D1CE[];
extern u8 D_8009D1D4[];
extern s32 D_8009D1E8;
extern s32 D_8009D200;
extern void *D_8009D20C;
extern u8 D_8009D234[];
extern s8 D_8009D244[];
extern void *D_8009D254;
extern void *D_8009D278;
extern s32 D_8009D28C;
extern s32 D_8009D2E8;
extern s32 D_8009D2FC;
extern u8 D_800B00EC[];
extern u8 D_800B00ED[];
extern u8 D_800B00EE[];
extern u8 D_800B00F4[];
extern u8 D_800B00F5[];
extern u8 D_800B00F6[];
extern u8 D_800B00FC[];
extern u8 D_800B00FD[];
extern u8 D_800B00FE[];
extern u8 D_800B0104[];
extern u8 D_800B0105[];
extern u8 D_800B0106[];
extern u8 D_800B0110[];
extern u8 D_800B0111[];
extern u8 D_800B0112[];
extern u8 D_800B0118[];
extern u8 D_800B0119[];
extern u8 D_800B011A[];
extern u8 D_800B0120[];
extern u8 D_800B0121[];
extern u8 D_800B0122[];
extern u8 D_800B0128[];
extern u8 D_800B0129[];
extern u8 D_800B012A[];
extern u8 D_800B0134[];
extern u8 D_800B0135[];
extern u8 D_800B0136[];
extern u8 D_800B013C[];
extern u8 D_800B013D[];
extern u8 D_800B013E[];
extern u8 D_800B0144[];
extern u8 D_800B0145[];
extern u8 D_800B0146[];
extern u8 D_800B014C[];
extern u8 D_800B014D[];
extern u8 D_800B014E[];
extern u8 D_800B0158[];
extern u8 D_800B0159[];
extern u8 D_800B015A[];
extern u8 D_800B0160[];
extern u8 D_800B0161[];
extern u8 D_800B0162[];
extern u8 D_800B0168[];
extern u8 D_800B0169[];
extern u8 D_800B016A[];
extern u8 D_800B0170[];
extern u8 D_800B0171[];
extern u8 D_800B0172[];
extern u8 D_800B017C[];
extern u8 D_800B017D[];
extern u8 D_800B017E[];
extern u8 D_800B0184[];
extern u8 D_800B0185[];
extern u8 D_800B0186[];
extern u8 D_800B018C[];
extern u8 D_800B018D[];
extern u8 D_800B018E[];
extern u8 D_800B0194[];
extern u8 D_800B0195[];
extern u8 D_800B0196[];
extern u8 D_800B01A0[];
extern u8 D_800B01A1[];
extern u8 D_800B01A2[];
extern u8 D_800B01A8[];
extern u8 D_800B01A9[];
extern u8 D_800B01AA[];
extern u8 D_800B01B0[];
extern u8 D_800B01B1[];
extern u8 D_800B01B2[];
extern u8 D_800B01B8[];
extern u8 D_800B01B9[];
extern u8 D_800B01BA[];
extern volatile s32 D_800B0E08;
extern u8 D_800B692C[];
extern u8 D_800B692D[];
extern u8 D_800B692E[];
extern u8 D_800B6948[];
extern u8 D_800B6949[];
extern u8 D_800B694A[];
extern s32 D_800BCF88;

static inline u8 Battle_GetReactionTimer(void) {
    return D_8009D234[0];
}

static inline void Battle_SetReactionTimer(u8 value) {
    D_8009D234[0] = value;
}

void Menu_MainUpdate(s32 arg0) {
    char unused_stack[0x2C0];
    s16 temp_v0_7;
    s16 temp_v0_8;
    s16 temp_v1_10;
    s16 temp_v1_11;
    s16 temp_v1_13;
    s16 temp_v1_8;
    s32 *temp_s2;
    s32 temp_a0_3;
    s32 temp_a0_4;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v1;
    s32 temp_v1_4;
    s32 temp_v1_6;
    s32 temp_v1_7;
    s32 temp_v1_9;
    register s32 var_a2;
    register s32 var_a1;
    register s32 var_a3;
    register s32 var_t0;
    register s32 var_t1;
    s32 var_v0_2;
    s32 var_v0_3;
    s32 var_v0_4;
    register s32 var_v1;
    register u32 var_a0;
    register u8 var_a0_2;
    u16 temp_a1;
    register u32 var_v0;
    u32 temp_a2;
    u8 *temp_a0_2;
    u8 temp_a0;
    u8 temp_v0_6;
    u8 temp_v1_12;
    register void *state0;
    register void *temp_a1_2;
    void *temp_v1_5;
    void *var_s0;

    state0 = D_8009D278;
    temp_s2 = state0 + 0x4C;
    if (arg0 & 0xFF) {
        if (!(D_8009D1A0 & 0x100)) {
            temp_a2 = M2C_FIELD(state0, u32 *, 0x4C);
            temp_a1 = M2C_FIELD(state0, u16 *, 0x10) + M2C_FIELD(state0, u16 *, 0x24);
            temp_v1 = temp_a2 & 0xC0;
            M2C_FIELD(state0, u16 *, 0x10) = temp_a1;
            if ((temp_v1 == 0x40) || (temp_v1 == 0x80)) {
                M2C_FIELD(state0, u16 *, 0x10) = temp_a1 - ((M2C_FIELD(state0, u16 *, 0x24) * 2) / 5);
            } else if (temp_a2 & 0x100) {
                M2C_FIELD(state0, u16 *, 0x10) = temp_a1 + ((u16) M2C_FIELD(state0, u16 *, 0x24) >> 1);
            }
            temp_a1_2 = D_8009D278;
            if ((M2C_FIELD(temp_a1_2, s32 *, 8) < M2C_FIELD(temp_a1_2, s32 *, 0x28)) && !(*temp_s2 & 0x2600)) {
                temp_v0 = M2C_FIELD(temp_a1_2, s32 *, 0x30) - 3;
                M2C_FIELD(temp_a1_2, s32 *, 0x30) = temp_v0;
                if (temp_v0 <= 0) {
                    M2C_FIELD(temp_a1_2, s32 *, 0x30) = 1;
                }
                temp_v0_2 = M2C_FIELD(temp_a1_2, s32 *, 0x2C) - ((s32) M2C_FIELD(temp_a1_2, s32 *, 0x28) / (s32) (M2C_FIELD(temp_a1_2, s32 *, 0x30) * 0x64));
                M2C_FIELD(temp_a1_2, s32 *, 0x2C) = temp_v0_2;
                if (temp_v0_2 < 0x1999) {
                    M2C_FIELD(temp_a1_2, s32 *, 0x2C) = 0x1999;
                }
                temp_v0_3 = M2C_FIELD(D_8009D278, s32 *, 8) + M2C_FIELD(temp_a1_2, s32 *, 0x2C);
                M2C_FIELD(D_8009D278, s32 *, 8) = temp_v0_3;
                if (temp_v0_3 >= M2C_FIELD(temp_a1_2, s32 *, 0x28)) {
                    volatile s32 *callback;

                    M2C_FIELD(temp_a1_2, s32 *, 0x34) = 0xF0;
                    callback = &D_800B0E08;
                    if (*callback != 0) {
                        Akao_SendTableCommand(*callback, 0x455, 0, 0x80, 0x7F);
                    }
                }
            }
        }
        if ((arg0 & 0xFF) == 1) {
            temp_a0 = M2C_FIELD(D_8009D254, u8 *, 0xE);
            if ((temp_a0 == M2C_FIELD(D_8009D278, u8 *, 0x12)) || (temp_a0 == 5)) {
                if (!(M2C_FIELD(M2C_FIELD(D_8009D278, void **, 0x6C), s32 *, 4) & 0x4000) || !(M2C_FIELD(D_8009D278, u32 *, 0x4C) & 0x4000)) {
                    goto block_clear_bit0;
                }
                if ((Pad_GetMenuPressedBitOrDisabled(temp_a0) << 0x18) <= 0) {
                    goto block_clear_bit0;
                }
                goto block_range_check;
block_clear_bit0:
                D_8009D2E8 &= ~1;
                goto block_after_d2e8;
            }
block_range_check:
            if ((u8) M2C_FIELD(D_8009D254, u8 *, 0xE) >= 0xEU) {
                D_8009D2E8 &= ~1;
            } else {
                goto block_27;
            }
            goto block_after_d2e8;
        }
    } else {
block_27:
        var_v0_2 = D_8009D2E8 | 1;
block_28:
        D_8009D2E8 = var_v0_2;
    }
block_after_d2e8:
    if (M2C_FIELD(D_8009D278, s32 *, 8) < 0) {
        M2C_FIELD(D_8009D278, s32 *, 8) = 0;
    }
    if (*temp_s2 & 0x2000) {
        var_v1 = D_8009D1E8 & 3;
        if (var_v1 == 0) {
            var_t1 = 0xFF;
            var_t0 = 0x3D;
            var_a3 = 0x81;
            var_a2 = 0x83;
            var_a1 = 0x13;
            {
            do { } while (0);
            var_v1 = D_8009CDDC;
            do { } while (0);
            var_a0 = 1;
            *(&D_800B0158[0] + (var_v1 * 0x48)) = var_t1;
            }
            *(&D_800B0159[0] + (D_8009CDDC * 0x48)) = var_t0;
            *(&D_800B015A[0] + (D_8009CDDC * 0x48)) = var_a3;
            *(&D_800B0160[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0161[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B0162[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0168[0] + (D_8009CDDC * 0x48)) = var_t1;
            *(&D_800B0169[0] + (D_8009CDDC * 0x48)) = var_t0;
            *(&D_800B016A[0] + (D_8009CDDC * 0x48)) = var_a3;
            *(&D_800B0170[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0171[0] + (D_8009CDDC * 0x48)) = var_a1;
            var_v0_3 = draw_slot_entity0;
            goto block_40;
        }
        if (var_v1 == 1) {
            var_a2 = 0xC1;
            var_a1 = 0x28;
            {
            do { } while (0);
            var_v1 = D_8009CDDC;
            do { } while (0);
            var_a0 = 0x41;
            *(&D_800B0158[0] + (var_v1 * 0x48)) = var_a2;
            }
            *(&D_800B0159[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B015A[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0160[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0161[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B0162[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0168[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0169[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B016A[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0170[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0171[0] + (D_8009CDDC * 0x48)) = var_a1;
            var_v0_3 = draw_slot_entity1;
            goto block_40;
        }
        if (var_v1 == 2) {
            var_t1 = 0x83;
            var_t0 = 0x13;
            var_a3 = 1;
            var_a2 = 0xFF;
            var_a1 = 0x3D;
            {
            do { } while (0);
            var_v1 = D_8009CDDC;
            do { } while (0);
            var_a0 = 0x81;
            *(&D_800B0158[0] + (var_v1 * 0x48)) = var_t1;
            }
            *(&D_800B0159[0] + (D_8009CDDC * 0x48)) = var_t0;
            *(&D_800B015A[0] + (D_8009CDDC * 0x48)) = var_a3;
            *(&D_800B0160[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0161[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B0162[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0168[0] + (D_8009CDDC * 0x48)) = var_t1;
            *(&D_800B0169[0] + (D_8009CDDC * 0x48)) = var_t0;
            *(&D_800B016A[0] + (D_8009CDDC * 0x48)) = var_a3;
            *(&D_800B0170[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0171[0] + (D_8009CDDC * 0x48)) = var_a1;
            var_v0_3 = draw_slot_entity2;
            goto block_40;
        }
        var_a2 = 0xC1;
        if (var_v1 == 3) {
            var_a2 = 0xC1;
            var_a1 = 0x28;
            {
            do { } while (0);
            var_v1 = D_8009CDDC;
            do { } while (0);
            var_a0 = 0x41;
            *(&D_800B0158[0] + (var_v1 * 0x48)) = var_a2;
            }
            *(&D_800B0159[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B015A[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0160[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0161[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B0162[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0168[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0169[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B016A[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0170[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0171[0] + (D_8009CDDC * 0x48)) = var_a1;
            var_v0_3 = D_8009CDDC;
block_40:
            *(&D_800B0172[0] + ((var_v0_3 * 9) * 8)) = var_a0;
        }
        if (M2C_FIELD(D_8009D254, u8 *, 0xE) != 0x12) {
            Entity_SetActionMode(D_8009D254, 0x12U, var_a2);
        }
        if (D_8009CE30 == 0x5A) {
            s32 reset_flags;
            u32 reset_operand;

            Entity_SetActionMode(D_8009D254, M2C_FIELD(D_8009D278, u8 *, 0x12));
            reset_flags = *temp_s2;
            reset_operand = ~0x2000U;
            reset_flags &= reset_operand;
            reset_operand = (u32) D_8009D278;
            D_8009CE30 = 0;
            *temp_s2 = reset_flags;
            M2C_FIELD((void *) reset_operand, s32 *, 8) = 0x10000;
            D_800B0158[0] = 0xFF;
            D_800B0159[0] = 0x3D;
            D_800B015A[0] = 0x81;
            D_800B0160[0] = 0x83;
            D_800B0161[0] = 0x13;
            D_800B0162[0] = 1;
            D_800B0168[0] = 0xFF;
            D_800B0169[0] = 0x3D;
            D_800B016A[0] = 0x81;
            D_800B0170[0] = 0x83;
            D_800B0171[0] = 0x13;
            D_800B0172[0] = 1;
            D_800B01A0[0] = 0xFF;
            D_800B01A1[0] = 0x3D;
            D_800B01A2[0] = 0x81;
            D_800B01A8[0] = 0x83;
            D_800B01A9[0] = 0x13;
            D_800B01AA[0] = 1;
            D_800B01B0[0] = 0xFF;
            D_800B01B1[0] = 0x3D;
            D_800B01B2[0] = 0x81;
            D_800B01B8[0] = 0x83;
            D_800B01B9[0] = 0x13;
            D_800B01BA[0] = 1;
        } else {
            D_8009CE30 += 1;
            M2C_FIELD(D_8009D254, s32 *, 0x68) = 0;
            M2C_FIELD(D_8009D254, s32 *, 0x6C) = 0;
            M2C_FIELD(D_8009D254, s32 *, 0x70) = 0;
        }
    }
    if (!(D_8009D1A0 & 0x100)) {
        register void *timer_state;

        timer_state = D_8009D278;
        temp_v0_4 = M2C_FIELD(timer_state, s32 *, 0x34);
        if (temp_v0_4 > 0) {
            temp_v0_4 -= 1;
            M2C_FIELD(timer_state, s32 *, 0x34) = temp_v0_4;
            if (temp_v0_4 != 0) {
                var_v1 = D_8009D1E8 & 3;
                if (var_v1 == 0) {
                    var_t0 = 0x82;
                    var_a3 = 0x36;
                    var_a2 = 0x4A;
                    var_a1 = 0xFF;
                    {
                    do { } while (0);
                    var_v1 = D_8009CDDC;
                    do { } while (0);
                    var_a0_2 = 0x3B;
                    *(&D_800B0134[0] + (var_v1 * 0x48)) = 0;
                    }
                    *(&D_800B0135[0] + (D_8009CDDC * 0x48)) = var_t0;
                    *(&D_800B0136[0] + (D_8009CDDC * 0x48)) = var_a3;
                    *(&D_800B013C[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B013D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B013E[0] + (D_8009CDDC * 0x48)) = 0x3B;
                    *(&D_800B0144[0] + (D_8009CDDC * 0x48)) = 0;
                    *(&D_800B0145[0] + (D_8009CDDC * 0x48)) = var_t0;
                    *(&D_800B0146[0] + (D_8009CDDC * 0x48)) = var_a3;
                    *(&D_800B014C[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B014D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    var_v0_4 = draw_slot_status0;
                    goto block_57;
                }
                if (var_v1 == 1) {
                    var_a2 = 0x25;
                    var_a1 = 0xC1;
                    {
                    do { } while (0);
                    var_v1 = D_8009CDDC;
                    do { } while (0);
                    var_a0_2 = 0x39;
                    *(&D_800B0134[0] + (var_v1 * 0x48)) = var_a2;
                    }
                    *(&D_800B0135[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B0136[0] + (D_8009CDDC * 0x48)) = var_a0_2;
                    *(&D_800B013C[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B013D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B013E[0] + (D_8009CDDC * 0x48)) = var_a0_2;
                    *(&D_800B0144[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B0145[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B0146[0] + (D_8009CDDC * 0x48)) = var_a0_2;
                    *(&D_800B014C[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B014D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    var_v0_4 = draw_slot_status1;
                    goto block_57;
                }
                if (var_v1 == 2) {
                    var_t0 = 0x4A;
                    var_a3 = 0xFF;
                    var_a2 = 0x3B;
                    var_a1 = 0x82;
                    {
                    do { } while (0);
                    var_v1 = D_8009CDDC;
                    do { } while (0);
                    var_a0_2 = 0x36;
                    *(&D_800B0134[0] + (var_v1 * 0x48)) = var_t0;
                    }
                    *(&D_800B0135[0] + (D_8009CDDC * 0x48)) = var_a3;
                    *(&D_800B0136[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B013C[0] + (D_8009CDDC * 0x48)) = 0;
                    *(&D_800B013D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B013E[0] + (D_8009CDDC * 0x48)) = var_a0_2;
                    *(&D_800B0144[0] + (D_8009CDDC * 0x48)) = var_t0;
                    *(&D_800B0145[0] + (D_8009CDDC * 0x48)) = var_a3;
                    *(&D_800B0146[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B014C[0] + (D_8009CDDC * 0x48)) = 0;
                    *(&D_800B014D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    var_v0_4 = draw_slot_status2;
                    goto block_57;
                }
                if (var_v1 == 3) {
                    var_a2 = 0x25;
                    var_a1 = 0xC1;
                    {
                    do { } while (0);
                    var_v1 = D_8009CDDC;
                    do { } while (0);
                    var_a0_2 = 0x39;
                    *(&D_800B0134[0] + (var_v1 * 0x48)) = var_a2;
                    }
                    *(&D_800B0135[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B0136[0] + (D_8009CDDC * 0x48)) = var_a0_2;
                    *(&D_800B013C[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B013D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B013E[0] + (D_8009CDDC * 0x48)) = var_a0_2;
                    *(&D_800B0144[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B0145[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B0146[0] + (D_8009CDDC * 0x48)) = var_a0_2;
                    *(&D_800B014C[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B014D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    var_v0_4 = D_8009CDDC;
block_57:
                    *(&D_800B014E[0] + ((var_v0_4 * 9) * 8)) = var_a0_2;
                }
                if (M2C_FIELD(D_8009D278, s32 *, 8) < M2C_FIELD(D_8009D278, s32 *, 0x28)) {
                    M2C_FIELD(D_8009D278, s32 *, 0x34) = 0;
                }
                if (M2C_FIELD(D_8009D278, s32 *, 0x34) == 0) {
                    goto block_61;
                }
            } else {
block_61:
                D_800B0134[0] = 0;
                D_800B0135[0] = 0x82;
                D_800B0136[0] = 0x36;
                D_800B013C[0] = 0x4A;
                D_800B013D[0] = 0xFF;
                D_800B013E[0] = 0x3B;
                D_800B0144[0] = 0;
                D_800B0145[0] = 0x82;
                D_800B0146[0] = 0x36;
                D_800B014C[0] = 0x4A;
                D_800B014D[0] = 0xFF;
                D_800B014E[0] = 0x3B;
                D_800B017C[0] = 0;
                D_800B017D[0] = 0x82;
                D_800B017E[0] = 0x36;
                D_800B0184[0] = 0x4A;
                D_800B0185[0] = 0xFF;
                D_800B0186[0] = 0x3B;
                D_800B018C[0] = 0;
                D_800B018D[0] = 0x82;
                D_800B018E[0] = 0x36;
                D_800B0194[0] = 0x4A;
                D_800B0195[0] = 0xFF;
                D_800B0196[0] = 0x3B;
            }
        }
        temp_v1_4 = *temp_s2;
        if (temp_v1_4 & 0x800000) {
            temp_v0_6 = Battle_GetReactionTimer() - 1;
            Battle_SetReactionTimer(temp_v0_6);
            if ((temp_v0_6 << 0x18) <= 0) {
                *temp_s2 &= 0xFF7FFFFF;
            }
        } else if (!(temp_v1_4 & 0x180000)) {
            var_s0 = D_8009D20C;
            if (var_s0 != NULL) {
                do {
                    if (var_s0 != D_8009D254) {
                        {
                        register void *entity0;

                        entity0 = M2C_FIELD(var_s0, void **, 0);
                        if ((entity0 != NULL) && !(M2C_FIELD(var_s0, s32 *, 0x98) & 0x10)) {
                            temp_v1_5 = entity0;
                            temp_a0_2 = M2C_FIELD(temp_v1_5, u8 **, 0x18);
                            if ((temp_a0_2 != NULL) && (*temp_s2 & 0x4000)) {
                                temp_v1_6 = M2C_FIELD(temp_v1_5, s32 *, 0);
                                if ((temp_v1_6 < 0) && ((u32) (*temp_a0_2 - 1) < 2U)) {
                                    temp_v1_7 = ((u32) temp_v1_6 >> 0x15) & 7;
                                    if (temp_v1_7 < 3) {
                                        Asset_Find08w(M2C_FIELD(((temp_v1_7 * 2) + temp_v1_5), u16 *, 0xB6), 0, M2C_FIELD(var_s0, s16 *, 0x268), M2C_FIELD(var_s0, s16 *, 0x26A), (s32) M2C_FIELD(var_s0, s16 *, 0x26C));
                                    }
                                    *temp_s2 |= 0x10000000;
                                    Entity_ResolveDropTable(var_s0);
                                    M2C_FIELD(D_8009D254, s32 *, 0x68) = 0;
                                    M2C_FIELD(D_8009D254, s32 *, 0x6C) = 0;
                                    M2C_FIELD(D_8009D254, s32 *, 0x70) = 0;
                                    {
                                    u16 v9c;
                                    s32 vflags;

                                    v9c = M2C_FIELD(temp_v1_5, u16 *, 0x9C);
                                    vflags = M2C_FIELD(temp_v1_5, s32 *, 0);
                                    v9c = (u16) (v9c + 1);
                                    vflags &= 0x7FFFFFFF;
                                    M2C_FIELD(temp_v1_5, s32 *, 0) = vflags;
                                    M2C_FIELD(temp_v1_5, u16 *, 0x9C) = v9c;
                                    }
                                    {
                                    s32 s2_flags;

                                    s2_flags = *temp_s2;
                                    D_8009CE34 = 0x5A;
                                    *temp_s2 = s2_flags | 0x01000000;
                                    }
                                }
                                {
                                register void *desc_state;

                                desc_state = D_8009D278;
                                temp_v1_8 = M2C_FIELD(desc_state, s16 *, 0xC);
                                if (M2C_FIELD(desc_state, s16 *, 0x1C) >= temp_v1_8) {
                                    register void *d254;

                                    d254 = D_8009D254;
                                    M2C_FIELD(desc_state, s16 *, 0x50) = (s16) (M2C_FIELD(desc_state, u16 *, 0xE) - temp_v1_8);
                                    {
                                    u16 copy210;

                                    copy210 = M2C_FIELD(d254, u16 *, 0x210);
                                    M2C_FIELD(D_8009D278, u16 *, 0x52) = copy210;
                                    }
                                    {
                                    u16 copy212;
                                    register void *desc_tail;

                                    copy212 = M2C_FIELD(d254, u16 *, 0x212);
                                    M2C_FIELD(D_8009D278, u8 *, 0x56) = 0x1EU;
                                    desc_tail = D_8009D278;
                                    M2C_FIELD(desc_state, u16 *, 0x54) = copy212;
                                    if (M2C_FIELD(desc_tail, s16 *, 0x50) == 0) {
                                        M2C_FIELD(desc_tail, s8 *, 0x57) = 0;
                                    } else {
                                        M2C_FIELD(desc_tail, s8 *, 0x57) = (s8) (((u32) M2C_FIELD(desc_tail, u32 *, 0x4C) >> 0xE) & 2);
                                    }
                                    }
                                    M2C_FIELD(D_8009D278, u32 *, 0x4C) = (u32) (M2C_FIELD(D_8009D278, u32 *, 0x4C) & 0xFFFF7FFF);
                                }
                                }
                            }
                            if (M2C_FIELD(var_s0, s32 *, 0x98) & 0x02000000) {
                                temp_v1_9 = *temp_s2;
                                if (!(temp_v1_9 & 0x01000000) && (M2C_FIELD(temp_v1_5, s32 *, 0x10) > 0)) {
                                    if (!(temp_v1_9 & 0x200)) {
                                        M2C_FIELD(D_8009D278, s16 *, 0xC) = (s16) ((u16) M2C_FIELD(D_8009D278, s16 *, 0xC) - M2C_FIELD(temp_v1_5, u8 *, 0x92));
                                    }
                                    {
                                    s32 s2_flags2;
                                    register void *desc_state;

                                    D_8009CE34 = 0x5A;
                                    s2_flags2 = *temp_s2;
                                    desc_state = D_8009D278;
                                    *temp_s2 = s2_flags2 | 0x01000000;
                                    temp_v1_10 = M2C_FIELD(desc_state, s16 *, 0xC);
                                    if (M2C_FIELD(desc_state, s16 *, 0x1C) >= temp_v1_10) {
                                        register void *d254;

                                        d254 = D_8009D254;
                                        M2C_FIELD(desc_state, s16 *, 0x50) = (s16) (M2C_FIELD(desc_state, u16 *, 0xE) - temp_v1_10);
                                        {
                                        u16 copy210;

                                        copy210 = M2C_FIELD(d254, u16 *, 0x210);
                                        M2C_FIELD(desc_state, u16 *, 0x52) = copy210;
                                        }
                                        {
                                        u16 copy212;
                                        register void *desc_tail;

                                        copy212 = M2C_FIELD(d254, u16 *, 0x212);
                                        M2C_FIELD(desc_state, u8 *, 0x56) = 0x1EU;
                                        desc_tail = D_8009D278;
                                        M2C_FIELD(desc_state, u16 *, 0x54) = copy212;
                                        if (M2C_FIELD(desc_tail, s16 *, 0x50) == 0) {
                                            M2C_FIELD(desc_tail, s8 *, 0x57) = 0;
                                        } else {
                                            M2C_FIELD(desc_tail, s8 *, 0x57) = (s8) (((u32) M2C_FIELD(desc_tail, u32 *, 0x4C) >> 0xE) & 2);
                                        }
                                        }
                                        M2C_FIELD(D_8009D278, u32 *, 0x4C) = (u32) (M2C_FIELD(D_8009D278, u32 *, 0x4C) & 0xFFFF7FFF);
                                    }
                                    }
                                    {
                                    register s32 clear_flags;

                                    clear_flags = 0xFFF3FFFF;
                                    M2C_FIELD(D_8009D254, s32 *, 0x68) = 0;
                                    M2C_FIELD(D_8009D254, s32 *, 0x6C) = 0;
                                    M2C_FIELD(D_8009D254, s32 *, 0x70) = 0;
                                    M2C_FIELD(D_8009D254, s32 *, 0x98) = (s32) (M2C_FIELD(D_8009D254, s32 *, 0x98) & clear_flags);
                                    M2C_FIELD(D_8009D278, s16 *, 0x4A) = Entity_ApplyHitAndSetAnim(var_s0);
                                    }
                                    M2C_FIELD(D_8009D278, u8 *, 0x49) = (u8) M2C_FIELD(temp_v1_5, u8 *, 0x93);
                                    M2C_FIELD(D_8009D278, s8 *, 0x48) = 6;
                                }
                            }
                        }
                        }
                    }
                    var_s0 = M2C_FIELD(var_s0, void **, 4);
                } while (var_s0 != NULL);
            }
            {
            register void *desc_state;

            desc_state = D_8009D278;
            temp_v1_11 = M2C_FIELD(desc_state, s16 *, 0xC);
            temp_v0_7 = (s16) M2C_FIELD(desc_state, u16 *, 0xE);
            if (temp_v1_11 < temp_v0_7) {
                if (M2C_FIELD(desc_state, s16 *, 0x1C) >= temp_v1_11) {
                    register void *d254;

                    d254 = D_8009D254;
                    M2C_FIELD(desc_state, s16 *, 0x50) = (s16) (temp_v0_7 - temp_v1_11);
                    {
                    u16 copy210;

                    copy210 = M2C_FIELD(d254, u16 *, 0x210);
                    M2C_FIELD(D_8009D278, u16 *, 0x52) = copy210;
                    }
                    {
                    u16 copy212;
                    register void *desc_tail;

                    copy212 = M2C_FIELD(d254, u16 *, 0x212);
                    M2C_FIELD(D_8009D278, u8 *, 0x56) = 0x1EU;
                    desc_tail = D_8009D278;
                    M2C_FIELD(desc_state, u16 *, 0x54) = copy212;
                    if (M2C_FIELD(desc_tail, s16 *, 0x50) == 0) {
                        M2C_FIELD(desc_tail, s8 *, 0x57) = 0;
                    } else {
                        M2C_FIELD(desc_tail, s8 *, 0x57) = (s8) (((u32) M2C_FIELD(desc_tail, u32 *, 0x4C) >> 0xE) & 2);
                    }
                    }
                    M2C_FIELD(D_8009D278, u32 *, 0x4C) = (u32) (M2C_FIELD(D_8009D278, u32 *, 0x4C) & 0xFFFF7FFF);
                }
                M2C_FIELD(D_8009D278, u16 *, 0xE) = (u16) M2C_FIELD(D_8009D278, s16 *, 0xC);
            }
            }
        }
        *temp_s2 &= ~0x4000;
        if (M2C_FIELD(D_8009D278, s8 *, 0x48) != 0) {
            temp_a0_3 = M2C_FIELD(D_8009D254, s32 *, 0x98);
            if ((temp_a0_3 & 0xC0000) || (M2C_FIELD(D_8009D278, u8 *, 0x49) == 0)) {
                M2C_FIELD(D_8009D254, s32 *, 0x98) = (s32) (temp_a0_3 & 0xFFF3FFFF);
                M2C_FIELD(D_8009D278, s8 *, 0x48) = 0;
                M2C_FIELD(D_8009D278, u8 *, 0x49) = 0U;
            } else {
                M2C_FIELD(D_8009D254, s32 *, 0x28) = (s32) (M2C_FIELD(D_8009D254, s32 *, 0x40) + (M2C_FIELD(D_8009D278, u8 *, 0x49) * rsin(M2C_FIELD(D_8009D278, s16 *, 0x4A), D_8009D254) * 0x10));
                M2C_FIELD(D_8009D254, s32 *, 0x30) = (s32) (M2C_FIELD(D_8009D254, s32 *, 0x48) + (M2C_FIELD(D_8009D278, u8 *, 0x49) * rcos(M2C_FIELD(D_8009D278, s16 *, 0x4A), D_8009D278) * 0x10));
                temp_v1_12 = M2C_FIELD(D_8009D278, u8 *, 0x49);
                M2C_FIELD(D_8009D278, u8 *, 0x49) = (u8) (temp_v1_12 - ((s32) temp_v1_12 / (s8) M2C_FIELD(D_8009D278, s8 *, 0x48)));
                M2C_FIELD(D_8009D278, s8 *, 0x48) = (s8) ((u8) M2C_FIELD(D_8009D278, s8 *, 0x48) - 1);
            }
        }
        Entity_UpdateTimers();
    }
    if (M2C_FIELD(D_8009D278, u8 *, 0x56) != 0) {
        Battle_DrawStatusPanel(0, D_8009D278 + 0x50);
        M2C_FIELD(D_8009D278, u8 *, 0x56) = (u8) (M2C_FIELD(D_8009D278, u8 *, 0x56) - 1);
    }
    {
    state0 = D_8009D278;
    temp_v0_8 = (s16) M2C_FIELD(state0, u16 *, 0xE);
    temp_v1_13 = M2C_FIELD(state0, s16 *, 0xC);
    if (temp_v0_8 < temp_v1_13) {
        register void *d254;

        d254 = D_8009D254;
        M2C_FIELD(state0, s16 *, 0x58) = (s16) (temp_v1_13 - temp_v0_8);
        {
        u16 copy210;

        copy210 = M2C_FIELD(d254, u16 *, 0x210);
        M2C_FIELD(D_8009D278, u16 *, 0x5A) = copy210;
        }
        {
        var_v0 = M2C_FIELD(d254, u16 *, 0x212);
        var_v1 = 0x1E;
        M2C_FIELD(D_8009D278, u8 *, 0x5E) = var_v1;
        { register void *post_state; post_state = D_8009D278;
        M2C_FIELD(state0, s16 *, 0x5C) = (s16) (var_v0 - 8);
        M2C_FIELD(post_state, s8 *, 0x5F) = 1; }
        }
        M2C_FIELD(D_8009D278, u16 *, 0xE) = (u16) M2C_FIELD(D_8009D278, s16 *, 0xC);
    }
    }
    temp_a1_2 = D_8009D278;
    if (M2C_FIELD(temp_a1_2, u8 *, 0x5E) != 0) {
        Battle_DrawStatusPanel(0, temp_a1_2 + 0x58);
        M2C_FIELD(D_8009D278, u8 *, 0x5E) = (u8) (M2C_FIELD(D_8009D278, u8 *, 0x5E) - 1);
        temp_a1_2 = D_8009D278;
    }
    if (M2C_FIELD(temp_a1_2, u8 *, 0x66) != 0) {
        Battle_DrawStatusPanel(0, temp_a1_2 + 0x60);
        M2C_FIELD(D_8009D278, u8 *, 0x66) = (u8) (M2C_FIELD(D_8009D278, u8 *, 0x66) - 1);
    }
    temp_a0_4 = M2C_FIELD(D_8009D278, s32 *, 8);
    M2C_FIELD(D_8009D278, u16 *, 0xE) = (u16) M2C_FIELD(D_8009D278, s16 *, 0xC);
    if (temp_a0_4 <= 0) {
        *temp_s2 |= 0x2000;
        if (Pad_GetMenuPressedBitOrDisabled(temp_a0_4) != D_8009D1D4[0]) {
            if (D_8009D200 != -1) {
                Pm_SendCmd(D_8009D200, 0, 0, 2, 0, 0);
                D_8009D200 = -1;
            }
            if (D_8009D2FC != -1) {
                Pm_SendCmd(D_8009D2FC, 0, 0, 2, 0, 0);
                D_8009D2FC = -1;
            }
        }
        Battle_FlushScriptSounds();
    }
    if (D_8009D28C == 1) {
        var_a2 = 0x46;
        var_a1 = 0x82;
        var_a0 = 0x9F;
        var_v0 = 0xFF;
        var_v1 = 0xF9;
        D_800B00ED[0] = var_a2;
        D_800B00FD[0] = var_a2;
        D_800B0111[0] = var_a2;
        D_800B0121[0] = var_a2;
        var_a2 = 0x36;
        D_800B00F4[0] = var_a0;
        D_800B0104[0] = var_a0;
        D_800B692C[0] = var_a0;
        D_800B0118[0] = var_a0;
        D_800B0128[0] = var_a0;
        D_800B6948[0] = var_a0;
        var_a0 = 0x4A;
        D_800B00F6[0] = var_v1;
        D_800B0106[0] = var_v1;
        D_800B692E[0] = var_v1;
        D_800B011A[0] = var_v1;
        D_800B012A[0] = var_v1;
        D_800B694A[0] = var_v1;
        var_v1 = 0x3B;
        D_800B00EC[0] = 0;
        D_800B00EE[0] = var_a1;
        D_800B00F5[0] = var_v0;
        D_800B00FC[0] = 0;
        D_800B00FE[0] = var_a1;
        D_800B0105[0] = var_v0;
        D_800B692D[0] = var_v0;
        D_800B0110[0] = 0;
        D_800B0112[0] = var_a1;
        D_800B0119[0] = var_v0;
        D_800B0120[0] = 0;
        D_800B0122[0] = var_a1;
        D_800B0129[0] = var_v0;
        D_800B6949[0] = var_v0;
        D_800B0134[0] = 0;
        D_800B0135[0] = var_a1;
        D_800B0136[0] = var_a2;
        D_800B013C[0] = var_a0;
        D_800B013D[0] = var_v0;
        D_800B013E[0] = var_v1;
        D_800B0144[0] = 0;
        D_800B0145[0] = var_a1;
        D_800B0146[0] = var_a2;
        D_800B014C[0] = var_a0;
        D_800B014D[0] = var_v0;
        D_800B014E[0] = var_v1;
        D_800B017C[0] = 0;
        D_800B017D[0] = var_a1;
        D_800B017E[0] = var_a2;
        D_800B0184[0] = var_a0;
        D_800B0185[0] = var_v0;
        D_800B0186[0] = var_v1;
        D_800B018C[0] = 0;
        D_800B018D[0] = var_a1;
        D_800B018E[0] = var_a2;
        D_800B0194[0] = var_a0;
        D_800B0195[0] = var_v0;
        D_800B0196[0] = var_v1;
    }
    if (M2C_FIELD(D_8009D278, s16 *, 0xC) <= 0) {
        var_a2 = 0x46;
        var_a1 = 0x82;
        var_a0 = 0x9F;
        var_v1 = 0xF9;
        var_a3 = 0x3D;
        {
        var_v0 = (u32) D_8009D254;
        M2C_FIELD((void *) var_v0, s32 *, 0x68) = 0;
        M2C_FIELD((void *) var_v0, s32 *, 0x6C) = 0;
        M2C_FIELD((void *) var_v0, s32 *, 0x70) = 0;
        }
        var_v0 = 0xFF;
        D_800B00ED[0] = var_a2;
        D_800B00FD[0] = var_a2;
        D_800B0111[0] = var_a2;
        D_800B0121[0] = var_a2;
        D_800B00F4[0] = var_a0;
        D_800B0104[0] = var_a0;
        D_800B692C[0] = var_a0;
        D_800B0118[0] = var_a0;
        D_800B0128[0] = var_a0;
        D_800B6948[0] = var_a0;
        D_800B00EC[0] = 0;
        D_800B00EE[0] = var_a1;
        D_800B00F5[0] = var_v0;
        D_800B00F6[0] = var_v1;
        D_800B00FC[0] = 0;
        D_800B00FE[0] = var_a1;
        D_800B0105[0] = var_v0;
        D_800B0106[0] = var_v1;
        D_800B692D[0] = var_v0;
        D_800B692E[0] = var_v1;
        D_800B0110[0] = 0;
        D_800B0112[0] = var_a1;
        D_800B0119[0] = var_v0;
        D_800B011A[0] = var_v1;
        D_800B0120[0] = 0;
        D_800B0122[0] = var_a1;
        D_800B0129[0] = var_v0;
        D_800B012A[0] = var_v1;
        D_800B6949[0] = var_v0;
        D_800B694A[0] = var_v1;
        {
        var_a2 = 0x36;
        var_a0 = 0x4A;
        var_v1 = 0x3B;
        D_800B0136[0] = var_a2;
        D_800B0146[0] = var_a2;
        D_800B017E[0] = var_a2;
        D_800B018E[0] = var_a2;
        var_a2 = 0x81;
        D_800B0135[0] = var_a1;
        D_800B0145[0] = var_a1;
        D_800B017D[0] = var_a1;
        D_800B018D[0] = var_a1;
        var_a1 = 0x83;
        D_800B013C[0] = var_a0;
        D_800B014C[0] = var_a0;
        D_800B0184[0] = var_a0;
        D_800B0194[0] = var_a0;
        var_a0 = 0x13;
        }
        D_800B013E[0] = var_v1;
        D_800B014E[0] = var_v1;
        D_800B0186[0] = var_v1;
        D_800B0196[0] = var_v1;
        var_v1 = 1;
        D_800B0134[0] = 0;
        D_800B013D[0] = var_v0;
        D_800B0144[0] = 0;
        D_800B014D[0] = var_v0;
        D_800B017C[0] = 0;
        D_800B0185[0] = var_v0;
        D_800B018C[0] = 0;
        D_800B0195[0] = var_v0;
        {
        D_800B0158[0] = var_v0;
        D_800B0159[0] = var_a3;
        D_800B015A[0] = var_a2;
        D_800B0160[0] = var_a1;
        D_800B0161[0] = var_a0;
        D_800B0162[0] = var_v1;
        D_800B0168[0] = var_v0;
        D_800B0169[0] = var_a3;
        D_800B016A[0] = var_a2;
        D_800B0170[0] = var_a1;
        D_800B0171[0] = var_a0;
        D_800B0172[0] = var_v1;
        D_800B01A0[0] = var_v0;
        D_800B01A1[0] = var_a3;
        D_800B01A2[0] = var_a2;
        D_800B01A8[0] = var_a1;
        D_800B01A9[0] = var_a0;
        D_800B01AA[0] = var_v1;
        D_800B01B0[0] = var_v0;
        D_800B01B1[0] = var_a3;
        D_800B01B2[0] = var_a2;
        D_800B01B8[0] = var_a1;
        D_800B01B9[0] = var_a0;
        D_800B01BA[0] = var_v1;
        }
        if (*temp_s2 & 0x10000) {
            Battle_ReturnToIdle();
        }
        Tbl_ResetAll();
        if (Pad_GetMenuPressedBitOrDisabled() != D_8009D1D4[0]) {
            if (D_8009D200 != -1) {
                Pm_SendCmd(D_8009D200, 0, 0, 2, 0, 0);
                D_8009D200 = -1;
            }
            if (D_8009D2FC != -1) {
                Pm_SendCmd(D_8009D2FC, 0, 0, 2, 0, 0);
                D_8009D2FC = -1;
            }
        }
        Battle_FlushScriptSounds();
        {
        register s32 tail_mask;
        register s32 tail_d1ac;
        register s32 tail_a0;

        tail_a0 = 0x46B;
        tail_mask = ~0x300;
        tail_d1ac = D_8009D1AC;
        tail_d1ac &= tail_mask;
        D_8009D1AC = tail_d1ac;
        {
            register s32 tail_a1;
            register s32 tail_a2;
            register s32 tail_a3;
            register s32 tail_sp;

            var_t0 = (s32) D_8009D254;
            tail_a2 = M2C_FIELD((void *) var_t0, s16 *, 0x2A);
            tail_a3 = M2C_FIELD((void *) var_t0, s16 *, 0x2E);
            D_8009D28C = 3;
            tail_sp = M2C_FIELD((void *) var_t0, s16 *, 0x32);
            tail_a1 = 0;
            D_8009D1CE[0] = 0;
            Asset_Find08Alt(tail_a0, tail_a1, tail_a2, tail_a3, tail_sp);
        }
        }
        D_8009D244[0] = 0;
        MenuWidget_InitPool();
        {
        register s32 minus5;

        if (D_800BCF88 & 0x2000) {
            Render_BeginSceneLoad();
            minus5 = -5;
        } else {
            minus5 = -5;
        }
        D_8009D1A0 &= minus5;
        D_8009D2E8 |= 1;
        Entity_SetActionMode(D_8009D254, 0x13U, minus5);
        }
        M2C_FIELD(D_8009D278, s16 *, 0xC) = 0;
    }
}
