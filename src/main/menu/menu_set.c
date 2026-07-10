/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

signed char D_8009CE94;

short D_8009CE98;
short D_8009CE9A;
short D_8009CE9C;
short D_8009CE9E;

extern char D_8009EC70[];
extern char D_8009ECA8[];
extern char D_80091680[];
extern char D_800BCEA8[];
extern char D_8009CEA0;
extern char D_8009CED0;
extern int D_8009CED4;
extern void *D_8009CE90;

void SetDrawMode(void *packet, int draw_texture, int dither, int tpage);
void SetSprt(u8 *prim);
void SetTile(u8 *prim);
int Gpu_AppendPacketPrimitive(void *packet, void *prim);
void Gpu_SetDrawEnable(u8 *packet, int enabled);
void Gpu_SetDither(u8 *packet, int enabled);
u16 GetTPage(int tp, int abr, int x, int y);
void exit(int code);

void Menu_SetEquipSlotIndex(int arg0) {
    D_8009CE94 = arg0;
}

void Render_SetupFogLayer(void *arg0) {
    u8 i;
    char *entry;
    u32 code;
    char *packet;
    char *prim;
    char *tile_packet;
    int offset;
    u16 tpage;

    for (i = 0; i < 4; i++) {
        entry = D_800BCEA8 + (i * 0x38);
        entry[0x0C] = 0;
        entry[0x0D] = 0;
        code = *(u32 *)(entry + 0x0C);
        entry[0x00] = 0;
        *(u32 *)(entry + 0x04) = 0;
        entry[0x08] = 0;
        entry[0x09] = 0;
        *(short *)(entry + 0x10) = -1;
        code &= 0xFFF0FFFF;
        code &= 0xFFEFFFFF;
        code &= 0xFFDFFFFF;
        code &= 0xFE3FFFFF;
        code &= 0xFDFFFFFF;
        *(u32 *)(entry + 0x0C) = code;
    }

    D_8009CEA0 = 0;
    D_8009CED0 = 0;
    D_8009CED4 = 0;
    D_8009CE90 = arg0;

    for (i = 0; i < 2; i++) {
        offset = ((i * 8) - i) * 4;
        packet = D_8009EC70 + offset;
        SetDrawMode(packet, 0, 1, *(u16 *)D_80091680);
        prim = packet + 8;
        SetSprt((u8 *)prim);
        if (Gpu_AppendPacketPrimitive(packet, prim) != 0) {
            exit(-1);
        }

        prim = D_8009EC70 + offset + 8;
        Gpu_SetDrawEnable((u8 *)prim, 1);
        prim[0x0C] = 0x70;
        prim[0x0D] = *(u16 *)(D_80091680 - 6);
        *(short *)(prim + 0x10) = 0x18;
        *(short *)(prim + 0x12) = 0x0C;
        *(short *)(D_8009EC70 + offset + 0x16) = *(u16 *)(D_80091680 + 2);
        tpage = GetTPage(0, 0, 0, 0);

        offset = ((i * 2) + i) * 8;
        tile_packet = D_8009ECA8 + offset;
        SetDrawMode(tile_packet, 0, 1, tpage);
        prim = tile_packet + 8;
        SetTile((u8 *)prim);
        if (Gpu_AppendPacketPrimitive(tile_packet, prim) != 0) {
            exit(-1);
        }

        prim[4] = 2;
        prim[5] = 2;
        prim[6] = 2;
        *(short *)(prim + 8) = 0;
        *(short *)(prim + 0x0A) = 0xAA;
        *(short *)(prim + 0x0C) = 0x140;
        *(short *)(prim + 0x0E) = 0x36;
        Gpu_SetDither((u8 *)prim, 1);
    }
}

void Menu_SetTextCursorRect(int arg0, int arg1, int arg2, int arg3) {
    D_8009CE98 = arg0;
    D_8009CE9A = arg1;
    D_8009CE9C = arg2;
    D_8009CE9E = arg3;
}
