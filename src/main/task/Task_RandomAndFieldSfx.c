/* Script opcodes: random value in a range and a field sound effect on the
 * current actor. Contiguous default-profile handlers. */
#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];

u32 Task_GpuFlushPrimQueue(void);
int Task_GpuPackPrimColor(int start, int end);
void Task_QueueFieldSfx(int arg0, int arg1, int arg2, int arg3, int arg4);

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

int Task_PlayFieldSfx(void **arg0) {
    register int a;
    int b;
    int c;
    int d;
    void *ptr0;
    void *ptr1;
    void *ptr2;

    ptr2 = arg0[2];
    ptr1 = arg0[1];
    a = *(unsigned char *)ptr2;
    b = *(unsigned char *)ptr1;
    ptr0 = arg0[0];
    c = *(unsigned short *)ptr0;
    d = g_CurrentEntity[0]->field_sfx_id;
    Task_QueueFieldSfx(a, b, c, d, 0);
    return 1;
}
