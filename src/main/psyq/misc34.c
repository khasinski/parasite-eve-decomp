typedef unsigned char u8;

#define DS_READ_U32(ptr_) \
    ((unsigned int)((ptr_)[0]) | ((unsigned int)((ptr_)[1]) << 8) | ((unsigned int)((ptr_)[2]) << 16) | ((unsigned int)((ptr_)[3]) << 24))
#define DS_DIR_ENTRY(i_) ((u8 *)D_800A3CB0 + ((i_) * 0x2C))
#define DS_CACHE_ENTRY(i_) ((u8 *)D_800A36B0 + ((i_) * 0x18))

int ds_read(int arg0, int arg1, void *arg2);
int strncmp(char *a, char *b, int n);
int puts(char *s);
int printf(char *fmt, ...);
void *memcpy(void *dst, void *src, int n);
void *CdIntToPos(int i, void *p);

extern int D_8009AFC0;
extern int D_8009B6DC;
extern u8 D_800A36B0[];
extern u8 D_800A3C8C[];
extern u8 D_800A3CB0[];
extern u8 D_800A52B0[];

extern char D_80011EF8[];
extern char D_80011F24[];
extern char D_80011F2C[];
extern char D_80011F5C[];
extern char D_80011F80[];
extern char D_80011FA0[];
extern char D_80011FB4[];
extern char D_80011FD8[];
extern char D_80011FF8[];
extern unsigned short D_80012014;
extern short D_80012018;
extern signed char D_8001201A;
extern char D_8001201C[];
extern char D_80012038[];

int DS_newmedia(void) {
    u8 *buffer;
    u8 *entry;
    u8 *dst;
    u8 *end;
    int count;
    int sector;
    int length;
    int entry_offset;

    buffer = D_800A52B0;
    if (ds_read(1, 0x10, buffer) != 1) {
        if (D_8009AFC0 > 0) {
            puts(D_80011EF8);
        }
        return 0;
    }

    if (strncmp((char *)(buffer + 1), D_80011F24, 5) != 0) {
        if (D_8009AFC0 > 0) {
            puts(D_80011F2C);
        }
        return 0;
    }

    sector = DS_READ_U32(buffer + 0x8C);
    if (ds_read(1, sector, buffer) != 1) {
        if (D_8009AFC0 > 0) {
            printf(D_80011F5C, sector);
        }
        return 0;
    }

    if (D_8009AFC0 >= 2) {
        puts(D_80011F80);
    }

    entry = buffer;
    end = buffer + 0x800;
    count = 0;
    while (entry < end && entry[0] != 0) {
        entry_offset = count * 0x2C;
        dst = (u8 *)D_800A3CB0 + entry_offset;
        *(unsigned int *)(dst + 8) = DS_READ_U32(entry + 2);
        *(unsigned int *)dst = count + 1;
        *(unsigned int *)(dst + 4) = entry[6];
        length = entry[0];
        memcpy(dst + 0xC, entry + 8, length);
        dst[0xC + length] = 0;
        entry += length + 8 + (length & 1);

        if (D_8009AFC0 >= 2) {
            printf(D_80011FA0, *(unsigned int *)(dst + 8), *(unsigned int *)dst,
                   *(unsigned int *)(dst + 4), dst + 0xC);
        }

        count++;
        if (count >= 0x80) {
            break;
        }
    }

    if (count < 0x80) {
        *(unsigned int *)(DS_DIR_ENTRY(count) + 4) = 0;
    }

    D_8009B6DC = 0;
    if (D_8009AFC0 >= 2) {
        printf(D_80011FB4, count);
    }

    return 1;
}

int strcmp(char *a, char *b);

extern int D_800A3CB4[];
extern char D_800A3CBC[];

int DS_searchdir(int dev, char *name) {
    register int saved_dev asm("$19");
    register char *saved_name asm("$20");
    register int i asm("$16");
    register int offset asm("$17");
    register char *entry_name asm("$18");
    register int value asm("$2");

    saved_dev = dev;
    saved_name = name;
    i = 0;
    entry_name = D_800A3CBC;
    offset = 0;
    do {
        asm volatile(
            "lui %0,%%hi(D_800A3CB4)\n\t"
            "addu %0,%0,%1\n\t"
            "lw %0,%%lo(D_800A3CB4)(%0)"
            : "=r"(value)
            : "r"(offset));

        if (value == 0) {
            return -1;
        }
        if (value == saved_dev) {
            if (strcmp(saved_name, entry_name) == 0) {
                return i + 1;
            }
            entry_name += 0x2C;
        } else {
            entry_name += 0x2C;
        }
        i++;
        offset += 0x2C;
    } while (i < 0x80);

    return -1;
}

int DS_cachefile(int index) {
    u8 *buffer;
    u8 *entry;
    u8 *dst;
    u8 *end;
    int count;
    int offset;
    int length;

    if (index == D_8009B6DC) {
        return 1;
    }

    offset = index * 0x2C;
    buffer = D_800A52B0;
    if (ds_read(1, *(int *)(D_800A3C8C + offset), buffer) != 1) {
        if (D_8009AFC0 > 0) {
            puts(D_80011FD8);
        }
        return -1;
    }

    if (D_8009AFC0 >= 2) {
        puts(D_80011FF8);
    }

    entry = buffer;
    end = buffer + 0x800;
    count = 0;
    offset = 0;
    while (entry < end && entry[0] != 0 && count < 0x40) {
        dst = D_800A36B0 + offset;
        CdIntToPos(DS_READ_U32(entry + 2), dst);
        *(unsigned int *)(dst + 4) = DS_READ_U32(entry + 0xA);

        if (count == 0) {
            *(unsigned short *)(dst + 8) = D_80012014;
        } else if (count == 1) {
            *(short *)(dst + 8) = D_80012018;
            *(signed char *)(dst + 0xA) = D_8001201A;
        } else {
            length = entry[0x20];
            memcpy(dst + 8, entry + 0x21, length);
            dst[8 + length] = 0;
        }

        if (D_8009AFC0 >= 2) {
            printf(D_8001201C, dst[0], dst[1], dst[2], *(unsigned int *)(dst + 4), dst + 8);
        }

        offset += 0x18;
        count++;
        entry += entry[0];
    }

    D_8009B6DC = index;
    if (count < 0x40) {
        DS_CACHE_ENTRY(count)[8] = 0;
    }

    if (D_8009AFC0 >= 2) {
        printf(D_80012038, count);
    }

    return 1;
}
