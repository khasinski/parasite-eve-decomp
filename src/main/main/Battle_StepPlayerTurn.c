/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned int u32;

extern struct { char _[16]; } D_8009D20C_o __asm__("g_FieldActorListHead");
extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
extern struct { char _[16]; } D_8009D278_o __asm__("D_8009D278");

#define g_FieldActorListHead (*(u8 **)&D_8009D20C_o)
#define D_8009D254 (*(u8 **)&D_8009D254_o)
#define D_8009D278 (*(u8 **)&D_8009D278_o)
#define TARGET_ENTRY(list, index) ((u8 *)(list) + ((u8)(index) * 12))
#define TARGET_ENTITY(list, index) (*(u8 **)TARGET_ENTRY((list), (index)))
#define TARGET_ANGLE(list, index) (*(s16 *)(TARGET_ENTRY((list), (index)) + 8))
#define U8_AT(ptr, offset) (*(u8 *)((u8 *)(ptr) + (offset)))
#define S8_AT(ptr, offset) (*(s8 *)((u8 *)(ptr) + (offset)))
#define U32_AT(ptr, offset) (*(u32 *)((u8 *)(ptr) + (offset)))
#define FADE_PTR(entity) ((u8 *)(entity) + 0x1B4)

extern s8 D_8009D2B0;
extern u8 D_8009CE68;
extern s8 D_8009CE6C;

void Render_FadeEntityColor(void *entity_render, int r, int g, int b);

void Battle_StepPlayerTurn(void *target_list, int target_index, int step) {
    int alpha;
    int mode;
    int i;
    int angle;
    int low;
    int high;
    int selected_angle;
    u8 *entity;
    u8 *core;
    u8 *iter;
    u8 *action;

    if (D_8009D2B0 == 0) {
        return;
    }

    if (D_8009CE68 < 0x41) {
        D_8009CE6C = 8;
    } else if (D_8009CE68 >= 0xC0) {
        D_8009CE6C = -8;
    }

    alpha = D_8009CE68 + D_8009CE6C;
    D_8009CE68 = alpha;

    if ((s8)step < 4) {
        action = *(u8 **)(D_8009D278 + 0x68);
        mode = (U32_AT(action, 0x10) >> 6) & 3;

        if (mode == 0) {
            entity = TARGET_ENTITY(target_list, (s8)target_index);
            core = *(u8 **)entity;

            if (S8_AT(core, 5) == 1) {
                Render_FadeEntityColor(FADE_PTR(*(u8 **)(entity + 0x18C)), (u8)alpha, (u8)alpha, (u8)alpha);
            } else if (S8_AT(core, 5) == 4) {
                for (iter = g_FieldActorListHead; iter != 0; iter = *(u8 **)(iter + 4)) {
                    core = *(u8 **)iter;
                    if ((iter != D_8009D254) && (core != 0) && (S8_AT(core, 5) == 4)) {
                        Render_FadeEntityColor(FADE_PTR(iter), D_8009CE68, D_8009CE68, D_8009CE68);
                    }
                }
            } else {
                Render_FadeEntityColor(FADE_PTR(entity), (u8)alpha, (u8)alpha, (u8)alpha);
            }
            return;
        }

        if (mode == 2) {
            selected_angle = TARGET_ANGLE(target_list, (s8)target_index);
            if (selected_angle < -0x600) {
                low = selected_angle + 0x200;
                high = selected_angle + 0xE00;
            } else if (selected_angle < 0x600) {
                low = selected_angle - 0x200;
                high = selected_angle + 0x200;
            } else {
                low = selected_angle - 0xE00;
                high = selected_angle - 0x200;
            }

            for (i = 0; TARGET_ENTITY(target_list, i) != 0; i++) {
                angle = TARGET_ANGLE(target_list, i);
                if (selected_angle >= -0x600 && selected_angle < 0x600) {
                    if (angle < low || high < angle) {
                        continue;
                    }
                } else if (!(angle < high || low < angle)) {
                    continue;
                }

                entity = TARGET_ENTITY(target_list, i);
                core = *(u8 **)entity;
                if (S8_AT(core, 5) == 1) {
                    Render_FadeEntityColor(FADE_PTR(*(u8 **)(entity + 0x18C)), D_8009CE68, D_8009CE68,
                                           D_8009CE68);
                } else if (S8_AT(core, 5) == 4) {
                    for (iter = g_FieldActorListHead; iter != 0; iter = *(u8 **)(iter + 4)) {
                        core = *(u8 **)iter;
                        if ((iter != D_8009D254) && (core != 0) && (S8_AT(core, 5) == 4)) {
                            Render_FadeEntityColor(FADE_PTR(iter), D_8009CE68, D_8009CE68, D_8009CE68);
                        }
                    }
                } else {
                    Render_FadeEntityColor(FADE_PTR(entity), D_8009CE68, D_8009CE68, D_8009CE68);
                }
            }
            return;
        }

        if ((mode == 1) || (mode == 3)) {
            for (i = 0; TARGET_ENTITY(target_list, i) != 0; i++) {
                entity = TARGET_ENTITY(target_list, i);
                core = *(u8 **)entity;
                if (S8_AT(core, 5) == 1) {
                    Render_FadeEntityColor(FADE_PTR(*(u8 **)(entity + 0x18C)), D_8009CE68, D_8009CE68,
                                           D_8009CE68);
                } else if (S8_AT(core, 5) == 4) {
                    for (iter = g_FieldActorListHead; iter != 0; iter = *(u8 **)(iter + 4)) {
                        core = *(u8 **)iter;
                        if ((iter != D_8009D254) && (core != 0) && (S8_AT(core, 5) == 4)) {
                            Render_FadeEntityColor(FADE_PTR(iter), D_8009CE68, D_8009CE68, D_8009CE68);
                        }
                    }
                } else {
                    Render_FadeEntityColor(FADE_PTR(entity), D_8009CE68, D_8009CE68, D_8009CE68);
                }
            }
            return;
        }
    } else if ((s8)step < 8) {
        entity = TARGET_ENTITY(target_list, (s8)target_index);
        core = *(u8 **)entity;
        if (S8_AT(core, 5) == 1) {
            Render_FadeEntityColor(FADE_PTR(*(u8 **)(entity + 0x18C)), (u8)alpha, (u8)alpha, (u8)alpha);
        } else if (S8_AT(core, 5) == 4) {
            for (iter = g_FieldActorListHead; iter != 0; iter = *(u8 **)(iter + 4)) {
                core = *(u8 **)iter;
                if ((iter != D_8009D254) && (core != 0) && (S8_AT(core, 5) == 4)) {
                    Render_FadeEntityColor(FADE_PTR(iter), D_8009CE68, D_8009CE68, D_8009CE68);
                }
            }
        } else {
            Render_FadeEntityColor(FADE_PTR(entity), (u8)alpha, (u8)alpha, (u8)alpha);
        }
    }
}
