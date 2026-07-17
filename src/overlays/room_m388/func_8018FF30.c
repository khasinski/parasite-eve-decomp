extern int FieldEng_GetStatus(void);
extern int func_800C251C(void *arg0, void *arg1);
extern int func_800C2758(void *arg0, void *arg1, void *arg2);

extern char D_801941CC[];
extern char D_801941F4[];
extern char D_80194208[];

extern int RoomLib_CloseTarget_8018FFBC(void *arg0);

int func_8018FF30(void *arg0) {
    int result;

    if (FieldEng_GetStatus() == 3) {
        result = func_800C251C(arg0, D_801941F4);
        result |= func_800C2758(arg0, D_801941CC, D_80194208);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018FFBC(arg0);
    }

    return 0;
}
