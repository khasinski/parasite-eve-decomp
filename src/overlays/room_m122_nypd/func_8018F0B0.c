extern int FieldEng_GetStatus(void);
extern int func_800C251C(void *arg0, void *arg1);
extern int func_800C2758(void *arg0, void *arg1, void *arg2);

extern char D_80190EDC[];
extern char D_80190F0C[];
extern char D_80190F24[];

extern int RoomLib_CloseTarget_8018F13C(void *arg0);

int func_8018F0B0(void *arg0) {
    int result;

    if ((unsigned int)FieldEng_GetStatus() >= 2) {
        result = func_800C251C(arg0, D_80190F0C);
        result |= func_800C2758(arg0, D_80190EDC, D_80190F24);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F13C(arg0);
    }

    return 0;
}
