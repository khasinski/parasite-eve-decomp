typedef unsigned short u16;

typedef struct {
    int field00;
    char pad04[0x20];
    void *field24;
    char pad28[4];
    u16 field2C;
    u16 field2E;
    u16 field30;
    u16 field32;
    char pad34[0x3C];
    u16 field70;
} Unk8003DF50Obj;

typedef struct {
    char pad[0x18];
    u16 *table;
} Unk8003DF50TableOwner;

typedef unsigned short u16_1;
typedef short s16;

typedef unsigned char u8;

typedef unsigned char u8_2;
typedef short s16_2;
typedef int s32;

void Render_InitObjectFromTable(Unk8003DF50Obj *obj, Unk8003DF50TableOwner *owner, int index) {
    int offset;
    int base;
    u16 *entry;

    obj->field32 = index;
    offset = (short)index << 4;
    obj->field00 = 0;
    obj->field24 = owner;

    base = (int)owner->table;
    entry = (u16 *)(offset + base);
    obj->field70 = entry[3];

    base = (int)owner->table;
    entry = (u16 *)(offset + base);
    obj->field2C = entry[0];

    base = (int)owner->table;
    entry = (u16 *)(offset + base);
    obj->field2E = entry[1];

    base = (int)owner->table;
    offset += base;
    entry = (u16 *)offset;
    obj->field30 = entry[2] + obj->field70;
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

void Render_SetObjectAnim(void *arg0, int arg1, short arg2) {
    if (*(u8 *)(*(char **)arg0 + 2) == 2) {
        *(int *)((char *)arg0 + 0x24) = arg1;
        *(short *)((char *)arg0 + 0x28) = 3;
    } else {
        *(int *)((char *)arg0 + 0x24) = arg1;
        *(short *)((char *)arg0 + 0x28) = 1;
    }
    *(short *)((char *)arg0 + 0x2A) = arg2;
}

void Render_ClearObjectAnim(void *arg0) {
    *(int *)((char *)arg0 + 0x24) = 0;
    if (*(u8 *)(*(char **)arg0 + 2) == 2) {
        *(short *)((char *)arg0 + 0x28) = 2;
    } else {
        *(short *)((char *)arg0 + 0x28) = 0;
    }
}

int Render_FindAnimEntry(void *arg0, int arg1, s32 *out) {
    char unused[8];
    char *base;
    register char *entry asm("$3");
    s32 i;
    s32 tmp;

    tmp = *(u8_2 *)(*(char **)arg0 + 3);
    base = *(char **)((char *)arg0 + 0x80);
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
            tmp = *(u8_2 *)(*(char **)arg0 + 3);
            base += 0xC;
        } while (i < tmp);
    }

    return 0;
}
