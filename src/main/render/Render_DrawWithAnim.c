/* CC1_FLAGS: -g3 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

M2C_UNK Anim_BuildRotationMatrices();
M2C_UNK Render_TransformVertices();
M2C_UNK Render_DrawObject();
M2C_UNK Render_UpdateClutTable();
M2C_UNK Render_CopyMatrixBlock();

extern struct { char _[16]; } D_80091A38_o __asm__("g_IdentityMatrixSource");
#define g_IdentityMatrixSource (*(M2C_UNK *)&D_80091A38_o)
extern struct { char _[16]; } D_8009CDDC_lh_o __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_lw0_o __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_sw0_o __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_lw1_o __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_sw1_o __asm__("g_ActiveDrawSlot");
#define D_8009CDDC_lh (*(s16 *)&D_8009CDDC_lh_o)
#define D_8009CDDC_lw0 (*(s32 *)&D_8009CDDC_lw0_o)
#define D_8009CDDC_sw0 (*(s32 *)&D_8009CDDC_sw0_o)
#define D_8009CDDC_lw1 (*(s32 *)&D_8009CDDC_lw1_o)
#define D_8009CDDC_sw1 (*(s32 *)&D_8009CDDC_sw1_o)
extern struct { char _[16]; } D_800B1638_o __asm__("g_RenderClutLookupTable");
#define g_RenderClutLookupTable (*(M2C_UNK *)&D_800B1638_o)

void Render_DrawWithAnim(void *arg0, s32 arg1, s16 arg2, M2C_UNK arg3) {
    M2C_UNK *temp_s0;
    s32 temp_v0;

    if (arg1 != 0) {
        M2C_FIELD(arg0, s32 *, 0xB0) = arg1;
        temp_s0 = arg0 + 0x34;
        Render_CopyMatrixBlock(temp_s0, &g_RenderClutLookupTable, 1);
        Render_CopyMatrixBlock(&g_IdentityMatrixSource, temp_s0, 1);
        Anim_BuildRotationMatrices(arg0, arg1, arg2, 0);
    }
    Render_TransformVertices(arg0);
    Render_CopyMatrixBlock(&g_RenderClutLookupTable, arg0 + 0x34, 1);
    Render_DrawObject(arg0, arg3);
    Render_UpdateClutTable(arg0, 1, D_8009CDDC_lh);
    temp_v0 = D_8009CDDC_lw0 ^ 1;
    D_8009CDDC_sw0 = temp_v0;
    Render_UpdateClutTable(arg0, 1, (s16) temp_v0);
    D_8009CDDC_sw1 = D_8009CDDC_lw1 ^ 1;
}
