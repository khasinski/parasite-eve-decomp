extern int FieldEng_GetStatus(void);
extern int func_800C251C(void *arg0, void *arg1);
extern int func_800C2758(void *arg0, void *arg1, void *arg2);

extern char D_80193F20[];
extern char D_80193F50[];
extern char D_80193F68[];

extern int RoomLib_CloseTarget_8018F348(void *arg0);

int func_8018F284(void *arg0) {
    char *entry;
    int result = 0;

    if ((unsigned int)FieldEng_GetStatus() >= 2) {
        entry = *(char **)((char *)arg0 + 8);
        if ((unsigned char)entry[0xE] >= 2 || *(unsigned char *)(*(char **)entry + 0xAC) != 0) {
            result = func_800C251C(arg0, D_80193F50);
            result |= func_800C2758(arg0, D_80193F20, D_80193F68);
        }
    } else {
        result = -1;
    }

    if (result == -1) {
        RoomLib_CloseTarget_8018F348(arg0);
    }

    return 0;
}
