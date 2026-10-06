#include "pe1/field_anim.h"
#include "pe1/scene_assets.h"
#include "pe1/psyq_bios.h"

/* Binds task program `index` to the owner's task context. */
int func_800CE49C(FieldAnimTaskOwner *owner, int index)
{
    FieldAnimTaskProgram *program = D_800E1044[index];

    if (program == 0) {
        printf(D_800C2244, index);
        return -1;
    }
    owner->tasks.table = &program->table;
    owner->tasks.pc.script = program->script;
    return 0;
}
