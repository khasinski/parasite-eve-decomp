typedef unsigned char u8;
typedef unsigned int u32;

#include "pe1/save_blob.h"

extern SaveBytes12E4 g_MemCardSaveStateBuffer;
extern SaveBytes12E4 g_SaveRuntimeState;
extern u8 g_MemCardFileBuffer[];
extern u8 *g_SaveIoCursor;
extern int g_MemCardLoadSucceeded;

void Save_DeserializeTail(void);
void Save_RestoreHeader(void);
void Menu_DestroyMemCardProgressWidget(void);
void Menu_CreateNotificationDialog(int arg0, int arg1);
void Menu_SetDeferredCallback(void (*callback)(void));
void Menu_CreateTwoLineDialog(int arg0, int arg1);
void Save_CancelUiFlow(void);

void Save_LoadCardFileIntoRuntime(void) {
    SaveBytes4 expected_crc;
    u8 *data;
    u32 crc;
    u32 i;
    u32 bit;
    u8 *cursor;

    g_SaveIoCursor = (u8 *)&g_MemCardSaveStateBuffer;
    g_SaveRuntimeState = g_MemCardSaveStateBuffer;
    g_SaveIoCursor += 0x12E4;
    Save_DeserializeTail();

    data = g_MemCardFileBuffer;
    crc = 0xFFFF;
    i = 0;
    cursor = g_SaveIoCursor;
    expected_crc = *(SaveBytes4 *)cursor;
    g_SaveIoCursor += 4;
    *(u32 *)cursor = 0;

    do {
        crc ^= data[i & 0xFFFF] << 8;
        bit = 0;
        do {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
            bit++;
        } while ((bit & 0xFFFF) < 8);
        i++;
    } while ((i & 0xFFFF) < 0x2000);

    if ((~crc & 0xFFFF) == *(u32 *)expected_crc.b) {
        Save_RestoreHeader();
        g_MemCardLoadSucceeded = 1;
        Menu_DestroyMemCardProgressWidget();
        Menu_CreateNotificationDialog(0x54, 0);
        Menu_SetDeferredCallback(Save_CancelUiFlow);
    } else {
        Menu_DestroyMemCardProgressWidget();
        Menu_CreateTwoLineDialog(0x55, 0x56);
    }
}
