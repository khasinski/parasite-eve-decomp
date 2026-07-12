typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

int FieldEng_GetStatus(void *obj);
void func_800C2D0C(u16 slot, u8 id, u16 size);

extern u8 *D_800F34F4;
extern char *D_800E2248;
extern char *D_800F3330;

void *func_800C2B90(void *obj, u8 id, u16 *sizes, int (**handlers)(void *, void *, void *)) {
    int slot = -1;
    int i;
    void *entry;
    void *data;
    int (*handler)(void *, void *, void *);

    for (i = 0; i < 0x40; i++) {
        if ((signed char)D_800F34F4[i * 6 + 1] == 0) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        *(int *)(D_800E2248 + 0x40) = 1;
        return 0;
    }

    if (FieldEng_GetStatus(obj) == 3 || FieldEng_GetStatus(obj) == 4 || FieldEng_GetStatus(obj) == 5) {
        func_800C2D0C(slot, id, sizes[id]);

        entry = D_800F34F4 + slot * 6;
        data = D_800F3330 + *(s16 *)((char *)entry + 4);
        handler = handlers[id];
        if (handler != (void *)-1) {
            handler(obj, entry, data);
        }
        return data;
    }

    return 0;
}
