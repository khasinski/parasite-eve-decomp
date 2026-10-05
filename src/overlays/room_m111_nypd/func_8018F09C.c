extern int FieldEng_GetStatus(void);
extern int func_800C251C(void *arg0, void *arg1);
extern int func_800C2758(void *arg0, void *arg1, void *arg2);

extern char D_80190094[];
extern char D_801900C4[];
extern char D_801900DC[];

extern int RoomLib_CloseTarget_8018F128(void *arg0);

int func_8018F09C(void *arg0) {
    int result;

    if ((unsigned int)FieldEng_GetStatus() >= 2) {
        result = func_800C251C(arg0, D_801900C4);
        result |= func_800C2758(arg0, D_80190094, D_801900DC);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F128(arg0);
    }

    return 0;
}
