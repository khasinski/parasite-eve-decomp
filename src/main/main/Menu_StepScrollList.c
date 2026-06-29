/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef int s32;

extern s32 D_8009CFE4;

void Draw_SetBaseOffsetPosition(s32 x, s32 y) __asm__("func_8005E850");
s32 Draw_GetBaseY(void) __asm__("func_8005E884");
void Menu_PlayMoveSound(void) __asm__("func_8005267C");
void Menu_PlayConfirmSound(void) __asm__("func_800525EC");
void Menu_PlayCancelSound(void) __asm__("func_80052634");
void MenuWidget_DestroyNode(void *node) __asm__("func_80062F1C");

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
