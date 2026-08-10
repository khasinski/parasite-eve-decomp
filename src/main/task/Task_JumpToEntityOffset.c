/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/field_actor.h"

typedef struct {
    int value;
} TaskJumpOffsetEntry;

extern FieldActor *D_8009D2F0[];
extern char *D_8009CE00;

int Task_JumpToEntityOffset(TaskJumpOffsetEntry **arg0)
{
    D_8009CE00 = D_8009D2F0[0]->script_base + ((*arg0)->value * 2);
    return 1;
}
