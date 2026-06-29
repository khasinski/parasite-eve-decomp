/* CC1_PROFILE: build0001 */
/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef signed short s16;
typedef int s32;

typedef struct PackedRect3 {
    s16 x;
    s16 y;
    s16 z;
} PackedRect3;

extern s32 Akao_Calc3DPan(PackedRect3 *rect, s32 *out1, s32 *out2);
extern s32 Akao_SendTableCommand(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);

s32 Akao_SendPositionalCmd(void *arg0, s32 arg1, s32 arg2, s16 arg3, s16 arg4, s16 arg5) {
    PackedRect3 rect;
    s32 out1;
    s32 out2;

    rect.x = arg3;
    rect.y = arg4;
    rect.z = arg5;

    Akao_Calc3DPan(&rect, &out1, &out2);
    return Akao_SendTableCommand(arg0, arg1, arg2, out1, out2);
}
