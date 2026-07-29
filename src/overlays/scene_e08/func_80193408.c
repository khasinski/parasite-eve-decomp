typedef short s16;
typedef unsigned short u16;

typedef struct {
    u16 value00;
    u16 value02;
    u16 value04;
    u16 value06;
    u16 value08;
    u16 value0A;
    u16 value0C;
    u16 value0E;
    u16 value10;
    u16 value12;
    u16 value14;
    u16 value16;
    u16 value18;
} State;

void func_80193408(void *arg0, unsigned char *signal, unsigned char *state) {
    State *work = (State *)state;

    work->value0A += 0xC8;
    if (*(s16 *)(signal + 2) < 0x10) {
        u16 value18 = work->value18;
        u16 value02 = work->value02;

        value18 += 8;
        do {
            value02 += 0x100;
            work->value18 = value18;
        } while (0);
        work->value02 = value02;
        work->value10 = value18 + 0x50;
    } else {
        u16 value18 = work->value18;

        value18 -= 4;
        work->value18 = value18;
        work->value10 += 0x50;
        work->value14 += 0x50;
    }
    if ((s16)work->value18 <= 0) {
        signal[1] = 2;
    }
}
