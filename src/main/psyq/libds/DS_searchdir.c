/* ASSEMBLER: GNU */
#include "pe1/psyq_cd.h"

int strcmp(const char *, const char *);

/* Psy-Q DSFILE.OBJ: find a directory by its parent ID and name. */
int DS_searchdir(int parent, char *name) {
    int i;

    for (i = 0; i < DSL_MAX_DIR; i++) {
        if (!g_DslDirectoryCache[i].parentDirectoryId) {
            break;
        }
        if (g_DslDirectoryCache[i].parentDirectoryId != parent) {
            continue;
        }
        if (strcmp(name, g_DslDirectoryCache[i].name) == 0) {
            return i + 1;
        }
    }
    return -1;
}
