typedef struct {
    char bytes[4];
} Ovl178_FourBytes;

extern Ovl178_FourBytes D_8018F04C;

void func_800D1AE0(void *arg0, int arg1, int arg2, int arg3);

void func_80192C8C(void *arg0, void *arg1, char *arg2) {
    Ovl178_FourBytes local;

    local = D_8018F04C;
    func_800D1AE0(&local, *(short *)(arg2 + 2), 1, 8);
}
