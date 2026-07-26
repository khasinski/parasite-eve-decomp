typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

#include "pe1/field_engine_state.h"

void *func_800C2B90(void *obj, u8 id, u16 *sizes, int (**handlers)(void *, void *, void *));
int func_800C2E08(void);

int func_800C2758(char *obj, int (**handlers)(void *, void *, void *), u16 *sizes) {
    int result = 0;
    u8 finished = 0;
    u8 stopped = 0;
    s16 timer;

    g_FieldEngineScriptState = obj + 0xC;
    g_FieldEngineSlotTable = (u8 *)(obj + 0x80);
    g_FieldEngineScriptObject = obj;
    g_FieldEngineScriptData = obj + 0x200;
    g_FieldEngineCommandTable = *(int **)(obj + 0x78);

    timer = *(s16 *)(obj + 0xC);
    if (timer != 0) {
        *(s16 *)(obj + 0xC) = timer - 1;
        return result;
    }

    while (!finished) {
        int command = g_FieldEngineCommandTable[*(s16 *)(g_FieldEngineScriptState + 2)];

        if (command == -3) {
            result |= func_800C2E08();
        }

        if (command == -2) {
            command = 0;
        }

        if (command == -1) {
            stopped = 1;
            g_FieldEngineScriptState[7] = 1;
        } else {
            unsigned int high = command >> 16;
            unsigned int op = high;
            u16 lowOp = op;
            int value = command;

            if (lowOp == 1) {
                func_800C2B90(obj, value & 0xFF, sizes, handlers);
            }

            if (lowOp == 2) {
                stopped = 1;
                *(s16 *)g_FieldEngineScriptState = value;
            }

            if (high - 0x10 < 0x10) {
                *(int *)(g_FieldEngineScriptState + (((op - 0x10) & 0xFF) * 4) + 8) = (s16)command;
            }

            if (high - 0x20 < 0x10) {
                char *slot = g_FieldEngineScriptState + (((op - 0x20) & 0xFF) * 4);

                *(int *)(slot + 8) += (s16)command;
            }

            if (high - 0x30 < 0x10) {
                char *slot = g_FieldEngineScriptState + (((op - 0x30) & 0xFF) * 4);

                *(int *)(slot + 8) -= (s16)command;
            }

            if (high - 0x40 < 0x10) {
                *(int *)(g_FieldEngineScriptState + (((op - 0x40) & 0xFF) * 4) + 8) =
                    *(int *)(g_FieldEngineScriptState + ((value & 0xFFFF) * 4) + 0x48);
            }

            if (high - 0x50 < 0x10) {
                *(int *)(g_FieldEngineScriptState + ((value & 0xFFFF) * 4) + 0x48) =
                    *(int *)(g_FieldEngineScriptState + (((op - 0x50) & 0xFF) * 4) + 8);
            }

            if (high - 0x1000 < 0x1000) {
                if (*(int *)(g_FieldEngineScriptState + ((high & 0xF00) >> 6) + 8) == (s16)command) {
                    *(u16 *)(g_FieldEngineScriptState + 2) += (signed char)op;
                }
            }

            if (op - 0x2000 < 0x1000) {
                if (*(int *)(g_FieldEngineScriptState + ((op & 0xF00) >> 6) + 8) != (s16)value) {
                    *(u16 *)(g_FieldEngineScriptState + 2) += (signed char)op;
                }
            }

            if (op - 0x3000 < 0x1000) {
                *(u16 *)(g_FieldEngineScriptState + 2) += (signed char)op;
            }
        }

        if ((signed char)g_FieldEngineScriptState[7] == 0) {
            *(u16 *)(g_FieldEngineScriptState + 2) += 1;
        } else if ((signed char)g_FieldEngineScriptState[6] == 0) {
            result = -1;
        }

        if (stopped) {
            finished = 1;
        }
    }

    return result;
}
