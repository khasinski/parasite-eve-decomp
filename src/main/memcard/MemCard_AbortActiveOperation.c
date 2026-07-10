#include "pe1/card_obj.h"

typedef unsigned char u8;

typedef struct MemCardStateRaw {
    u8 field_00;
    u8 field_01;
    u8 pad_02;
    u8 slot_index;
    u8 field_04;
    u8 pad_05[7];
    int fd;
    u8 pad_10[0x408];
} MemCardStateRaw;

extern int D_800A185C;
extern void *D_800A1854;
extern void *D_800A1838;
extern MemCardStateRaw D_800A0ED4[];
extern char D_8009EE70[];
extern char D_80010F4C[];
extern char *D_80092224;

void Save_CancelUiFlow(void);
void Menu_CloseSaveSlotListView(int slot);
void MenuWidget_RestoreSavedCurrentNode(void);
int Square_Vsprintf();
int open(char *path, int mode);
int close(int fd);
int erase(char *path);

void MemCard_AbortActiveOperation(MemCardStateRaw *state) {
    register MemCardStateRaw *state_reg asm("$18");
    register int state_index asm("$16");
    register int retry asm("$17");
    register int minus_one asm("$19");
    int fd;
    int slot;
    u8 *slot_entry;
    char path[0x20];

    state_reg = state;

    if (D_800A185C != 0) {
        Save_CancelUiFlow();
        return;
    }

    if (state_reg->fd >= 0) {
        close(state_reg->fd);
        state_reg->fd = -1;
    }

    if (state_reg->field_01 == 9) {
        state_index = state_reg - D_800A0ED4;
        retry = 0;
        minus_one = -1;
        asm volatile("" : : "r"(minus_one));
        slot = state_reg->slot_index;
        slot_entry = (u8 *)state_reg + ((slot << 4) + slot) * 4;

        Square_Vsprintf(D_8009EE70, D_80092224, state_reg > D_800A0ED4,
                        slot_entry[0x45] + 0x30, slot + 0x41);
        Square_Vsprintf(path, D_80010F4C, state_index, D_8009EE70);

        do {
            fd = open(path, 1);
            if (fd != minus_one) {
                break;
            }
            retry++;
        } while (retry < 10);

        if (retry < 10) {
            close(fd);
            erase(path);
        }
    }

    state_index = state_reg - D_800A0ED4;
    Menu_CloseSaveSlotListView(state_index);
    state_reg->field_01 = 0;
    state_reg->field_04 = 0;
    D_800A1854 = 0;
    D_800A1838 = 0;
    MenuWidget_RestoreSavedCurrentNode();
}
