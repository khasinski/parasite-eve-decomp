#include "common.h"
#include "pe1/save_blob.h"

extern u8 D_800B8868[];
extern u8 D_800B88C8[];
extern u8 D_8009EED0[];
extern unsigned short D_80010F48;
extern int D_800A5D50;
extern u8 *g_SaveIoCursor;
extern SaveBytes12E4 g_SaveRuntimeState;
extern int D_800C0DE8;

void bzero(void *dst, int len);
char *strcpy(char *dst, char *src);
u8 *Str_ResolveDataPtr(char *path);
u8 *Save_FormatTitle(int chapter, int playtime_seconds);
void Save_SerializeTail(void);

int Save_BuildCardFile(char *path) {
    u8 *resolved;
    u8 *file;
    u32 crc;

    resolved = Str_ResolveDataPtr(path);
    file = D_800B8868;

    D_800A5D50 = 0x2000;
    bzero(file, 0x100);

    *(short *)(file + 0) = D_80010F48;
    file[2] = 0x11;
    file[3] = 1;
    strcpy((char *)(file + 4), (char *)Save_FormatTitle(path[0x18] - 0x40, D_800C0DE8));

    __builtin_memcpy(D_800B88C8, resolved + 0x14, 0x20);
    __builtin_memcpy(file + 0x80, resolved + 0x40, 0x80);

    bzero(D_8009EED0, 0x2000);
    g_SaveIoCursor = D_8009EED0;
    __builtin_memcpy(g_SaveIoCursor, D_800B8868, 0x100);
    g_SaveIoCursor += 0x100;
    __builtin_memcpy(g_SaveIoCursor, (u8 *)&g_SaveRuntimeState, 0x12E4);
    g_SaveIoCursor += 0x12E4;

    Save_SerializeTail();

    {
        u8 *data = D_8009EED0;
        u32 checksum = 0xFFFF;
        u32 i = 0;
        do {
            u32 bit = 0;
            checksum ^= data[i & 0xFFFF] << 8;
            do {
                if (checksum & 0x8000) {
                    checksum = (checksum << 1) ^ 0x1021;
                } else {
                    checksum <<= 1;
                }
                bit++;
            } while ((bit & 0xFFFF) < 8);
            i++;
        } while ((i & 0xFFFF) < 0x2000);
        crc = ~checksum & 0xFFFF;
    }
    __builtin_memcpy(g_SaveIoCursor, &crc, 4);
    g_SaveIoCursor += 4;

    return 0;
}
