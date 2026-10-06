/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBDS DSFILE.OBJ, part 1 of 2: DsSearchFile, _cmp. */
/* DSFILE is split in two: DsSearchFile and _cmp only match with GCC 2.8.1
 * and -mno-split-addresses, the directory cache in dsfile_2.c only with
 * GCC 2.7.2. */
#include "pe1/psyq_ds.h"
#include "pe1/cdrom.h"
#include "common.h"

PE1_STATIC_ASSERT(sizeof(DslFILE) == 24, ds_search_file_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslFILE, name) == 8, ds_search_file_name_offset);

/* Psy-Q 4.6 DSFILE.OBJ: private diagnostics and persistent media state. */
const char D_80011E6C[] =
    "%s: path level (%d) error\n";
const char D_80011E88[] =
    "%s: dir was not found\n";
const char D_80011EA0[] =
    "DsSearchFile: disc error\n";
const char D_80011EBC[] =
    "DsSearchFile: searching %s...\n";
const char D_80011EDC[] = "%s:  found\n";
const char D_80011EE8[] = "%s: not found\n";
const char D_80011EF8[] =
    "DS_newmedia: Read error in ds_read(PVD)\n";
const char D_80011F24[] = "CD001";
const char D_80011F2C[] =
    "DS_newmedia: Disc format error in ds_read(PVD)\n";
const char D_80011F5C[] =
    "DS_newmedia: Read error (PT:%08x)\n";
const char D_80011F80[] =
    "DS_newmedia: sarching dir..\n";
const char D_80011FA0[] =
    "\t%08x,%04x,%04x,%s\n";
const char D_80011FB4[] =
    "DS_newmedia: %d dir entries found\n";
const char D_80011FD8[] =
    "DS_cachefile: dir not found\n";
const char D_80011FF8[] =
    "DS_cachefile: searching...\n";
const char D_80012014[] = ".";
const char D_80012018[] = "..";
const char D_8001201C[] =
    "\t(%02x:%02x:%02x) %8d %s\n";
const char D_80012038[] =
    "DS_cachefile: %d files found\n";
const u8 dsfile_rodata_padding[4] = {0};

int D_8009B6DC = 0;
int D_8009B6E0 = 0;
int dsfile_data_padding[2] = {0, 0};

extern char D_800A36B8[DSL_MAX_FILE][sizeof(DslFILE)];
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
    char *base;
    register int zero asm("$0");
    register char *entry_name;
    /* $zero supplies the independent cache offset without a redundant move. */
        if (g_DsCachedDiskType < DsShellOpen()) {
        if (!DS_newmedia()) return 0;
        g_DsCachedDiskType = DsShellOpen();
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
    entry = (DslFILE *)(base - 8);
    entry_name = base;
    for (offset = zero; depth < DSL_MAX_FILE;
         entry++, entry_name += sizeof(DslFILE), depth++, offset += sizeof(DslFILE)) {
        if (!((signed char *)D_800A36B8)[offset]) break;
        if (_cmp(entry_name, component)) {
            if (D_8009AFC0 > 1) printf(D_80011EDC, component);
            /* Copy the aligned 24-byte record in the retail four-word/two-word order. */
            {
                u32 word0 = ((u32 *)entry)[0];
                register u32 word1 = ((u32 *)entry)[1];
                register u32 word2 asm("$4") = ((u32 *)entry)[2];
                register u32 word3 asm("$5") = ((u32 *)entry)[3];
                ((u32 *)out)[0] = word0;
                ((u32 *)out)[1] = word1;
                ((u32 *)out)[2] = word2;
                ((u32 *)out)[3] = word3;
                word0 = ((u32 *)entry)[4];
                word1 = ((u32 *)entry)[5];
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


int _cmp(char *s1, char *s2) {
    return strncmp(s1, s2, 0xC) == 0;
}
