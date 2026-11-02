/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBDS DSFILE.OBJ, part 1 of 2: DsSearchFile, _cmp. */
/* DSFILE is split in two: DsSearchFile and _cmp only match with GCC 2.8.1
 * and -mno-split-addresses, the directory cache in dsfile_2.c only with
 * GCC 2.7.2. */

#include "common.h"
#include "pe1/cdrom.h"
#include "pe1/psyq_bios.h"
#include "pe1/psyq_ds.h"

PE1_STATIC_ASSERT(sizeof(DslFILE) == 24, ds_search_file_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DslFILE, name) == 8, ds_search_file_name_offset);

/* Psy-Q 4.6 DSFILE.OBJ: private diagnostics and persistent media state. */
const char g_DsPathLevelErrorFormat[] =
    "%s: path level (%d) error\n";
const char g_DsDirNotFoundFormat[] =
    "%s: dir was not found\n";
const char g_DsSearchFileDiscErrorText[] =
    "DsSearchFile: disc error\n";
const char g_DsSearchComponentFormat[] =
    "DsSearchFile: searching %s...\n";
const char g_DsComponentFoundFormat[] = "%s:  found\n";
const char g_DsComponentNotFoundFormat[] = "%s: not found\n";
const char g_DsNewMediaPvdReadErrorText[] =
    "DS_newmedia: Read error in ds_read(PVD)\n";
const char g_DsIso9660Magic[] = "CD001";
const char g_DsNewMediaPvdFormatErrorText[] =
    "DS_newmedia: Disc format error in ds_read(PVD)\n";
const char g_DsNewMediaPtReadErrorFormat[] =
    "DS_newmedia: Read error (PT:%08x)\n";
const char g_DsNewMediaSearchingDirText[] =
    "DS_newmedia: sarching dir..\n";
const char g_DsDirectoryEntryFormat[] =
    "\t%08x,%04x,%04x,%s\n";
const char g_DsDirectoryCountFormat[] =
    "DS_newmedia: %d dir entries found\n";
const char g_DsCacheFileDirNotFoundText[] =
    "DS_cachefile: dir not found\n";
const char g_DsCacheFileSearchingText[] =
    "DS_cachefile: searching...\n";
const char g_DsDotName[] = ".";
const char g_DsDotDotName[] = "..";
const char g_DsCacheEntryFormat[] =
    "\t(%02x:%02x:%02x) %8d %s\n";
const char g_DsCacheCountFormat[] =
    "DS_cachefile: %d files found\n";
const u8 dsfile_rodata_padding[4] = {0};

int g_DsCachedDirectory = 0;
int g_DsCachedDiskType = 0;
int dsfile_data_padding[2] = {0, 0};

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
        if (CD_debug > 0) printf(g_DsPathLevelErrorFormat, name, depth);
        return 0;
    }
    if (!*(signed char *)component) {
        if (CD_debug > 0) printf(g_DsDirNotFoundFormat, name);
        return 0;
    }
    *cursor = 0;
    if (!DS_cachefile(directory)) {
        if (CD_debug > 0) puts(g_DsSearchFileDiscErrorText);
        return 0;
    }
    if (CD_debug > 1) printf(g_DsSearchComponentFormat, component);
    depth = 0;
    base = g_DslFileCache[0].name;
    entry = (DslFILE *)(base - 8);
    entry_name = base;
    for (offset = zero; depth < DSL_MAX_FILE;
         entry++, entry_name += sizeof(DslFILE), depth++, offset += sizeof(DslFILE)) {
        if (!((signed char *)g_DslFileCache[0].name)[offset]) break;
        if (_cmp(entry_name, component)) {
            if (CD_debug > 1) printf(g_DsComponentFoundFormat, component);
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
    if (CD_debug > 0) printf(g_DsComponentNotFoundFormat, component);
    return 0;
}

int _cmp(char *s1, char *s2) {
    return strncmp(s1, s2, 0xC) == 0;
}
