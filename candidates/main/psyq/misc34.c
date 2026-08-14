/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */


typedef signed int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef union DsLba {
    s32 addr;
    struct {
        u8 unk0;
        u8 unk1;
        u8 unk2;
        u8 unk3;
    } i;
} DsLba;

typedef struct DsLoc {
    u8 minute;
    u8 second;
    u8 sector;
    u8 track;
} DsLoc;

extern s32 D_8009AFC0;
extern s32 D_8009B6DC;
extern u8 D_800A52B0[];

/* Directory table field views, stride 0x2C. */
extern s32 D_800A3CB0[];
extern s32 D_800A3CB4[];
extern char D_800A3CB8[];
extern char D_800A3CBC[];
extern s32 D_800A3C8C[];

/* File table field views, stride 0x18. */
extern u8 D_800A36B0[];
extern u8 D_800A36B1[];
extern u8 D_800A36B2[];
extern s32 D_800A36B4[];
extern char D_800A36B8[];
extern u8 D_800A5AB0[];

int ds_read(int count, int lba, u8 *buf);
int strncmp(const char *s1, const char *s2, int n);
int strcmp(const char *s1, const char *s2);
void *memcpy(char *dst, const char *src, int n);
char *strcpy(char *dst, const char *src);
int CdIntToPos(int lba, DsLoc *pos);
int puts(const char *s);
int printf(const char *fmt, ...);

extern char D_80011EF8[];
extern char D_80011F24[];
extern char D_80011F2C[];
extern char D_80011F5C[];
extern char D_80011F80[];
extern char D_80011FA0[];
extern char D_80011FB4[];
extern char D_80011FD8[];
extern char D_80011FF8[];
extern char D_8001201C[];
extern char D_80012038[];

s32 DS_newmedia(void) {
    DsLba lba;
    u8 *p;
    u8 *end;
    s32 i;
    s32 off;

    if (ds_read(1, 0x10, D_800A52B0) != 1) {
        if (D_8009AFC0 > 0) {
            puts(D_80011EF8);
        }
        return 0;
    }
    if (strncmp((char *)&D_800A52B0[1], D_80011F24, 5) != 0) {
        if (D_8009AFC0 > 0) {
            puts(D_80011F2C);
        }
        return 0;
    }

    (&lba)->i = ((DsLba *)&D_800A52B0[0x8C])->i;
    if (ds_read(1, lba.addr, D_800A52B0) != 1) {
        if (D_8009AFC0 > 0) {
            printf(D_80011F5C, lba.addr);
        }
        return 0;
    }
    if (D_8009AFC0 > 1) {
        puts(D_80011F80);
    }

    i = 0;
    p = D_800A52B0;
    end = D_800A52B0 + 0x800;
    while (p < end) {
        if (p[0] == 0) {
            break;
        }
        off = i * 0x2C;
        ((DsLba *)(D_800A3CB8 + off))->i = ((DsLba *)&p[2])->i;
        *(s32 *)((char *)D_800A3CB0 + off) = i + 1;
        *(s32 *)((char *)D_800A3CB4 + off) = p[6];
        memcpy(D_800A3CB8 + off + 4, (char *)&p[8], p[0]);
        (D_800A3CB8 + off + 4)[p[0]] = 0;
        p += 8 + p[0] + p[0] % 2;
        if (D_8009AFC0 > 1) {
            printf(D_80011FA0, *(s32 *)(D_800A3CB8 + off),
                   *(s32 *)((char *)D_800A3CB0 + off),
                   *(s32 *)((char *)D_800A3CB4 + off), D_800A3CB8 + off + 4);
        }
        if (++i >= 0x80) {
            break;
        }
    }
    if (i < 0x80) {
        *(s32 *)((char *)D_800A3CB4 + i * 0x2C) = 0;
    }

    D_8009B6DC = 0;
    if (D_8009AFC0 > 1) {
        printf(D_80011FB4, i);
    }
    return 1;
}

s32 DS_searchdir(s32 arg0, char *arg1) {
    s32 i;
    char *name;
    s32 off;
    s32 v;

    i = 0;
    name = D_800A3CBC;
    off = 0;
    while (i < 0x80) {
        v = *(s32 *)((char *)D_800A3CB4 + off);
        if (v == 0) {
            return -1;
        }
        if (v == arg0) {
            if (strcmp(arg1, name) == 0) {
                return i + 1;
            }
        }
        name += 0x2C;
        i++;
        off += 0x2C;
    }
    return -1;
}

s32 DS_cachefile(s32 arg0) {
    DsLba lba;
    u8 *p;
    s32 i;
    s32 off;
    DsLoc *pos;
    char *name;

    if (arg0 == D_8009B6DC) {
        return 1;
    }

    if (ds_read(1, *(s32 *)((char *)D_800A3C8C + arg0 * 0x2C), D_800A52B0) != 1) {
        if (D_8009AFC0 > 0) {
            puts(D_80011FD8);
        }
        return -1;
    }
    if (D_8009AFC0 > 1) {
        puts(D_80011FF8);
    }

    i = 0;
    off = 0;
    name = (char *)D_800A36B0 + 8;
    pos = (DsLoc *)D_800A36B0;
    p = D_800A52B0;
    if (p < D_800A52B0 + 0x800) {
        do {
            if (p[0] == 0) {
                break;
            }
            (&lba)->i = ((DsLba *)&p[2])->i;
            CdIntToPos(lba.addr, pos);
            ((DsLba *)((char *)D_800A36B0 + 4 + off))->i = ((DsLba *)&p[0xA])->i;
            switch (i) {
            case 0:
                strcpy((char *)D_800A36B0 + off + 8, ".");
                break;
            case 1:
                strcpy((char *)D_800A36B0 + off + 8, "..");
                break;
            default:
                memcpy(name, (char *)&p[0x21], p[0x20]);
                name[p[0x20]] = 0;
                break;
            }
            if (D_8009AFC0 > 1) {
                printf(D_8001201C, *(u8 *)((char *)D_800A36B0 + off),
                       *(u8 *)((char *)D_800A36B1 + off),
                       *(u8 *)((char *)D_800A36B2 + off),
                       *(s32 *)((char *)D_800A36B4 + off), name);
            }
            name += 0x18;
            off += 0x18;
            i++;
            p += p[0];
            pos = (DsLoc *)((char *)pos + 0x18);
            if (i >= 0x40) {
                break;
            }
        } while (p < (u8 *)D_800A5AB0);
    }
    D_8009B6DC = arg0;
    if (i < 0x40) {
        D_800A36B8[i * 0x18] = 0;
    }
    if (D_8009AFC0 > 1) {
        printf(D_80012038, i);
    }
    return 1;
}
