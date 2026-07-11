typedef signed char s8;
typedef signed short s16;
typedef int s32;

typedef struct {
    char pad0[0x68];
    s32 word68;
    s32 word6C;
    s32 word70;
    char pad74[0x4];
    s32 word78;
    s32 word7C;
    s32 word80;
    char pad84[0x4];
    s32 word88;
    s32 word8C;
    s32 word90;
} Overlay189NestedInit;

typedef struct {
    char pad0[0x3];
    s8 state;
    char pad4[0x4];
    Overlay189NestedInit *nested;
    void (*callback)(void *);
    s32 word10;
    s16 half14;
    s8 id16;
    s8 id17;
    char pad18[0x1];
    s8 mode;
    s8 byte1A;
    char pad1B[0x31];
    s32 word4C;
    s32 word50;
    s32 word54;
    char pad58[0x14];
    s32 word6C;
    s32 word70;
    s32 word74;
    s32 word78;
    s32 word7C;
    s32 word80;
    char pad84[0x8];
    s32 word8C;
    char pad90[0x2];
    s16 half92;
    s16 half94;
    s16 half96;
} Overlay189InitB;

extern void D_80190DCC(void *);

s32 func_80181A60(Overlay189InitB *arg0) {
    Overlay189NestedInit *nested;

    arg0->state = 1;
    arg0->id17 = -1;
    arg0->id16 = -1;
    arg0->mode = 7;
    arg0->word8C = 0x10000;

    nested = arg0->nested;
    arg0->word10 = 0;
    arg0->half14 = 0;
    arg0->byte1A = 0;
    arg0->word74 = 0;
    arg0->word78 = 0;
    arg0->word7C = 0;
    arg0->word80 = 0;
    arg0->half92 = 0;
    arg0->word4C = 0;
    arg0->word50 = 0;
    arg0->word54 = 0;
    arg0->half94 = 0;
    arg0->word6C = 0;
    arg0->callback = D_80190DCC;
    arg0->word70 = 0;
    arg0->half96 = 0;

    nested->word68 = 0;
    nested->word6C = 0;
    nested->word70 = 0;
    nested->word88 = 0;
    nested->word8C = 0;
    nested->word90 = 0;
    nested->word78 = 0;
    nested->word7C = 0;
    nested->word80 = 0;
    return 0;
}
