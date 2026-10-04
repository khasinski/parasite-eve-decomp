#include "common.h"
#include "pe1/field_engine_state.h"

void *func_800C2B90(void *obj, u8 id, u16 *sizes, int (**handlers)(void *, void *, void *));
int func_800C2E08(void);

#define SCRIPT ((FieldEngState *)g_FieldEngineScriptState)

/* Runs the field script of obj until a wait or end command. */
int func_800C2758(FieldEngObject *obj, int (**handlers)(void *, void *, void *), u16 *sizes) {
    u8 stopped = 0;
    int result = 0;
    s16 wait;

    g_FieldEngineScriptState = (char *)&obj->state;
    g_FieldEngineSlotTable = (u8 *)obj->slots;
    g_FieldEngineScriptObject = (char *)obj;
    g_FieldEngineScriptData = (char *)obj->data;
    g_FieldEngineCommandTable = obj->state.commands;

    wait = obj->state.wait;
    if (wait == 0) {
        do {
            int command = g_FieldEngineCommandTable[SCRIPT->pc];

            if (command == -3) {
                result |= func_800C2E08();
            }
            if (command == -2) {
                command = 0;
            }
            if (command != -1) {
                int high = (unsigned int)command >> 16;
                u16 op = high;
                    s16 operand = command;
                u8 index;

                if (op == 1) {
                    func_800C2B90(obj, (u8)operand, sizes, handlers);
                }
                if (op == 2) {
                    stopped = 1;
                    SCRIPT->wait = operand;
                }
                if ((unsigned int)(high - 0x10) < 0x10) {
                    index = op - 0x10;
                    SCRIPT->regs[index] = (s16)command;
                }
                if ((unsigned int)(high - 0x20) < 0x10) {
                    index = op - 0x20;
                    SCRIPT->regs[index] += (s16)command;
                }
                if ((unsigned int)(high - 0x30) < 0x10) {
                    index = op - 0x30;
                    SCRIPT->regs[index] -= (s16)command;
                }
                if ((unsigned int)(high - 0x40) < 0x10) {
                    index = op - 0x40;
                    SCRIPT->regs[index] = SCRIPT->vars[(u16)operand];
                }
                if ((unsigned int)(high - 0x50) < 0x10) {
                    index = op - 0x50;
                    SCRIPT->vars[(u16)operand] = SCRIPT->regs[index];
                }
                if ((unsigned int)(high - 0x1000) < 0x1000) {
                    s8 jump = op;

                    if (SCRIPT->regs[(high & 0xF00) >> 8] == (s16)command) {
                        SCRIPT->pc += jump;
                    }
                }
                if ((unsigned int)(op - 0x2000) < 0x1000) {
                    s8 jump = op;

                    if (SCRIPT->regs[(op & 0xF00) >> 8] != (s16)operand) {
                        SCRIPT->pc += jump;
                    }
                }
                if ((unsigned int)(op - 0x3000) < 0x1000) {
                    SCRIPT->pc += (s8)op;
                }
            } else {
                SCRIPT->halted = 1;
                stopped = 1;
            }

            if (SCRIPT->halted == 0) {
                SCRIPT->pc++;
            } else if (SCRIPT->keep_alive == 0) {
                result = -1;
            }
        } while (!stopped);
    } else {
        obj->state.wait = wait - 1;
    }

    return result;
}

int FieldEng_Spawn6(char *base, int unused, int index, int value) {
    base += 0xC;
    g_FieldEngineScriptState = base;
    *(int *)(base + index * 4 + 0x48) = value;
    return 0;
}
extern char *D_800E2248;

void *func_800C2B10(int index) {
    int offset;
    char *base;

    offset = index << 2;
    base = D_800E2248;
    offset += 8;
    return base + offset;
}
extern char *D_800E2248;

void *func_800C2B28(int index) {
    int offset;
    char *base;

    offset = index << 2;
    base = D_800E2248;
    offset += 0x48;
    return base + offset;
}

void func_800C2B40(void *context) {
    ((FieldEngState *)g_FieldEngineScriptState)->current_context = context;
}

void *func_800C2B50(void) {
    return ((FieldEngState *)g_FieldEngineScriptState)->current_context;
}

int func_800C2B68(void) {
    return ((*(unsigned int *)(g_FieldEngineScriptState + 4) & 0xFFFF0000U) ^ 0x01010000U) < 1;
}

int FieldEng_GetStatus(void *obj);
void func_800C2D0C(u16 slot, u8 id, u16 size);

extern u8 *D_800F34F4;
extern char *D_800E2248;
extern char *D_800F3330;

void *func_800C2B90(void *obj, u8 id, u16 *sizes, int (**handlers)(void *, void *, void *)) {
    s16 slot = -1;
    s16 i;
    FieldEngSlot *base;
    void *entry;
    void *data;
    int (*handler)(void *, void *, void *);

    base = (FieldEngSlot *)D_800F34F4;
    for (i = 0; i < 0x40; i++) {
        if (base[i].flag == 0) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        ((FieldEngState *)D_800E2248)->regs[14] = 1;
        return 0;
    }

    if (FieldEng_GetStatus(obj) == 3 || FieldEng_GetStatus(obj) == 4 || FieldEng_GetStatus(obj) == 5) {
        func_800C2D0C(slot, id, sizes[id]);

        entry = D_800F34F4 + slot * 6;
        data = D_800F3330 + ((FieldEngSlot *)entry)->data_offset;
        handler = handlers[id];
        if (handler != (void *)-1) {
            handler(obj, entry, data);
        }
        return data;
    }

    return 0;
}
