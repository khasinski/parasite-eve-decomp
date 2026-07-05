/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuItemActionContext;
int g_MenuActiveItemSlot;
int g_MenuItemActionDisabled;
int g_MenuEquipMode;
extern int g_MenuItemActionStateTable[][3];

void *Inv_LookupActiveListData(int index);
int Inv_TestSelectionBit();
int Inv_IsSlotSelectable(int arg0);
void *Str_LookupTable4(unsigned int arg0);
void Draw_SetTextDimmed(int value);
void MenuWidget_DrawCenteredText(int arg0);

void Sfx_ContextPick(int arg0)
{
    int state;
    int flag;
    int sound;
    int raw;
    int one;
    unsigned char *entry;

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
        if (g_MenuEquipMode != one || entry[6] != 0xA || entry[0xE] < 4) {
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
