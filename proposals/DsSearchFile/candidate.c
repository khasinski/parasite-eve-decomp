/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"
PE1_STATIC_ASSERT(sizeof(DslFILE) == 24, ds_search_file_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslFILE, name) == 8, ds_search_file_name_offset);

extern int D_8009B6E0, D_8009AFC0;
extern char D_80011E6C[], D_80011E88[], D_80011EA0[], D_80011EBC[], D_80011EDC[], D_80011EE8[];
extern char D_800A36B8[DSL_MAX_FILE][sizeof(DslFILE)];
int CdRom_GetDiskType(void);
int DS_newmedia(void);
int DS_searchdir(int, char *);
int DS_cachefile(int);
int _cmp(char *, char *);
int printf(const char *, ...);
int puts(const char *);
DslFILE *DsSearchFile(DslFILE *output, char *input_name) {
    register DslFILE *out asm("$22") = output;
    char *name = input_name;
    char component[32];
    signed char *path;
    int separator;
    char *cursor;
    int depth, directory;
    int not_found;
    register DslFILE *entry asm("$16");
    register int offset;
    register char *base asm("$2");
    register int zero asm("$0");
    register char *entry_name;
    /* $zero supplies the independent cache offset without a redundant move. */
    asm volatile("" : "=r"(zero) : "r"(out));
    if (D_8009B6E0 < CdRom_GetDiskType()) {
        if (!DS_newmedia()) return 0;
        D_8009B6E0 = CdRom_GetDiskType();
    }
    if (*(signed char *)name != '\\') return 0;
    component[0] = 0;
    directory = 1;
    path = (signed char *)name;
    for (depth = 0; depth < 8; depth++) {
        separator = '\\';
        cursor = component;
        not_found = -1;
        if (*path != separator) {
            while (*path != '\\' && *path) *cursor++ = *path++;
        }
        if (!*path) break;
        path++;
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
    depth = 0;
    base = D_800A36B8[0];
    asm volatile("" : "+r"(base));
    entry = (DslFILE *)(base - 8);
    entry_name = base;
    for (offset = zero; depth < DSL_MAX_FILE;
         entry++, entry_name += sizeof(DslFILE), depth++, offset += sizeof(DslFILE)) {
        asm volatile("" : "+r"(offset));
        if (!((signed char *)D_800A36B8)[offset]) break;
        if (_cmp(entry_name, component)) {
            if (D_8009AFC0 > 1) printf(D_80011EDC, component);
            /* Copy the aligned 24-byte record in the retail four-word/two-word order. */
            {
                register u32 word0 asm("$2") = ((u32 *)entry)[0];
                register u32 word1 = ((u32 *)entry)[1];
                register u32 word2 asm("$4") = ((u32 *)entry)[2];
                register u32 word3 asm("$5") = ((u32 *)entry)[3];
                asm volatile("" : "+r"(word0), "+r"(word1), "+r"(word2), "+r"(word3));
                ((u32 *)out)[0] = word0;
                ((u32 *)out)[1] = word1;
                ((u32 *)out)[2] = word2;
                ((u32 *)out)[3] = word3;
                word0 = ((u32 *)entry)[4];
                word1 = ((u32 *)entry)[5];
                asm volatile("" : "+r"(word0), "+r"(word1));
                ((u32 *)out)[4] = word0;
                ((u32 *)out)[5] = word1;
            }
            asm volatile("" : : "m"(*out));
            return entry;
        }
        asm volatile("" : : "r"(depth));
    }
    if (D_8009AFC0 > 0) printf(D_80011EE8, component);
    return 0;
}
