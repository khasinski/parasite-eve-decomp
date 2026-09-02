extern int *func_800C2B10(int index);
extern int *func_800C2B28(int index);
extern int func_800C2B68(void);
extern void func_800C2B90(int owner, int command, void *config, void *state);

extern char D_80192B5C;
extern char D_80192BBC;

#define ROOMLIB_EFFECT_CALL(command) \
    func_800C2B90(owner, (command), &D_80192BBC, &D_80192B5C)
#define ROOMLIB_EFFECT_SET(index, value, command) \
    *func_800C2B10(index) = (value); \
    ROOMLIB_EFFECT_CALL(command)

void ROOMLIB_CONFIGURE_EFFECT_SEQUENCE_NAME(int owner, void *signal)
{
    short counter;
    int mode3;
    int mode4;

    if (*func_800C2B28(0) == 0) {
        if (RW16(signal, 2) == 0) {
            *func_800C2B10(1) = 0;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        counter = RW16(signal, 2);
        if (counter == 10) {
            *func_800C2B10(1) = counter;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 20) {
            *func_800C2B10(1) = 8;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 30) {
            *func_800C2B10(1) = 15;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 40) {
            *func_800C2B10(1) = 4;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 50) {
            *func_800C2B10(1) = 18;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 60) {
            *func_800C2B10(1) = 2;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 70) {
            *func_800C2B10(1) = 9;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 80) {
            *func_800C2B10(1) = 3;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 90) {
            *func_800C2B10(1) = 19;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 100) {
            *func_800C2B10(1) = 20;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 110) {
            *func_800C2B10(1) = 22;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 120) {
            *func_800C2B10(1) = 12;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
        if (RW16(signal, 2) == 130) {
            *func_800C2B10(1) = 17;
            ROOMLIB_EFFECT_SET(2, -1, 1);
        }
    }

    if (*func_800C2B28(0) == 1) {
        if (RW16(signal, 2) == 0) {
            *func_800C2B10(1) = 5;
            ROOMLIB_EFFECT_SET(2, 80, 1);
        }
        if (RW16(signal, 2) == 3) {
            *func_800C2B10(1) = 6;
            ROOMLIB_EFFECT_SET(2, 80, 1);
        }
        if (RW16(signal, 2) == 6) {
            *func_800C2B10(1) = 5;
            ROOMLIB_EFFECT_SET(2, 40, 1);
        }
        if (RW16(signal, 2) == 9) {
            *func_800C2B10(1) = 6;
            ROOMLIB_EFFECT_SET(2, 40, 1);
        }
        if (RW16(signal, 2) == 12) {
            *func_800C2B10(1) = 5;
            ROOMLIB_EFFECT_SET(2, 20, 1);
        }
        if (RW16(signal, 2) == 15) {
            *func_800C2B10(1) = 6;
            ROOMLIB_EFFECT_SET(2, 20, 1);
        }
    }

    if (*func_800C2B28(0) == 2 && RW16(signal, 2) == 0) {
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(5);
        ROOMLIB_EFFECT_SET(1, 800, 4);
        ROOMLIB_EFFECT_SET(1, 700, 4);
        ROOMLIB_EFFECT_SET(1, 600, 4);
        ROOMLIB_EFFECT_SET(1, 500, 4);
        ROOMLIB_EFFECT_SET(1, 300, 4);
        ROOMLIB_EFFECT_SET(1, 300, 4);
        ROOMLIB_EFFECT_SET(1, 200, 4);
        ROOMLIB_EFFECT_SET(1, 100, 4);
    }

    if (*func_800C2B28(0) == 5 && RW16(signal, 2) == 0) {
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(6);
        ROOMLIB_EFFECT_CALL(5);
        ROOMLIB_EFFECT_SET(1, 1000, 4);
        ROOMLIB_EFFECT_SET(1, 800, 4);
        ROOMLIB_EFFECT_SET(1, 600, 4);
        ROOMLIB_EFFECT_SET(1, 500, 4);
        ROOMLIB_EFFECT_SET(1, 400, 4);
        ROOMLIB_EFFECT_SET(1, 300, 4);
        ROOMLIB_EFFECT_SET(1, 200, 4);
        ROOMLIB_EFFECT_SET(1, 100, 4);
    }

    mode3 = *func_800C2B28(0);
    if (mode3 == 3 && RW16(signal, 2) == 0) {
        *func_800C2B10(2) = -1;
        ROOMLIB_EFFECT_SET(1, 1, 3);
        ROOMLIB_EFFECT_SET(1, mode3, 3);
        ROOMLIB_EFFECT_SET(1, 5, 3);
        ROOMLIB_EFFECT_SET(1, 7, 3);
        ROOMLIB_EFFECT_SET(1, 9, 3);
        ROOMLIB_EFFECT_SET(1, 11, 3);
        ROOMLIB_EFFECT_SET(1, 13, 3);
        ROOMLIB_EFFECT_SET(1, 15, 3);
        ROOMLIB_EFFECT_SET(1, 17, 3);
        ROOMLIB_EFFECT_SET(1, 19, 3);
        ROOMLIB_EFFECT_SET(1, 21, 3);
        ROOMLIB_EFFECT_SET(1, 0, 3);
        ROOMLIB_EFFECT_SET(1, 2, 3);
        ROOMLIB_EFFECT_SET(1, 4, 3);
        ROOMLIB_EFFECT_SET(1, 6, 3);
        ROOMLIB_EFFECT_SET(1, 8, 3);
    }

    mode4 = *func_800C2B28(0);
    if (mode4 == 4 && RW16(signal, 2) == 0) {
        *func_800C2B10(2) = -1;
        ROOMLIB_EFFECT_SET(1, 2, 2);
        ROOMLIB_EFFECT_SET(1, mode4, 2);
        ROOMLIB_EFFECT_SET(1, 6, 2);
        ROOMLIB_EFFECT_SET(1, 8, 2);
        ROOMLIB_EFFECT_SET(1, 10, 2);
        ROOMLIB_EFFECT_SET(1, 12, 2);
        ROOMLIB_EFFECT_SET(1, 14, 2);
        ROOMLIB_EFFECT_SET(1, 16, 2);
        ROOMLIB_EFFECT_SET(1, 17, 2);
        ROOMLIB_EFFECT_SET(1, 18, 2);
        ROOMLIB_EFFECT_SET(1, 20, 2);
        ROOMLIB_EFFECT_SET(1, 22, 2);
        ROOMLIB_EFFECT_SET(1, mode4, 2);
        ROOMLIB_EFFECT_SET(1, 6, 2);
        ROOMLIB_EFFECT_SET(1, 8, 2);
        ROOMLIB_EFFECT_SET(1, 10, 2);
        ROOMLIB_EFFECT_SET(1, 12, 2);
        ROOMLIB_EFFECT_SET(1, 14, 2);
        ROOMLIB_EFFECT_SET(1, 16, 2);
        ROOMLIB_EFFECT_SET(1, 17, 2);
        ROOMLIB_EFFECT_SET(1, 18, 2);
        ROOMLIB_EFFECT_SET(1, 20, 2);
        ROOMLIB_EFFECT_SET(1, 21, 2);
    }

    if (func_800C2B68() == 1) {
        RW8(signal, 1) = 2;
    }
}

#undef ROOMLIB_EFFECT_SET
#undef ROOMLIB_EFFECT_CALL
