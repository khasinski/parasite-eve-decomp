#include "common.h"
#include "pe1/render_object.h"

typedef unsigned short u16_1;

typedef unsigned char u8_2;
typedef short s16_2;

void Render_InitObjectFromTable(RenderObjectEntity *obj, RenderObjectEntity *owner, int index) {
    int offset;
    int base;
    u16 *entry;

    obj->table_index = index;
    offset = (short)index << 4;
    obj->header = 0;
    obj->animation_source = owner;

    base = (int)owner->bounds_vertices;
    entry = (u16 *)(offset + base);
    obj->table_value70 = entry[3];

    base = (int)owner->bounds_vertices;
    entry = (u16 *)(offset + base);
    obj->table_value2c = entry[0];

    base = (int)owner->bounds_vertices;
    entry = (u16 *)(offset + base);
    obj->table_value2e = entry[1];

    base = (int)owner->bounds_vertices;
    offset += base;
    entry = (u16 *)offset;
    obj->table_value30 = entry[2] + obj->table_value70;
}

void Render_Noop(void) {
}

int Render_ReturnZero(void) {
    return 0;
}

void Render_CopyMatrixBlock(u16_1 *arg0, u16_1 *arg1, s16 count) {
    char unused[8];
    register u16_1 *src_cur asm("$7");
    register u16_1 *dst_cur asm("$8");
    int i;
    register char *src_tail asm("$4");
    register char *dst_tail asm("$5");
    int tmp;

    src_cur = arg0;
    dst_cur = arg1;
    i = 0;
    if (count > 0) {
        dst_tail = (char *)arg1 + 0x1C;
        src_tail = (char *)arg0 + 0x1C;
        do {
            *dst_cur = *src_cur;
            *(u16_1 *)(dst_tail - 0x1A) = *(u16_1 *)(src_tail - 0x1A);
            *(u16_1 *)(dst_tail - 0x18) = *(u16_1 *)(src_tail - 0x18);
            *(u16_1 *)(dst_tail - 0x16) = *(u16_1 *)(src_tail - 0x16);
            *(u16_1 *)(dst_tail - 0x14) = *(u16_1 *)(src_tail - 0x14);
            *(u16_1 *)(dst_tail - 0x12) = *(u16_1 *)(src_tail - 0x12);
            *(u16_1 *)(dst_tail - 0x10) = *(u16_1 *)(src_tail - 0x10);
            *(u16_1 *)(dst_tail - 0xE) = *(u16_1 *)(src_tail - 0xE);
            tmp = *(u16_1 *)(src_tail - 0xC);
            i++;
            *(u16_1 *)(dst_tail - 0xC) = tmp;
            tmp = *(int *)(src_tail - 8);
            src_cur += 0x10;
            *(int *)(dst_tail - 8) = tmp;
            tmp = *(int *)(src_tail - 4);
            dst_cur += 0x10;
            *(int *)(dst_tail - 4) = tmp;
            tmp = *(int *)src_tail;
            src_tail += 0x20;
            *(int *)dst_tail = tmp;
            dst_tail += 0x20;
        } while (i < count);
    }
}

void Render_SetObjectAnim(RenderObjectEntity *arg0, RenderObjectEntity *arg1, short arg2) {
    if (arg0->header->part_count == 2) {
        arg0->animation_source = arg1;
        arg0->animation_state = 3;
    } else {
        arg0->animation_source = arg1;
        arg0->animation_state = 1;
    }
    arg0->animation_id = arg2;
}

void Render_ClearObjectAnim(RenderObjectEntity *arg0) {
    arg0->animation_source = 0;
    if (arg0->header->part_count == 2) {
        arg0->animation_state = 2;
    } else {
        arg0->animation_state = 0;
    }
}

int Render_FindAnimEntry(RenderObjectEntity *arg0, int arg1, s32 *out) {
    char unused[8];
    char *base;
    register char *entry asm("$3");
    s32 i;
    s32 tmp;

    tmp = arg0->header->animation_entry_count;
    base = (char *)arg0->animation_entries;
    i = 0;
    if (tmp > 0) {
        tmp = arg1 << 16;
        arg1 = tmp >> 16;
        entry = base + 4;
        do {
            tmp = *(s16_2 *)(entry + 2);
            i++;
            if (tmp == arg1) {
                out[0] = *(s16_2 *)(base + 0);
                out[1] = *(s16_2 *)(entry - 2);
                out[2] = *(s16_2 *)(entry + 0);
                return 1;
            }
            entry += 0xC;
            tmp = arg0->header->animation_entry_count;
            base += 0xC;
        } while (i < tmp);
    }

    return 0;
}
