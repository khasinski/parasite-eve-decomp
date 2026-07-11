typedef unsigned char u8;

extern int D_800A1850;
extern int D_800BCDA8;
extern int D_800BCDAC;
extern int D_800BCDB0;
extern int D_800BCDB4;
extern int D_800BCDB8;
extern int D_800BCDBC;
extern int D_800BCDC0;
extern int D_800BCDC4;
extern u8 D_800A0ED4[];

int EnterCriticalSection(void);
int ExitCriticalSection(void);
int OpenEvent(int desc, int spec, int mode, void (*func)(void));
int EnableEvent(int event);
void MemCard_InitCardSubsystem(int arg0);
long StartCARD(void);
void _bu_init(void);
void _card_auto(int arg0);

void MemCard_OnEventF400Spec0004(void);
void MemCard_OnEventF400Spec8000(void);
void MemCard_OnEventF400Spec0100(void);
void MemCard_OnEventF400Spec2000(void);
void MemCard_OnEventF000Spec0004(void);
void MemCard_OnEventF000Spec8000(void);
void MemCard_OnEventF000Spec0100(void);
void MemCard_OnEventF000Spec2000(void);

void MemCard_InitManager(void) {
    register int i asm("$17");
    register int *events asm("$16");
    int value;
    int event;
    register int desc asm("$4");
    register int spec asm("$5");
    register int mode asm("$6");
    register void (*callback)(void) asm("$7");

    if (D_800A1850 == 0) {
        D_800A1850 = 1;
        i = 0;
        EnterCriticalSection();

        event = OpenEvent(0xF4000001, 0x0004, 0x1000, MemCard_OnEventF400Spec0004);
        desc = 0xF4000001;
        spec = 0x8000;
        mode = 0x1000;
        callback = MemCard_OnEventF400Spec8000;
        /* Keep the event table base load after the second OpenEvent arguments. */
        asm volatile("" ::: "$16");
        events = &D_800BCDA8;
        events[0] = event;
        D_800BCDAC = OpenEvent(desc, spec, mode, callback);
        D_800BCDB0 = OpenEvent(0xF4000001, 0x0100, 0x1000, MemCard_OnEventF400Spec0100);
        D_800BCDB4 = OpenEvent(0xF4000001, 0x2000, 0x1000, MemCard_OnEventF400Spec2000);
        D_800BCDB8 = OpenEvent(0xF0000011, 0x0004, 0x1000, MemCard_OnEventF000Spec0004);
        D_800BCDBC = OpenEvent(0xF0000011, 0x8000, 0x1000, MemCard_OnEventF000Spec8000);
        D_800BCDC0 = OpenEvent(0xF0000011, 0x0100, 0x1000, MemCard_OnEventF000Spec0100);
        D_800BCDC4 = OpenEvent(0xF0000011, 0x2000, 0x1000, MemCard_OnEventF000Spec2000);

        MemCard_InitCardSubsystem(0);
        StartCARD();
        _bu_init();
        _card_auto(0);

        do {
            register int loop_event asm("$4");

            loop_event = *events;
            events++;
            /* Keep the table pointer increment before the EnableEvent call. */
            asm volatile("" : "=r"(events) : "0"(events));
            EnableEvent(loop_event);
            i++;
        } while (i < 8);

        ExitCriticalSection();
    }

    value = 0x418;
    do {
        D_800A0ED4[value] = 0;
        value -= 0x418;
    } while (value >= 0);
}
