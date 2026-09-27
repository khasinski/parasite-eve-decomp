#include "pe1/pm.h"
extern PmCommand **g_PmCmdHandlerTable;

int Pm_SendCmd(int arg0, int arg1, int arg2, int *arg3, int *arg4, int *arg5) {
    int offset;
    char *entry;
    int cmd;
    PmCommand *handler;
    PmCommand **table;
    int table_offset;
    PmSendCallback callback;

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
        entry = (char *)g_PmSlotTable2 + (offset_hi << 2);
    } else {
        offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
        offset <<= 2;
        entry = (char *)g_PmSlotTable + offset;
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
    handler = *(PmCommand **)(table_offset + (int)table);
    if (handler == 0) {
        return -0xC;
    }
    callback = handler->send;
    if (callback == 0) {
        return -1;
    }

    if ((arg1 == 1) && (arg2 == 0)) {
        *arg3 = *(u8 *)(entry + 2);
        *arg4 = *(u8 *)(entry + 3);
        *arg5 = *(int *)(entry + 4);
    }

    {
        register PmCommand **reload_table asm("$3");
        int reload_offset;
        reload_table = g_PmCmdHandlerTable;
        /* Match debt: preserve the table-load scheduling and operand order. */
        asm volatile("" : "=r"(cmd) : "0"(cmd) : "memory");
        reload_offset = cmd << 2;
        asm volatile("" : "=r"(reload_table), "=r"(reload_offset) : "0"(reload_table), "1"(reload_offset));
        handler = *(PmCommand **)((u32)reload_offset + (u32)reload_table);
        /* The table can change through arg3..arg5, so reload before calling. */
        return handler->send((PmSlotHeader *)entry, arg1, arg2, arg3, arg4, arg5);
    }
}

int Pm_SetGetState(int arg0, int arg1, int arg2) {
    int offset;
    int cmd;
    PmCommand *handler;

    if ((unsigned int)arg0 >= 0x16) {
        return -0xD;
    }

    if ((unsigned int)arg0 >= 0xB) {
        int idx;
        int offset_hi;
        idx = arg0 - 0xB;
        offset_hi = idx << 4;
        offset_hi += idx;
        offset_hi <<= 2;
        offset_hi -= idx;
        arg0 = (int)((char *)g_PmSlotTable2 + (offset_hi << 2));
    } else {
        offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
        offset <<= 2;
        arg0 = (int)((char *)g_PmSlotTable + offset);
    }

    cmd = *(u8 *)(arg0 + 1);
    if ((unsigned int)cmd >= 0x55) {
        cmd = 0x55;
    }

    handler = g_PmCmdHandlerTable[cmd];
    if (handler == 0) {
        return -0xF;
    }

    if (arg1 == 0) {
        if ((unsigned int)arg2 < 6) {
            *(u8 *)arg0 = arg2;
        }
    } else {
        *(int *)arg2 = *(u8 *)arg0;
    }

    return *(u8 *)arg0;
}

int Pm_Start(int arg0) {
    int offset;
    register int cmd asm("$5");
    PmCommand *handler;
    int (*callback)(void);
    PmCommand **table;
    register int table_offset asm("$2");

    if ((unsigned int)arg0 >= 0x16) {
        return -0x10;
    }

    if ((unsigned int)arg0 >= 0xB) {
        int idx;
        int offset_hi;
        idx = arg0 - 0xB;
        offset_hi = idx << 4;
        offset_hi += idx;
        offset_hi <<= 2;
        offset_hi -= idx;
        arg0 = (int)((char *)g_PmSlotTable2 + (offset_hi << 2));
    } else {
        offset = (((((arg0 * 4) + arg0) << 5) + arg0) << 2) - arg0;
        offset <<= 2;
        arg0 = (int)((char *)g_PmSlotTable + offset);
    }

    if ((unsigned int)(*(u8 *)arg0 - 1) >= 2U) {
        return 0;
    }

    cmd = *(u8 *)(arg0 + 1);
    if ((unsigned int)cmd >= 0xC0) {
        return -0x11;
    }
    if ((unsigned int)cmd >= 0x55) {
        cmd = 0x55;
    }

    table = g_PmCmdHandlerTable;
    table_offset = cmd << 2;
    handler = *(PmCommand **)(table_offset + (int)table);
    if (handler == 0) {
        return -0x12;
    }
    callback = handler->start;
    if (callback != 0) {
        return callback();
    }

    return -1;
}
