/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "common.h"

#include "pe1/psyq_cd.h"

typedef struct CdlFILE {
    CdlLOC pos;
    u32 size;
    char name[16];
} CdlFILE;

extern int D_8009B6E0;
extern int D_8009AFC0;
extern char D_800A36B8[];
extern char D_80011E6C[];
extern char D_80011E88[];
extern char D_80011EA0[];
extern char D_80011EBC[];
extern char D_80011EDC[];
extern char D_80011EE8[];

int CdRom_GetDiskType(void);
int DS_newmedia(void);
int DS_searchdir(int type, char *name);
int DS_cachefile(int type);
int _cmp(char *a, char *b);
int printf(char *fmt, ...);
int puts(char *s);

CdlFILE *DsSearchFile(CdlFILE *out, char *path) {
    char buf[32];
    char *p;
    char *b;
    int type;
    int n;
    int i;
    CdlFILE *rec;
    char *nm;

    if (D_8009B6E0 < CdRom_GetDiskType()) {
        if (DS_newmedia() == 0) {
            return 0;
        }
        D_8009B6E0 = CdRom_GetDiskType();
    }

    type = 1;
    if (*(s8 *)path != '\\') {
        return 0;
    }

    buf[0] = 0;
    p = path;
    for (n = 0; n < 8; n++) {
        b = buf;
        while (*(s8 *)p != '\\') {
            if (*p == 0) {
                break;
            }
            *b++ = *p++;
        }
        if (*(s8 *)p == 0) {
            break;
        }
        p++;
        *b = 0;
        type = DS_searchdir(type, buf);
        if (type == -1) {
            buf[0] = 0;
            break;
        }
    }

    if (n >= 8) {
        if (D_8009AFC0 > 0) {
            printf(D_80011E6C, path, n);
        }
        return 0;
    }

    if (buf[0] == 0) {
        if (D_8009AFC0 > 0) {
            printf(D_80011E88, path);
        }
        return 0;
    }

    *b = 0;
    if (DS_cachefile(type) == 0) {
        if (D_8009AFC0 > 0) {
            puts(D_80011EA0);
        }
        return 0;
    }

    if (D_8009AFC0 >= 2) {
        printf(D_80011EBC, buf);
    }

    rec = (CdlFILE *)(D_800A36B8 - 8);
    nm = D_800A36B8;
    for (i = 0; i < 64; i++) {
        if (*(s8 *)&D_800A36B8[i * 24] == 0) {
            break;
        }
        if (_cmp(nm, buf)) {
            if (D_8009AFC0 >= 2) {
                printf(D_80011EDC, buf);
            }
            *out = *rec;
            return rec;
        }
        rec++;
        nm += 24;
    }

    if (D_8009AFC0 > 0) {
        printf(D_80011EE8, buf);
    }
    return 0;
}
