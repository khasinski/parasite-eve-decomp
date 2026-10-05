#include "common.h"
typedef struct Room348Node {
    s32 flags;
} Room348Node;

typedef struct Room348Obj8 {
    Room348Node *node;
} Room348Obj8;

typedef struct Room348Obj {
    char pad0[8];
    Room348Obj8 *unk8;
} Room348Obj;

extern char D_80192880;

extern s32 FieldEng_GetStatus(Room348Obj *obj);
extern void func_800C2414(Room348Obj *obj, void *table);

s32 func_8018F0C4(Room348Obj *obj) {
    Room348Node *node;

    if (FieldEng_GetStatus(obj) == 3) {
        node = obj->unk8->node;
        node->flags &= 0xBFFFFFFF;
        func_800C2414(obj, &D_80192880);
    }

    return 0;
}
