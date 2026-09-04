#ifndef ROOMLIB_SIXTEEN_STATE_FUNC
#error "ROOMLIB_SIXTEEN_STATE_FUNC must name the room entry point"
#endif

extern char *D_800F32D0;
extern char *D_800E2368;
extern s16 ROOMLIB_SIXTEEN_FRAME;
extern int ROOMLIB_SIXTEEN_CFG0;
extern int ROOMLIB_SIXTEEN_CFG1;
extern int ROOMLIB_SIXTEEN_CFG2;
extern int ROOMLIB_SIXTEEN_CFG3;
extern int ROOMLIB_SIXTEEN_CFG4;
extern int ROOMLIB_SIXTEEN_CFG5;
extern int ROOMLIB_SIXTEEN_CFG6;
extern int ROOMLIB_SIXTEEN_CFG7;

int func_8003010C(void *object, int id);

int ROOMLIB_SIXTEEN_STATE_FUNC(int mode, char *state) {
    char *slot;
    register char *workState asm("$16");
    register unsigned char *phaseState asm("$4");
    unsigned char *completionState;
    register int frame asm("$3");
    unsigned int flags;
    register int activity asm("$2");
    register int motion asm("$4");
    register unsigned short phase asm("$2");
    register unsigned short phaseCopy asm("$3");
    register int reversedMotion asm("$2");
    register int lastConfig asm("$2");

    slot = *(char **)(D_800F32D0 + 8);
    workState = state;
    if (mode == 0) {
        *(char **)(D_800E2368 + 0x14) = workState;
        **(unsigned int **)slot |= 0x40000000;
        frame = ROOMLIB_SIXTEEN_FRAME;
        *(s16 *)(workState + 0x32) = 0x1A;
        *(s16 *)(workState + 0x30) = 0;
        *(s16 *)(workState + 0x34) = 0;
        *(u8 *)(workState + 0x3A) = 0;
        *(u8 *)(workState + 0x3B) = 0;
        *(int *)(workState + 0x28) = -1;
        *(int *)(workState + 0x0C) = frame;
        *(int *)(workState + 0x10) = ROOMLIB_SIXTEEN_CFG0;
        *(int *)(workState + 0x1C) = ROOMLIB_SIXTEEN_CFG1;
        *(int *)(workState + 0x20) = ROOMLIB_SIXTEEN_CFG2;
        *(int *)(workState + 0x24) = ROOMLIB_SIXTEEN_CFG3;
        *(int *)(workState + 0x14) = ROOMLIB_SIXTEEN_CFG4;
        *(int *)(workState + 0x18) = ROOMLIB_SIXTEEN_CFG5;
        *(s16 *)(workState + 0x36) = ROOMLIB_SIXTEEN_CFG6;
        lastConfig = ROOMLIB_SIXTEEN_CFG7;
        *(s16 *)(workState + 0x38) = lastConfig;
        return 0;
    }
    if (mode != 1) {
        return 0;
    }

    if (*(u8 *)(workState + 0x3A) == 0) {
        if (*(void **)slot == 0 || func_8003010C(slot, 0x2C) <= 0) {
            *(u8 *)(workState + 0x3A) = 1;
        } else {
            flags = **(unsigned int **)slot;
            if ((flags & 0x1800) != 0) {
                *(u8 *)(workState + 0x3A) = 1;
            } else {
                activity = (flags >> 1) & 7;
                if (activity > 0) {
                    *(u8 *)(workState + 0x3A) = 1;
                }
            }
        }
    }

    if (*(s16 *)(workState + 0x2E) == 0 &&
        *(u8 *)(workState + 0x3B) != 0) {
        completionState = *(unsigned char **)(*(char **)slot + 0x18);
        if (*completionState == 0) {
            *(u8 *)(workState + 0x3A) = 1;
        }
    }

    if (*(u8 *)(workState + 0x3A) != 0) {
        if (*(s16 *)(workState + 0x2E) != 0) {
            return *(s16 *)(workState + 0x30) != 0;
        }
        *(s16 *)(workState + 0x2E) = -1;
        return 1;
    }

    phaseState = *(unsigned char **)(*(char **)slot + 0x18);
    if (*phaseState == 1) {
        *phaseState = 2;
        *(u8 *)(workState + 0x3B) = 1;
    }
    if (*(s16 *)(workState + 0x30) != 0) {
        *(s16 *)(workState + 0x30) += 1;
    }
    if (*(u8 *)(slot + 0xE) != 9) {
        return 0;
    }

    phase = *(u16 *)(slot + 0x16);
    PE1_COMPILER_USE(phase);
    motion = *(int *)(slot + 0x1C);
    if (motion >= 0) {
        asm("" : "=r"(phaseCopy) : "0"(phase));
        if ((s16)phase >= 0x1C) {
            if (*(s16 *)(workState + 0x30) < 0x18) {
                reversedMotion = -motion;
                goto store_reversed_motion;
            }
        }
        if ((s16)phaseCopy < *(u8 *)(slot + 0xF) - 1) {
            return 0;
        }
        phaseState = *(unsigned char **)(*(char **)slot + 0x18);
        if (*phaseState == 2) {
            *phaseState = 4;
        }
        return 1;
    }
    if ((s16)phase >= 0x17) {
        return 0;
    }
    reversedMotion = -motion;
store_reversed_motion:
    *(int *)(slot + 0x1C) = reversedMotion;
    return 0;
}

#undef ROOMLIB_SIXTEEN_STATE_FUNC
#undef ROOMLIB_SIXTEEN_FRAME
#undef ROOMLIB_SIXTEEN_CFG0
#undef ROOMLIB_SIXTEEN_CFG1
#undef ROOMLIB_SIXTEEN_CFG2
#undef ROOMLIB_SIXTEEN_CFG3
#undef ROOMLIB_SIXTEEN_CFG4
#undef ROOMLIB_SIXTEEN_CFG5
#undef ROOMLIB_SIXTEEN_CFG6
#undef ROOMLIB_SIXTEEN_CFG7
