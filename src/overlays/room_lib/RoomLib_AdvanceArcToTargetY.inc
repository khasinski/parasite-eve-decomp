extern short D_800966EC[][2];

void RoomLib_AdvanceArcToTargetY_80191D18(RoomEnt *obj) {
    register RoomEnt *self asm("$18") = obj;
    register char *state asm("$16") = (char *)self + 0xC;
    register RoomLink *link asm("$19") = self->link;
    register short *scratch asm("$17") = (short *)0x1F800000;
    int angle;
    int y;

    if (self->t1A == 0) {
        register int count asm("$4");
        register int step asm("$5");
        register int targetY asm("$6");
        register int accel asm("$7");
        self->t1A = 1;
        RW8(self, 0xA2) = 0;
        if (RW32(self, 0x7C) == 0) {
            RW32((void *)0x1F800000, 0x28) = RW32(self, 0x3C);
            RW32((void *)0x1F800000, 0x2C) = RW32(self, 0x40);
            RW32((void *)0x1F800000, 0x30) = RW32(self, 0x44);
        } else {
            RW32((void *)0x1F800000, 0x28) =
                RW32((void *)RW32(self, 0x7C), 0x28);
            RW32((void *)0x1F800000, 0x2C) =
                RW32((void *)RW32(self, 0x7C), 0x2C);
            RW32((void *)0x1F800000, 0x30) =
                RW32((void *)RW32(self, 0x7C), 0x30);
        }

        angle = FieldEng_VecToAngle(link->pos, (int *)&scratch[20]) & 0xFFF;
        {
            int *rotBase = (int *)D_800966EC;
            int *entry = rotBase + angle;
            int hi = *(short *)((char *)entry + 2);
            int lo;
            int savedHi;
            *(volatile short *)&scratch[1] = 0;
            *(volatile short *)&scratch[0] = hi;
            lo = *entry;
            savedHi = RWU16(scratch, 0x0);
            *(volatile short *)&scratch[3] = 0;
            *(volatile short *)&scratch[5] = 0;
            *(volatile short *)&scratch[7] = 0;
            *(volatile short *)&scratch[2] = lo;
            *(volatile short *)&scratch[4] = 0x1000;
            *(volatile short *)&scratch[8] = savedHi;
            *(volatile short *)&scratch[6] = -RWU16(scratch, 0x4);
        }
        scratch[16] = RW32(state, 0x50) >> 12;
        scratch[17] = 0;
        scratch[18] = RW32(state, 0x58) >> 12;
        gte_ldrotmatrix((void *)scratch);
        gte_ldv0((void *)((int)scratch + 0x20));
        gte_mvmva();
        gte_stmac((void *)((int)scratch + 0x38));

        RW32(state, 0x40) = RW32(scratch, 0x28) +
                            (RW32(scratch, 0x38) << 12);
        RW32(state, 0x44) = RW32(scratch, 0x2C) +
                            (RW32(scratch, 0x3C) << 12);
        RW32(state, 0x48) = RW32(scratch, 0x30) +
                            (RW32(scratch, 0x40) << 12);

        count = 0;
        step = RW32(state, 0x7C);
        accel = RW32(state, 0x88);
        targetY = RW32(state, 0x44);
        y = link->pos[1];
        do {
            y += step;
            step += accel;
            count++;
        } while (y < targetY);
        {
            register int value asm("$2");
            register int base asm("$3");
            value = RW32(state, 0x40);
            base = link->pos[0];
            value -= base;
            ROOMLIB_DIV_V0_A0_CHECKED(value, count);
            RVW32(state, 0x80) = value;
            value = RW32(state, 0x48);
            base = link->pos[2];
            value -= base;
            ROOMLIB_DIV_V0_A0_CHECKED(value, count);
            RVW32(state, 0x84) = value;
        }
    }

    RW32(scratch, 0x48) = 0;
    if (RW8(state, 0x96) == 0) {
        switch (((int (*)(RoomEnt *))func_800DFB78)(self)) {
        case 0:
            break;
        case 1:
            RW8(state, 0x96) = 1;
            RW32(state, 0x80) = 0;
            RW32(state, 0x84) = 0;
            if (RW32(state, 0x4) != 0) {
                RW32((void *)RW32(state, 0x4), 0) = 2;
            }
            break;
        case 2:
            return;
        default:
            break;
        }
    }

    if (RW8(link, 0xE) < 2) {
        return;
    }
    link->pos[0] += RW32(state, 0x80);
    link->pos[1] += RW32(state, 0x7C);
    link->pos[2] += RW32(state, 0x84);
    RW32(state, 0x7C) += RW32(state, 0x88);
    if (RW16(link, 0x2E) >= RW16(state, 0x46)) {
        RW32(scratch, 0x48) = 1;
        link->pos[1] = RW32(state, 0x44);
    }
    func_800DFB20(self);
    if (RW32(scratch, 0x48) != 0) {
        RoomLib_ResetAndSignal_801920A0(self);
    }
}
