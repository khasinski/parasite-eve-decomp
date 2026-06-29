/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;

s32 MenuWidget_GetChild(s32 node, s32 index) __asm__("func_80062A20");
s32 MenuWidget_GetCellIndex(s32 node);
s32 MenuWidget_FindByModeAndSelectedBase(s32 mode, s32 base);
void MenuWidget_DestroyNode(s32 node) __asm__("func_80062F1C");
void Menu_CreateBonusPointAllocationView(void);
void Menu_StepItemDetailPanel2(void);
void Menu_PlayConfirmSound(void) __asm__("func_800525EC");
void Menu_PlayCancelSound(void) __asm__("func_80052634");
void Menu_PlayErrorSound(void) __asm__("func_800526C4");

asm(".globl func_80046B58");
asm("func_80046B58 = Menu_SkillActionHandler");

s32 Menu_SkillActionHandler(s32 node, s32 flags) {
    s32 child;
    s32 handled;

    handled = 0;
    child = MenuWidget_GetChild(node, 0);
    if (flags & 0x10000) {
        if (MenuWidget_GetCellIndex(child) >= 0) {
            handled = 1;
            Menu_StepItemDetailPanel2();
            Menu_PlayConfirmSound();
        } else {
            handled = 1;
            Menu_PlayErrorSound();
        }
    } else if (flags & 0x40) {
        MenuWidget_DestroyNode(node);
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x1D));
        Menu_CreateBonusPointAllocationView();
        handled = 1;
        Menu_PlayCancelSound();
    }
    return handled;
}
