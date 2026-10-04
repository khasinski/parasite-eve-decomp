#ifndef PE1_MEMCARD_SAVE_STATE_H
#define PE1_MEMCARD_SAVE_STATE_H

/* Declarations used only by the memory card save state machine
 * (MemCard_UpdateSaveState). */
#include "pe1/memcard.h"
#include "pe1/menu_inventory.h"
#include "pe1/psyq_bios.h"
#include "pe1/psyq_libc.h"

/* PSY-Q struct DIRENTRY as filled by firstfile/nextfile. */
typedef struct MemCardDirEntry {
    char name[20];
    s32 attr;
    s32 size;
    struct MemCardDirEntry *next;
    s32 head;
    char system[4];
} MemCardDirEntry;

/* SDK firstfile wrapper (FIRST.OBJ); returns the entry or 0. */
void *Scene_CreateEntityNode(const char *name, void *entry);
void *nextfile(void *entry);
int format(char *device);
int lseek(int descriptor, int offset, int origin);
int read(int descriptor, void *buffer, int count);
int write(int descriptor, void *buffer, int count);
int strncmp(const char *left, const char *right, unsigned int count);

unsigned int MemCard_GetDialogMode(void);
int Menu_CreateSaveSlotListView(int port, int count);
void Menu_ShowMemCardErrorDialog(int port);
void Menu_DestroyMemCardProgressWidget(void);
void Menu_CreateTwoLineDialog(int first, int second);
void Save_LoadCardFileIntoRuntime(void);

/* Card directory path "bu00:" whose port digit is patched before scans. */
extern char *D_80092230;
/* Format device name "bu%ld0:". */
extern s8 D_80010F60[];
/* Save-file image written by the save flow. */
extern u8 D_8009EED0[];
extern int D_800A1704;
extern int D_800A186C;
extern int D_800A1864;
extern MemCardPortState *D_800A1854;

void MemCard_UpdateSaveState(int port);

#endif
