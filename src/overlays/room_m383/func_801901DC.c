typedef int s32;

typedef struct Room383Node {
    s32 flags;
} Room383Node;

typedef struct Room383Obj8 {
    Room383Node *node;
} Room383Obj8;

typedef struct Room383Obj {
    char pad0[8];
    Room383Obj8 *unk8;
} Room383Obj;

extern char D_80196DA4;

extern s32 FieldEng_GetStatus(Room383Obj *obj);
extern void FieldEng_Register(Room383Obj *obj, void *table);

s32 func_801901DC(Room383Obj *obj) {
    Room383Node *node;

    if (FieldEng_GetStatus(obj) == 3) {
        node = obj->unk8->node;
        node->flags &= 0xBFFFFFFF;
        FieldEng_Register(obj, &D_80196DA4);
    }

    return 0;
}
