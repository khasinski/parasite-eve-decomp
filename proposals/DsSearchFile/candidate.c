/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"
extern int D_8009B6E0;
extern int D_8009AFC0;
extern char D_80011E6C[], D_80011E88[], D_80011EA0[], D_80011EBC[], D_80011EDC[], D_80011EE8[];
int CdRom_GetDiskType(void);
int DS_newmedia(void);
int DS_searchdir(int parent, char *name);
int DS_cachefile(int parent);
int _cmp(char *, char *);
int printf(const char *, ...);
int puts(const char *);
DslFILE *DsSearchFile(DslFILE *out, char *name) {
    char component[32];
    signed char *path;
    char *cursor;
    int depth, directory;
    int not_found;
    DslFILE *entry;
    if (D_8009B6E0 < CdRom_GetDiskType()) {
        if (!DS_newmedia()) return 0;
        D_8009B6E0 = CdRom_GetDiskType();
    }
    if (*(signed char *)name != '\\') return 0;
    component[0] = 0;
    directory = 1;
    path = (signed char *)name;
    for (depth = 0; depth < 8; depth++) {
        cursor = component;
        while (*path != '\\' && *path) *cursor++ = *path++;
        if (!*path) break;
        path++;
        not_found = -1;
        *cursor = 0;
        directory = DS_searchdir(directory, component);
        if (directory == not_found) { component[0] = 0; break; }
    }
    if (depth >= 8) {
        if (D_8009AFC0 > 0) printf(D_80011E6C, name, depth);
        return 0;
    }
    if (!*(signed char *)component) {
        if (D_8009AFC0 > 0) printf(D_80011E88, name);
        return 0;
    }
    *cursor = 0;
    if (!DS_cachefile(directory)) {
        if (D_8009AFC0 > 0) puts(D_80011EA0);
        return 0;
    }
    if (D_8009AFC0 > 1) printf(D_80011EBC, component);
    for (depth = 0, entry = g_DslFileCache; depth < 64; depth++, entry++) {
        if (!*(signed char *)g_DslFileCache[depth].name) break;
        if (_cmp(entry->name, component)) {
            if (D_8009AFC0 > 1) printf(D_80011EDC, component);
            *out = *entry;
            return entry;
        }
    }
    if (D_8009AFC0 > 0) printf(D_80011EE8, component);
    return 0;
}
