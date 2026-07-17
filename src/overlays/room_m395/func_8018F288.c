extern int FieldEng_GetStatus(void);
extern int func_800C251C(void *arg0, void *arg1);
extern int func_800C2758(void *arg0, void *arg1, void *arg2);

extern char D_80194334[];
extern char D_8019434C[];
extern char D_80194358[];

extern int RoomLib_CloseTarget_8018F314(void *arg0);

int func_8018F288(void *arg0) {
    int result;

    if ((unsigned int)FieldEng_GetStatus() >= 2) {
        result = func_800C251C(arg0, D_8019434C);
        result |= func_800C2758(arg0, D_80194334, D_80194358);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F314(arg0);
    }

    return 0;
}
