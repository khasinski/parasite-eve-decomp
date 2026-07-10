/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "include_asm.h"

extern int g_FieldMoveLock[];

#include "pe1/field_actor.h"
#include "pe1/task_node.h"

extern FieldActor *g_CurrentEntity[];
extern int g_SceneDataTable0;

extern TaskNode *D_8009D300;
extern int *D_8009CE00;
extern struct { char _[16]; } D_8009D1A0_o __asm__("D_8009D1A0");
extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
#define D_8009D1A0 (*(unsigned int *)&D_8009D1A0_o)
#define D_8009D254 (*(FieldActor **)&D_8009D254_o)
extern int D_8009DF70[];
extern int D_800A77F0[];
extern int D_800B6A80[];
extern int (*D_800910A0[])(int **args);

int Task_GetPlayerInputFlag(int **arg0) {
    if (g_FieldMoveLock[0] & 1) {
        *arg0[0] = 0;
    } else {
        *arg0[0] = 1;
    }
    return 1;
}

void Task_RunQueue(void) {
    int *args[16];
    TaskNode *node;
    register FieldActor *entity asm("$4");
    unsigned int header;
    unsigned int mode_bits;
    unsigned int argc;
    unsigned int opcode;
    int node_flags;
    int mode_tail;
    int *arg_data;
    unsigned short i;

    do {
        node = D_8009D300;
        node_flags = *(volatile int *)((char *)node + 8);
        if ((node_flags & 0x50) == 0) {
            entity = g_CurrentEntity[0];
            if (((entity->flags & 0x1000) == 0) || (*(unsigned short *)((char *)node + 8) & 0x80)) {
                if (((D_8009D1A0 & 0x100) == 0) || (entity == D_8009D254) ||
                    (*(unsigned short *)((char *)node + 8) & 0x80)) {
                    if (node->field_10 != 0) {
                        node->field_10--;
                        if (node->field_10 == 0) {
                            D_8009CE00 = (int *)node->ptr;
                            do {
                                header = *D_8009CE00++;
                                arg_data = D_8009CE00 + 1;
                                mode_tail = *D_8009CE00;
                                mode_bits = header >> 17;
                                opcode = header & 0x1FFF;
                                argc = (header >> 13) & 0xF;
                                D_8009CE00 = arg_data + argc;

                                for (i = 0; i < argc; i++) {
                                    switch (mode_bits & 7) {
                                    case 0:
                                        args[i] = &arg_data[i];
                                        break;
                                    case 1:
                                        args[i] = &D_8009DF70[arg_data[i]];
                                        break;
                                    case 2:
                                        args[i] = (int *)((char *)entity + 0xAC + arg_data[i] * 4);
                                        break;
                                    case 3:
                                        args[i] = &D_800A77F0[arg_data[i]];
                                        break;
                                    case 4:
                                        args[i] = &D_800B6A80[arg_data[i]];
                                        break;
                                    }

                                    if (i == 4) {
                                        mode_bits = mode_tail;
                                    } else {
                                        mode_bits >>= 3;
                                    }
                                }
                            } while (D_800910A0[opcode](args) != 0);

                            node = D_8009D300;
                            node->ptr = (int)D_8009CE00;
                        }
                    }
                }
            }
        }

        D_8009D300 = node->next;
    } while (D_8009D300 != 0);
}

int Task_JumpToEntityOffset(int **arg0) {
    g_SceneDataTable0 = g_CurrentEntity[0]->field_9c + (*arg0)[0] * 2;
    return 1;
}
