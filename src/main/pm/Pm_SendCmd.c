/* CC1_PROFILE: build0001 */

typedef unsigned char u8;

extern char *g_PmSlotTable;
extern char *g_PmSlotTable2;
extern int **g_PmCmdHandlerTable;

int Pm_SendCmd(int arg0, int arg1, int arg2, int *arg3, int *arg4, int *arg5) {
    int offset;
    char *entry;
    int cmd;
    int **handler;
    int **table;
    int table_offset;
    int (*callback)(char *, int, int, int *, int *, int *);
    volatile int frame_pad[6];

    if ((unsigned int)arg0 >= 0x16) {
        return -0xA;
    }

    if ((unsigned int)arg0 >= 0xB) {
        int idx;
        int offset_hi;
        idx = arg0 - 0xB;
        offset_hi = idx << 4;
        offset_hi += idx;
        offset_hi <<= 2;
        offset_hi -= idx;
        entry = g_PmSlotTable2 + (offset_hi << 2);
    } else {
        offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
        offset <<= 2;
        entry = g_PmSlotTable + offset;
    }

    cmd = *(u8 *)(entry + 1);
    if ((unsigned int)cmd >= 0xC0) {
        return -0xB;
    }
    if ((unsigned int)cmd >= 0x55) {
        cmd = 0x55;
    }

    table = g_PmCmdHandlerTable;
    table_offset = cmd << 2;
    handler = *(int ***)(table_offset + (int)table);
    if (handler == 0) {
        return -0xC;
    }
    callback = (int (*)(char *, int, int, int *, int *, int *))handler[2];
    if (callback == 0) {
        return -1;
    }

    if ((arg1 == 1) && (arg2 == 0)) {
        *arg3 = *(u8 *)(entry + 2);
        *arg4 = *(u8 *)(entry + 3);
        *arg5 = *(int *)(entry + 4);
    }

    {
        char *call_entry = entry;
        int call_arg1 = arg1;
        int call_arg2 = arg2;
        int *call_arg3 = arg3;
        int call_cmd = cmd;
        int *call_arg4 = arg4;
        register int *call_arg5 asm("$10") = arg5;
        int result;

        asm volatile(
            "lui $3,%%hi(g_PmCmdHandlerTable)\n"
            "lw $3,%%lo(g_PmCmdHandlerTable)($3)\n"
            "sll $2,$8,2\n"
            "addu $2,$2,$3\n"
            "lw $2,0($2)\n"
            "sw $9,0x10($sp)\n"
            "sw $10,0x14($sp)\n"
            "lw $2,8($2)\n"
            "nop\n"
            "jalr $2\n"
            "nop"
            : "=r"(result)
            : "r"(call_entry), "r"(call_arg1), "r"(call_arg2), "r"(call_arg3), "r"(call_cmd), "r"(call_arg4), "r"(call_arg5)
            : "$3", "$31", "memory");
        return result;
    }
}
