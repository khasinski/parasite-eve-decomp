typedef int s32;

extern char D_80196DDC;
extern char D_80196D6C;
extern char D_80196E14;

s32 func_800C251C(void *arg0, void *arg1);
s32 func_800C2758(void *arg0, void *arg1, void *arg2);
int FieldEng_GetStatus(void);
void RoomLib_CloseTarget_801902CC(void *arg0);

s32 func_80190240(void *arg0) {
    s32 ret;

    if (FieldEng_GetStatus() == 3) {
        ret = func_800C251C(arg0, &D_80196DDC);
        ret = ret | func_800C2758(arg0, &D_80196D6C, &D_80196E14);
    } else {
        ret = -1;
    }

    if (ret == -1) {
        RoomLib_CloseTarget_801902CC(arg0);
    }

    return 0;
}
