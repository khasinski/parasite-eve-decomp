/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef short s16;

extern int g_SceneDataTable0;
extern int *g_TaskNodePool;

int Tbl_LookupEntry(s16 arg0);

int Task_YieldWhileTableEntrySet(s16 **arg0) {
    if ((Tbl_LookupEntry(*arg0[0]) << 24) != 0) {
        g_SceneDataTable0 -= 0xC;
        g_TaskNodePool[4] = 1;
        return 0;
    }
    return 1;
}
