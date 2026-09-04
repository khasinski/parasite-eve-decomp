#include "../room_lib/room_lib.h"

typedef struct Room118Stack {
    RoomFxSeed8 seed;
    RoomSpriteMatrix base_matrix;
    RoomSpriteMatrix matrix;
    RoomFxVec4 transformed_scale;
    RoomFxVec4 scale;
} Room118Stack;

extern RoomFxSeed8 D_8018EFF4;
extern unsigned char D_80190FB4[];

void func_80071A44(RoomFxVec4 *arg0, s32 arg1, s32 arg2);
void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
void func_800C3134(void *arg0, s32 arg1, void *arg2);
void func_800C4FC4(RoomFxEmitterParams *params, RoomSpriteMatrix *matrix,
                   s32 arg2);

void func_8018F514(void *arg0, char *arg1, RoomFxPairedEmitterState *state) {
    Room118Stack stack;
    register char *arg1_reg asm("$21");
    register RoomFxPairedEmitterState *state_reg asm("$18");
    RoomSpriteMatrix *base_matrix;
    register RoomSpriteMatrix *matrix asm("$19");
    RoomClock *owner;
    unsigned char *table;
    register s32 gte_word0 asm("$12");
    register s32 gte_word1 asm("$13");
    register s32 gte_word2 asm("$14");

    arg1_reg = arg1;
    state_reg = state;
    owner = func_800C2B50();
    stack.seed = D_8018EFF4;
    stack.base_matrix.m[2][2] = 0x1000;
    stack.base_matrix.m[1][1] = 0x1000;
    stack.base_matrix.m[0][0] = 0x1000;
    stack.base_matrix.t[2] = 0;
    stack.base_matrix.t[1] = 0;
    stack.base_matrix.t[0] = 0;
    stack.base_matrix.m[2][1] = 0;
    stack.base_matrix.m[2][0] = 0;
    stack.base_matrix.m[1][2] = 0;
    stack.base_matrix.m[1][0] = 0;
    stack.base_matrix.m[0][2] = 0;
    stack.base_matrix.m[0][1] = 0;

    func_80071A44(&stack.scale, 0, 0x10);
    stack.scale.x = state_reg->timer * 2;
    base_matrix = &stack.base_matrix;
    stack.scale.y = state_reg->timer * 2;
    stack.scale.z = state_reg->timer * 2;
    stack.transformed_scale = stack.scale;
    func_80078CC4(base_matrix, &stack.transformed_scale);
    {
        RoomFxSeed8 *seed_ptr;

        seed_ptr = &stack.seed;
        PE1_COMPILER_USE(seed_ptr);
        matrix = &stack.matrix;
        func_800794C4(seed_ptr, matrix);
    }

    {
        register RoomSpriteMatrix *base_opaque asm("$16");
        register RoomSpriteMatrix *matrix_opaque asm("$19");

        base_opaque = base_matrix;
        matrix_opaque = matrix;
        PE1_COMPILER_LAUNDER2(base_opaque, matrix_opaque);
        gte_word0 = *(s32 *)((char *)base_opaque + 0);
        gte_word1 = *(s32 *)((char *)base_opaque + 4);
        gte_ctc2_0(gte_word0);
        gte_ctc2_1(gte_word1);
        gte_word0 = *(s32 *)((char *)base_opaque + 8);
        gte_word1 = *(s32 *)((char *)base_opaque + 0xC);
        gte_word2 = *(s32 *)((char *)base_opaque + 0x10);
        gte_ctc2_2(gte_word0);
        gte_ctc2_3(gte_word1);
        gte_ctc2_4(gte_word2);

        gte_word0 = *(u16 *)((char *)matrix_opaque + 0);
        gte_word1 = *(u16 *)((char *)matrix_opaque + 6);
        gte_word2 = *(u16 *)((char *)matrix_opaque + 0xC);
        gte_mtc2_9(gte_word0);
        gte_mtc2_10(gte_word1);
        gte_mtc2_11(gte_word2);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_light_ir_sf0();
        gte_mfc2_9(gte_word0);
        gte_mfc2_10(gte_word1);
        gte_mfc2_11(gte_word2);
        *(volatile s16 *)((char *)matrix_opaque + 0) = gte_word0;
        *(volatile s16 *)((char *)matrix_opaque + 6) = gte_word1;
        *(volatile s16 *)((char *)matrix_opaque + 0xC) = gte_word2;
        PE1_COMPILER_MEMORY_BARRIER();

        {
            register u16 *column asm("$2");

            column = (u16 *)((char *)&stack + 0x2A);
            PE1_COMPILER_LAUNDER_MEM(column);
            gte_word0 = column[0];
            gte_word1 = column[3];
            gte_word2 = column[6];
            gte_mtc2_9(gte_word0);
            gte_mtc2_10(gte_word1);
            gte_mtc2_11(gte_word2);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_mvmva_light_ir_sf0();
            gte_mfc2_9(gte_word0);
            gte_mfc2_10(gte_word1);
            gte_mfc2_11(gte_word2);
            column[0] = gte_word0;
            column[3] = gte_word1;
            column[6] = gte_word2;
        }

        {
            register u16 *column asm("$2");

            column = (u16 *)((char *)&stack + 0x2C);
            PE1_COMPILER_LAUNDER_MEM(column);
            gte_word0 = column[0];
            gte_word1 = column[3];
            gte_word2 = column[6];
            gte_mtc2_9(gte_word0);
            gte_mtc2_10(gte_word1);
            gte_mtc2_11(gte_word2);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_mvmva_light_ir_sf0();
            gte_mfc2_9(gte_word0);
            gte_mfc2_10(gte_word1);
            gte_mfc2_11(gte_word2);
            column[0] = gte_word0;
            column[3] = gte_word1;
            column[6] = gte_word2;
        }

        gte_word0 = *(s32 *)((char *)base_opaque + 0x14);
        gte_word1 = *(s32 *)((char *)base_opaque + 0x18);
        gte_ctc2_5(gte_word0);
        gte_word2 = *(s32 *)((char *)base_opaque + 0x1C);
        gte_ctc2_6(gte_word1);
        gte_ctc2_7(gte_word2);

        {
            u16 *translation;

            translation = (u16 *)((char *)&stack + 0x3C);
            PE1_COMPILER_LAUNDER_MEM(translation);
            gte_word1 = translation[2];
            gte_word0 = translation[0];
            gte_word1 <<= 16;
            gte_word0 |= gte_word1;
            gte_mtc2_0(gte_word0);
            gte_lwc2_1_8(translation);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtv0tr_sf0();
            gte_swc2_9_0(translation);
            gte_swc2_10_4(translation);
            gte_swc2_11_8(translation);
        }
    }

    func_800C3238(2);

    table = D_80190FB4;
    {
        register RoomLink *link asm("$16");

        link = (*(RoomLink **)owner)->p238;
        func_800C3134(table, *(s16 *)(arg1_reg + 2), &state_reg->primary.color0);
        func_800C3134(table, *(s16 *)(arg1_reg + 2),
                      &state_reg->secondary.color0);
        state_reg->primary.intensity = state_reg->intensity;

        stack.matrix.t[0] = RW32(link, 0x14);
        stack.matrix.t[1] = RW32(link, 0x18) - 0x64;
        stack.matrix.t[2] = RW32(link, 0x1C);
        func_800C4FC4(&state_reg->primary, matrix, 0);

        {
            RoomFxEmitterParams *secondary;

            secondary = &state_reg->secondary;
            secondary->intensity = state_reg->intensity;
            PE1_COMPILER_MEMORY_BARRIER();
            {
                s32 x;
                s32 floor_y;

                x = RW32(link, 0x14);
                floor_y = *(s16 *)&g_FrameCount16;
                stack.matrix.t[0] = x;
                stack.matrix.t[1] = floor_y;
            }
            stack.matrix.t[2] = RW32(link, 0x1C);
            func_800C4FC4(secondary, matrix, 0);
        }
    }
}
