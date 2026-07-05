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
