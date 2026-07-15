/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int D_8009CFE4;

void Draw_SetBaseOffsetPosition(int x, int y);
int Draw_GetBaseY(void);
void Menu_PlayMoveSound(void);
void Menu_PlayConfirmSound(void);
void Menu_PlayCancelSound(void);
void MenuWidget_DestroyNode(void *node);

int func_8004B650(void *node, int input) {
    int zero_arg;

    if ((input & 0x1000) != 0) {
        zero_arg = 0;
        input = -1;
        goto move;
    }

    if ((input & 0x4000) != 0) {
        zero_arg = 0;
        input = 1;
move:
        Draw_SetBaseOffsetPosition(zero_arg, input);
        Menu_PlayMoveSound();
        return 1;
    }

    if ((input & 0x10000) != 0) {
        MenuWidget_DestroyNode(node);
        Menu_PlayConfirmSound();
        return 1;
    }

    if ((input & 0x40) != 0) {
        Draw_SetBaseOffsetPosition(0, D_8009CFE4 - Draw_GetBaseY());
        MenuWidget_DestroyNode(node);
        Menu_PlayCancelSound();
    }

    return 1;
}
