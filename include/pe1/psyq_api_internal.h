#ifndef PE1_PSYQ_API_INTERNAL_H
#define PE1_PSYQ_API_INTERNAL_H

typedef signed int s32;
typedef signed short s16;
typedef unsigned short u16;
typedef void (*PadToggleFunc)(void);

typedef struct RootCounter {
    u16 rootCounter;
    s16 field_02;
    s16 mode;
    s16 field_06;
    s16 target;
    s16 field_0A;
    s32 field_0C;
} RootCounter;

typedef struct InterruptControlRegisters {
    s32 status;
    s32 mask;
} InterruptControlRegisters;

extern RootCounter *D_8009B7D0;
extern InterruptControlRegisters *D_8009B7CC;
extern s32 D_8009B7D4[4];
extern PadToggleFunc jtbl_800A34C8;
extern PadToggleFunc jtbl_800A34CC;

#define _counters ((volatile RootCounter *)D_8009B7D0)
#define _interrupt_status_register ((s32 *)D_8009B7CC)
#define _interrupt_status_masks D_8009B7D4

#endif
