#include "room_lib.h"

extern short D_800966EC[][2];

void func_8018FC68(char *state, char *work) {
    int angle;
    int hi;
    int lo;
    int savedHi;

    if (RW32(state, 0x80) != 0) {
        RW32(state, 0x40) = RW32((void *)RW32(state, 0x80), 0x28);
        RW32(state, 0x44) = RW32((void *)RW32(state, 0x80), 0x2C);
        RW32(state, 0x48) = RW32((void *)RW32(state, 0x80), 0x30);
    }

    if (RW32(state, 0x4C) != 0) {
        ROOMLIB_ROT_ENTRY_DECL;
        short localY;
        angle = FieldEng_VecToAngle(RWPTR(state, 0x50),
                                    RWPTR(state, 0x40)) & 0xFFF;
        base = (int *)D_800966EC;
        entry = base + angle;
        hi = RW16(entry, 2);
        RVW16(work, 0x2A) = 0;
        RVW16(work, 0x28) = hi;
        lo = *entry;
        savedHi = RWU16(work, 0x28);
        RVW16(work, 0x2E) = 0;
        RVW16(work, 0x32) = 0;
        RVW16(work, 0x36) = 0;
        RVW16(work, 0x00) = 0;
        RVW16(work, 0x02) = 0;
        RVW16(work, 0x2C) = lo;
        RVW16(work, 0x30) = 0x1000;
        RVW16(work, 0x38) = savedHi;
        RVW16(work, 0x34) = -RWU16(work, 0x2C);
        ROOMLIB_LOAD_S16(localY, state + 0x4E);
        RVW16(work, 0x04) = localY;
        gte_ldrotmatrix(work + 0x28);
        gte_ldv0(work);
        gte_mvmva();
        gte_stmac(work + 0x08);
        RW32(state, 0x40) = RW32(state, 0x50) + (RW32(work, 0x08) << 16);
        RW32(state, 0x48) = RW32(state, 0x58) + (RW32(work, 0x10) << 16);
    }

    {
        ROOMLIB_ROT_ENTRY_DECL;
        angle = FieldEng_VecToAngle(RWPTR(state, 0x50),
                                    RWPTR(state, 0x40)) & 0xFFF;
        base = (int *)D_800966EC;
        entry = base + angle;
        hi = RW16(entry, 2);
        RVW16(work, 0x2A) = 0;
        RVW16(work, 0x28) = hi;
        lo = *entry;
        savedHi = RWU16(work, 0x28);
        RVW16(work, 0x2E) = 0;
        RVW16(work, 0x32) = 0;
        RVW16(work, 0x36) = 0;
        RVW16(work, 0x2C) = lo;
        RVW16(work, 0x30) = 0x1000;
        RVW16(work, 0x38) = savedHi;
        RVW16(work, 0x34) = -RWU16(work, 0x2C);
        gte_ldrotmatrix(work + 0x28);
        gte_ldv0(state + 0x70);
        gte_mvmva();
        gte_stmac(work + 0x08);
        RW32(state, 0x40) += RW32(work, 0x08) << 16;
        RW32(state, 0x44) += RW32(work, 0x0C) << 16;
        RW32(state, 0x48) += RW32(work, 0x10) << 16;
    }
}
