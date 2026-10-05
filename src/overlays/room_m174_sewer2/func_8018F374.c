extern int FieldEng_GetStatus(void);
extern int func_800C251C(void *arg0, void *arg1);
extern int func_800C2758(void *arg0, void *arg1, void *arg2);

extern char D_80196C7C[];
extern char D_80196CA4[];
extern char D_80196CB8[];

extern int RoomLib_CloseTarget_8018F400(void *arg0);

int func_8018F374(void *arg0) {
    int result;

    if (FieldEng_GetStatus() == 3) {
        result = func_800C251C(arg0, D_80196CA4);
        result |= func_800C2758(arg0, D_80196C7C, D_80196CB8);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F400(arg0);
    }

    return 0;
}
