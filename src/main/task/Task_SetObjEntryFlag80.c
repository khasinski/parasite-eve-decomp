/* CC1_FLAGS: -fno-schedule-insns */
extern int g_CollisionPlaneTable;
extern char *g_CollisionDb;

int Task_SetObjEntryFlag80(int **arg0) {
    char *entry;
    char *base_entry;

    if (g_CollisionPlaneTable == 0) {
        register int index asm("$2");
        char *base;
        int *arg = arg0[0];

        index = *arg;
        base = g_CollisionDb;
        entry = (char *)(index * 11);
        base_entry = *(char **)(base + 0x1C);
        /* Keep the base load branch-local while allowing the final shift into the jump delay slot. */
        entry = (char *)((int)entry << 1);
    } else {
        register int index asm("$2");
        char *base;
        int *arg = arg0[0];

        index = *arg;
        base = g_CollisionDb;
        entry = (char *)(index * 7);
        base_entry = *(char **)(base + 0x1C);
        /* Keep the base load branch-local without materializing the add before the join. */
        asm volatile("" : : "r"(base_entry));
        entry = (char *)((int)entry << 2);
    }
    /* GCC prints addu operands in RTL order; tie entry as operand 0 to match retail bytes. */
    asm volatile("addu %0,%0,%1" : "=r"(entry) : "r"(base_entry), "0"(entry));
    *entry |= 0x80;
    return 1;
}
