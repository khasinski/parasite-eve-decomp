/* GCC_VERSION: 2.8.1 */
#include "common.h"

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

int strncmp(char *s1, char *s2, int n);

int _cmp(char *s1, char *s2) {
    return strncmp(s1, s2, 0xC) == 0;
}
