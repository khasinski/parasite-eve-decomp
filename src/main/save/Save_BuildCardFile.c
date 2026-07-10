typedef unsigned char u8;
typedef unsigned int u32;

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
u32 Save_CalcCrc16(u32 size, u8 *data);

#define COPY_BYTES(dst_, src_, count_)                       \
    do {                                                     \
        u8 *copy_dst_ = (u8 *)(dst_);                        \
        u8 *copy_src_ = (u8 *)(src_);                        \
        int copy_count_ = (count_);                          \
                                                             \
        while (copy_count_-- > 0) {                          \
            *copy_dst_++ = *copy_src_++;                     \
        }                                                    \
    } while (0)

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

    COPY_BYTES(D_800B88C8, resolved + 0x14, 0x20);
    COPY_BYTES(file + 0x80, resolved + 0x40, 0x80);

    bzero(D_8009EED0, 0x2000);
    g_SaveIoCursor = D_8009EED0;
    COPY_BYTES(g_SaveIoCursor, D_800B8868, 0x100);
    g_SaveIoCursor += 0x100;
    COPY_BYTES(g_SaveIoCursor, (u8 *)&g_SaveRuntimeState, 0x12E4);
    g_SaveIoCursor += 0x12E4;

    Save_SerializeTail();

    crc = Save_CalcCrc16(0x2000, D_8009EED0);
    *(u32 *)g_SaveIoCursor = crc;
    g_SaveIoCursor += 4;

    return 0;
}
