extern int FieldEng_GetStatus(void);
extern int func_800C251C(void *arg0, void *arg1);
extern int func_800C2758(void *arg0, void *arg1, void *arg2);

extern char D_8018FFF8[];
extern char D_80190010[];
extern char D_8019001C[];

extern int RoomLib_CloseTarget_8018F128(void *arg0);

int func_8018F09C(void *arg0) {
    int result;

    if ((unsigned int)FieldEng_GetStatus() >= 2) {
        result = func_800C251C(arg0, D_80190010);
        result |= func_800C2758(arg0, D_8018FFF8, D_8019001C);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F128(arg0);
    }

    return 0;
}
