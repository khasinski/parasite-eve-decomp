#ifndef PE1_SAVE_H
#define PE1_SAVE_H

#include "common.h"
#include "pe1/inventory.h"
#include "pe1/save_blob.h"

PE1_STATIC_ASSERT(sizeof(SaveMetadataWindow) == 0x10,
                  save_metadata_window_size);

PE1_STATIC_ASSERT(sizeof(SaveBytes12E4) == 0x12E4, save_runtime_blob_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SaveBytes12E4, metadata[1]) == 0x10,
                  save_second_metadata_window_offset);
extern SaveBytes12E4 g_SaveRuntimeState;
void Save_InitMetadataState(void);

/* Save-slot metadata window selected by the current frontend operation. */
extern int g_SaveMetadataWindowIndex;
extern u8 *g_SaveMetadataCursor;
extern int g_SaveMetadataWindowLength;
extern ItemDataRecord *g_SaveMetadataSourceRecord;
extern ItemDataRecord *g_MenuRenameTargetRecord;
extern SaveMetadataWindow g_SaveRuntimeMetadataWindows[2]
    __asm__("g_SaveRuntimeState");
extern SaveMetadataWindow g_CursorRenderMetadataWindows[2]
    __asm__("g_CursorRenderDataBlock");

int Save_GetMetadataWindowIndex(void);
void Save_AppendMetadataChar(unsigned char ch);
int Save_DeleteMetadataChar(void);
char *Save_GetActiveMetadataBuffer(void);
u8 *Save_GetMetadataCursor(void);
u32 Save_GetMetadataRemainingChars(void);
void Save_SelectMetadataWindow(ItemDataRecord *record);
void Save_LoadMetadataWindowText(void);

/* Save-notification state and screen-layout selector. These globals are
 * shared with older frontend/battle callers under their retail names. */
extern unsigned char D_8009D1CE;
extern unsigned char D_8009CE80;
extern unsigned char D_8009CE88;
extern void *D_8009D1F8;
void Menu_SaveOverlayDraw(void);

/* Leading values of the four runtime timer records reset at new game. */
extern unsigned int D_800A76A4[], D_800A76B0[], D_800A76BC[], D_800A76C8[];
extern unsigned char D_800C20A4[], D_800C20B4[];

#endif /* PE1_SAVE_H */
