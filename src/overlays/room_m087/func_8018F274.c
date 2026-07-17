extern int FieldEng_GetStatus(void);
extern int func_800C251C(void *arg0, void *arg1);
extern int func_800C2758(void *arg0, void *arg1, void *arg2);

extern char D_801934EC[];
extern char D_80193504[];
extern char D_80193510[];

extern int RoomLib_CloseTarget_8018F300(void *arg0);

int func_8018F274(void *arg0) {
    int result;

    if ((unsigned int)FieldEng_GetStatus() >= 2) {
        result = func_800C251C(arg0, D_80193504);
        result |= func_800C2758(arg0, D_801934EC, D_80193510);
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F300(arg0);
    }

    return 0;
}
