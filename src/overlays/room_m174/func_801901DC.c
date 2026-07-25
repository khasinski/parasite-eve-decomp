typedef int s32;

typedef struct Room174Node {
    s32 flags;
} Room174Node;

typedef struct Room174Obj8 {
    Room174Node *node;
} Room174Obj8;

typedef struct Room174Obj {
    char pad0[8];
    Room174Obj8 *unk8;
} Room174Obj;

extern char D_80196DA4;

extern s32 FieldEng_GetStatus(Room174Obj *obj);
extern void FieldEng_Register(Room174Obj *obj, void *table);

s32 func_801901DC(Room174Obj *obj) {
    Room174Node *node;

    if (FieldEng_GetStatus(obj) == 3) {
        node = obj->unk8->node;
        node->flags &= 0xBFFFFFFF;
        FieldEng_Register(obj, &D_80196DA4);
    }

    return 0;
}
