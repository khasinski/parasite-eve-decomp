/* Script opcodes: random value in a range and a field sound effect on the
 * current actor. Contiguous default-profile handlers. */
#include "pe1/field_sfx.h"

extern FieldActor *g_CurrentEntity[];

u32 Task_GpuFlushPrimQueue(void);
int Task_GpuPackPrimColor(int start, int end);

/* Equal bounds return one raw RNG word. Otherwise the result is in [start, end). */
int Task_Random(int **arg0) {
    int start = *arg0[1];
    int end = *arg0[2];

    if (start == end) {
        *arg0[0] = Task_GpuFlushPrimQueue();
    } else {
        *arg0[0] = Task_GpuPackPrimColor(start, end);
    }

    return 1;
}

int Task_PlayFieldSfx(FieldSfxScriptArgs *arg0) {
    register int a;
    int b;
    int c;
    int d;
    void *ptr0;
    void *ptr1;
    void *ptr2;

    ptr2 = (void *)arg0->taskArgument;
    ptr1 = (void *)arg0->subId;
    a = *(unsigned char *)ptr2;
    b = *(unsigned char *)ptr1;
    ptr0 = (void *)arg0->typeId;
    c = *(unsigned short *)ptr0;
    d = g_CurrentEntity[0]->field_sfx_id;
    Task_QueueFieldSfx(a, b, c, d, 0);
    return 1;
}
