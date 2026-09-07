/* Partial storage views; the allocated entry's fourth halfword is unnamed. */
typedef struct { unsigned short x, y, z, flags; } Entry;
typedef struct { Entry entries[2]; unsigned char unknown[14]; unsigned char stopped; } EmitterState;
typedef struct { int unknown[2]; void *pool; } Context;
extern void *D_800B0E64, *D_8019AF6C;
extern Context *D_800F33E0;
extern EmitterState D_8019AF84;
extern short D_800F3372, D_800F3374;
extern void *Asset_FindTable08ByU32Key(void *, unsigned int);
extern void func_800C6D5C(void *, int, int);
extern int func_80198E94(int, void *);
extern int func_800CE560(void *, int, int, int (*)(int, void *));
extern Entry *func_800CE610(void *);

int func_801993F0(int mode) {
    switch (mode) {
    case 0:
        D_8019AF6C = Asset_FindTable08ByU32Key(D_800B0E64, 0xC5541704);
        func_800C6D5C(D_8019AF6C, 0, 0);
        return func_800CE560(D_800F33E0->pool, 8, 4, func_80198E94);
    case 1: {
        int i;
        unsigned char *flags;
        Entry *source;
        unsigned char *anchor = (unsigned char *)&D_8019AF84 + 30;
        if (*anchor) return 2;
        i = 0;
        flags = anchor - 24;
        source = (Entry *)(anchor - 30);
        for (; i < 2; ++i) {
            asm("" : "=r"(source) : "0"(source));
            if (*(unsigned short *)flags & 1) {
                Entry *entry = func_800CE610(D_800F33E0->pool);
                if (!entry) return 0;
                entry->x = source->x;
                entry->y = source->y;
                entry->z = source->z;
                entry->flags = 0;
                *(unsigned short *)flags &= 0xFFFE;
            }
            flags += 8;
            ++source;
        }
        break;
    }
    case 2:
        D_800F3372 = 0;
        D_800F3374 = 0;
        break;
    }
    return 0;
}
