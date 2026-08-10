
#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];

void Task_QueueFieldSfx(int arg0, int arg1, int arg2, int arg3, int arg4);

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
