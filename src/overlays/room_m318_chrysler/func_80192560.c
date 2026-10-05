#include "pe1/field_actor.h"

extern FieldActor *D_8009D20C;
extern FieldActor *D_8009D254;
extern char D_8018F1CC[];

extern void func_800CE870();
extern void func_80071A74();

FieldActor *func_80192560(int type_id, int sub_id, int unused) {
    FieldActor *actor;

    actor = D_8009D20C;
    while (actor != 0) {
        if ((actor != D_8009D254) && (*(char **)actor != 0) &&
            (*(int *)(*(char **)actor + 0x10) > 0) &&
            (actor->sub_id == type_id) && (actor->type_id == sub_id)) {
            func_800CE870(actor, 0, unused);
            return actor;
        }
        actor = actor->next;
    }

    func_80071A74(D_8018F1CC, type_id, sub_id);
    return 0;
}
