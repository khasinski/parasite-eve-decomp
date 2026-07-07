/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */


#include "include_asm.h"
typedef int s32;

extern s32 D_8009CFE4;

void Draw_SetBaseOffsetPosition(s32 x, s32 y);
s32 Draw_GetBaseY(void);
void Menu_PlayMoveSound(void);
void Menu_PlayConfirmSound(void);
void Menu_PlayCancelSound(void);
void MenuWidget_DestroyNode(void *node);

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
void __maspsx_include_asm_hack_Aya_SetTotalExp(void);

s32 Menu_StepScrollList(void *node, s32 input)
{
    s32 zero_arg;

    if (input & 0x1000) {
        zero_arg = 0;
        input = -1;
        goto move;
    }

    if (input & 0x4000) {
        zero_arg = 0;
        input = 1;
move:
        Draw_SetBaseOffsetPosition(zero_arg, input);
        Menu_PlayMoveSound();
        return 1;
    }

    if (input & 0x10000) {
        MenuWidget_DestroyNode(node);
        Menu_PlayConfirmSound();
        return 1;
    }

    if (input & 0x40) {
        Draw_SetBaseOffsetPosition(0, D_8009CFE4 - Draw_GetBaseY());
        MenuWidget_DestroyNode(node);
        Menu_PlayCancelSound();
    }

    return 1;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/aya/misc6", Aya_SetTotalExp);
