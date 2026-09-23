#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef struct CdArchiveRoot {
    u8 reserved[4];
    u32 header_offset;
} CdArchiveRoot;

typedef struct CdArchiveHeader {
    u8 reserved[0x24];
    u32 range_info;
} CdArchiveHeader;

typedef struct CdFindState {
    u8 _00[0x10];
    u8 read_mode;
    u8 _11[0xE2];
    u8 state;
    u8 _F4[0xA0];
    void *buffer;
} CdFindState;

typedef struct CdRange {
    u8 _00[3];
    u8 flags;
    u16 first;
    u16 last;
} CdRange;

extern CdFindState D_800B0CD8;
extern u8 D_800B0E64[16];
extern int D_8009CDCC;
int CD_ReadSectors(int, int, int, void *, int, int);

int CD_FindNextDataSector(void)
{
    u8 *base = *(u8 **)D_800B0E64;
    CdFindState *state = &D_800B0CD8;
    CdArchiveHeader *header = (CdArchiveHeader *)(base + ((CdArchiveRoot *)base)->header_offset);
    CdRange *ranges = (CdRange *)(base + (header->range_info & 0x3fffff));

dispatch:
    switch (state->state) {
        case 0:
            D_8009CDCC = 0;
            state->state = 0x28;
            goto dispatch;
        case 0x28:
        {
            if (CD_ReadSectors(1, 1, 0, state->buffer, 0x21, 0) == 1) return 1;
            if (state->read_mode >= 2) {
                state->state = 0x29;
                return 1;
            }
            goto set_scan_state;
        }
        case 0x29:
        {
            if (CD_ReadSectors(1, state->read_mode, 0, state->buffer, 0x21, 0) == 1) return 1;
set_scan_state:
            state->state = 0x2a;
            goto dispatch;
        }
        case 0x2a:
        {
            int index;
            CdRange *entry;
            int byte_offset;
            index = D_8009CDCC;
            if (index < (int)(header->range_info >> 22)) {
                byte_offset = index * sizeof(CdRange);
                entry = (CdRange *)(byte_offset + (u32)ranges);
                if (entry->flags & 0x10) {
                    if (entry->first >= 2) goto read_range;
                }
                D_8009CDCC = index + 1;
                goto dispatch;
read_range:
                state->state = 0x2b;
                goto dispatch;
            } else {
                state->state = 0;
                return 0;
            }
        }
        case 0x2b:
        {
            CdRange *entry = (CdRange *)(D_8009CDCC * sizeof(CdRange) + (u32)ranges);
            if (CD_ReadSectors(3, entry->first, entry->last, state->buffer, 0x21, 0) == 1) return 1;
            {
                state->state = 0x2a;
                D_8009CDCC = D_8009CDCC + 1;
            }
            goto dispatch;
        }
        default:
            return 0;
    }
}
