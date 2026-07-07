/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern char *g_CurrentEntity[];

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
    d = *(unsigned short *)(g_CurrentEntity[0] + 0x24);
    Task_QueueFieldSfx(a, b, c, d, 0);
    return 1;
}
