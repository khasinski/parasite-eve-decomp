/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory.h"
#include "pe1/draw_state.h"
#include "pe1/text.h"

int g_MenuItemActionContext;
int g_MenuActiveItemSlot;
int g_MenuItemActionDisabled;
int g_MenuEquipMode;
extern int g_MenuItemActionStateTable[][3];

int Inv_IsSlotSelectable(int arg0);
void Draw_SetTextDimmed(int value);

void Sfx_ContextPick(int arg0)
{
    int state;
    int flag;
    int sound;
    int raw;
    int one;
    ItemDataRecord *entry;

    state = g_MenuItemActionStateTable[g_MenuItemActionContext][arg0];
    one = 1;

    if (state != one) {
        if (state < 2) {
            flag = 0;
            if (state == 0) {
                goto case_0;
            }
        } else if (state == 2) {
            goto case_2;
        } else if (state == 3) {
            goto case_3;
        }
    }
    goto tail;

case_0:
    raw = g_MenuActiveItemSlot;
    entry = Inv_LookupActiveListData(raw);
    if (Inv_TestSelectionBit(raw) != 0) {
        if (g_MenuEquipMode != one || entry->kind != 0xA || *(u8 *)&entry->bonusStats[0] < 4) {
            flag = 1;
        }
    }
    sound = flag ^ 1;
    goto call_sound;

case_2:
    sound = Inv_IsSlotSelectable(g_MenuActiveItemSlot) == 0;
    goto call_sound;

case_3:
    sound = g_MenuItemActionDisabled;

call_sound:
    Draw_SetTextDimmed(sound);

tail:
    MenuWidget_DrawCenteredText(Str_LookupTable4(state));
}
