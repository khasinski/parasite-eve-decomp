extern int FieldEng_GetStatus(void);
extern int func_800C251C(void *arg0, void *arg1);
extern int func_800C2758(void *arg0, void *arg1, void *arg2);

extern char D_801941C0[];
extern char D_801941D8[];
extern char D_801941E4[];

extern int RoomLib_CloseTarget_8018F308(void *arg0);

int func_8018F27C(void *arg0) {
    int result;

    if ((unsigned int)FieldEng_GetStatus() >= 2) {
        result = func_800C251C(arg0, D_801941D8);
        result |= func_800C2758(arg0, D_801941C0, D_801941E4);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F308(arg0);
    }

    return 0;
}
