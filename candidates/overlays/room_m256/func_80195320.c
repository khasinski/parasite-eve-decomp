/* Non-matching reconstruction of room_m256 [0x6338, 0x6544).
 * GCC 2.7.2 emits 0x204 bytes; retail has 0x20C. The mode 0 and mode 1
 * instructions match after relocation normalization. Mode 2 still differs
 * in load/store scheduling and register allocation. Never count as a match.
 */
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern int D_800B0E64;
extern u16 D_800E11EA;
extern char *D_800E2368;
extern int D_800E27EC;
extern u16 D_800E2850[];
extern char *D_800F32D0;
extern short D_800F336C, D_800F336E, D_800F3372, D_800F3374;
extern u16 D_800F3370;
extern char *D_800F33E0;
extern int D_80195EF8;
extern char func_801940B0[];

extern int func_8006E498(int, int);
extern void func_800C6D5C(int, int, int);
extern int func_800CE560(char *, int, int, void *);
extern char *func_800CE610(char *);
extern int func_800D3FD8(void);
extern void func_800D3F64(int, int);

int func_80195320(int mode, u16 *out) {
    char *p, *q, *r;
    register int ret asm("$2");
    register char *task asm("$3");
    u16 value;

    if (mode == 1) goto mode1;
    if (mode < 2) {
        if (mode == 0) goto mode0;
        return 0;
    }
    if (mode == 2) goto mode2;
    return 0;

mode0:
    *out = *(u16 *)(D_800E2368 + 0x12);
    D_80195EF8 = func_8006E498(D_800B0E64, 0xC5887704);
    func_800C6D5C(D_80195EF8, 0, 0);
    if (*(u8 *)(D_800E2368 + 0xD)) {
        p = *(char **)(D_800F32D0 + 8);
        if (p) {
            q = *(char **)p;
            if (q) {
                r = *(char **)(q + 0x18);
                if (*r == 1) *r = 2;
            }
        }
    }
    return func_800CE560(*(char **)(D_800F33E0 + 8), 0x18, 0x20,
                           func_801940B0);

mode1:
    if (D_800E27EC == mode) {
        ret = D_800E27EC < 8;
        task = func_800CE610(*(char **)(D_800F33E0 + 8));
        if (task) {
            value = *out;
            *(s16 *)(task + 0x14) = 0;
            *(s16 *)(task + 0x16) = 0;
            *(u16 *)(task + 6) = value;
        }
        func_800D3F64(0x5A6, func_800D3FD8());
        func_800D3F64(0x5BE, 0x80);
        ret = D_800E27EC < 8;
    } else {
        ret = D_800E27EC < 8;
    }
    if (ret) return 0;
    return 2;

mode2:
    D_800F336C = 3;
    D_800F336E = 0;
    D_800F3372 = 0;
    D_800F3374 = 8;
    D_800F3370 = D_800E2850[D_800E11EA];
    return 0;
}
