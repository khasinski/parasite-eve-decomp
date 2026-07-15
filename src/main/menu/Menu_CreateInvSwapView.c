/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "include_asm.h"

typedef unsigned char u8;
typedef unsigned int u32;

void *MenuWidget_CreateSimpleNode(void *state, int parent, int arg2, int arg3);
void *MenuWidget_CreateNode(void *state, void *parent, void *selected);
void MenuWidget_SetCurrentNode(void *node);
void *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
void MenuWidget_SetCursorY(void *node);
void Menu_EquipOptionsInputHandler(void);
void Menu_DrawEquipOptionsList(void);

extern u32 D_8009CF0C;
extern u32 D_8009CFB8;

void Menu_CreateInvSwapView(int parent, u8 *state) {
    register u8 *state_reg asm("$17");
    register void *node asm("$16");
    void *current;

    state_reg = state;
    node = MenuWidget_CreateSimpleNode(state_reg + 0x3A, parent, 0, 0);
    current = MenuWidget_CreateNode(state_reg + 0x3A, node, node);
    ((void (**)(void))node)[0xB] = Menu_EquipOptionsInputHandler;
    ((void (**)(void))current)[0xC] = Menu_DrawEquipOptionsList;
    MenuWidget_SetCurrentNode(current);
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x33));

    if (state_reg != 0) {
        u32 value;

        value = D_8009CF0C;
        /* Match note: preserve the load-delay nop before storing the cached value. */
        asm volatile("nop");
        D_8009CFB8 = value;
    } else {
        register u32 zero asm("$0");

        D_8009CFB8 = zero;
    }
}
