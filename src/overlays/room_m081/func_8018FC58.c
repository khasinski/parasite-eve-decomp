#include "../room_lib/room_lib.h"

extern int D_800966EC[];

#define RVW32(o, off) (*(volatile int *)((char *)(o) + (off)))
#define RVW16(o, off) (*(volatile short *)((char *)(o) + (off)))
#define RVU16(o, off) (*(volatile unsigned short *)((char *)(o) + (off)))

#define ROOM_M081_ROT_ENTRY(angle) \
    ({ \
        register int idx asm("$2") = (angle); \
        register int *base asm("$3") = D_800966EC; \
        (int *)((char *)base + (idx << 2)); \
    })

#define ROOM_M081_GTE_MVMVA_STORE_LOCAL(base) \
    asm volatile( \
        "addiu $2,%0,0x28\n\t" \
        "lw $12,0($2)\n\t" \
        "lw $13,4($2)\n\t" \
        "ctc2 $12,$0\n\t" \
        "ctc2 $13,$1\n\t" \
        "lw $12,8($2)\n\t" \
        "lw $13,12($2)\n\t" \
        "lw $14,16($2)\n\t" \
        "ctc2 $12,$2\n\t" \
        "ctc2 $13,$3\n\t" \
        "ctc2 $14,$4\n\t" \
        "lwc2 $0,0(%0)\n\t" \
        "lwc2 $1,4(%0)\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        ".word 0x4A486012\n\t" \
        "addiu $2,%0,0x8\n\t" \
        "swc2 $25,0($2)\n\t" \
        "swc2 $26,4($2)\n\t" \
        "swc2 $27,8($2)" \
        : \
        : "r"(base) \
        : "$2", "$12", "$13", "$14", "memory")

#define ROOM_M081_GTE_MVMVA_STORE_OFFSET_TAIL(base, vecbase, hval, low) \
    asm volatile( \
        "addiu $2,%2,0x28\n\t" \
        "sh $4,0x38(%2)\n\t" \
        "negu $3,$3\n\t" \
        "sh $3,0x34(%2)\n\t" \
        "lw $12,0($2)\n\t" \
        "lw $13,4($2)\n\t" \
        "ctc2 $12,$0\n\t" \
        "ctc2 $13,$1\n\t" \
        "lw $12,8($2)\n\t" \
        "lw $13,12($2)\n\t" \
        "lw $14,16($2)\n\t" \
        "ctc2 $12,$2\n\t" \
        "ctc2 $13,$3\n\t" \
        "ctc2 $14,$4\n\t" \
        "addiu $2,%3,0x70\n\t" \
        "lwc2 $0,0($2)\n\t" \
        "lwc2 $1,4($2)\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        ".word 0x4A486012\n\t" \
        "addiu $2,%2,0x8\n\t" \
        "swc2 $25,0($2)\n\t" \
        "swc2 $26,4($2)\n\t" \
        "swc2 $27,8($2)" \
        : "=r"(low) \
        : "0"(low), "r"(base), "r"(vecbase), "r"(hval) \
        : "$2", "$12", "$13", "$14", "memory")

void func_8018FC58(char *state, char *work) {
    int *entry;
    int angle;
    int link;
    int heading;
    int table_word;
    int table_half;
    register unsigned int word asm("$3");
    register unsigned int half asm("$4");

    link = RVW32(state, 0x80);
    if (link != 0) {
        RW32(state, 0x40) = RW32(link, 0x28);
        RW32(state, 0x44) = RW32(RVW32(state, 0x80), 0x2C);
        RW32(state, 0x48) = RW32(RVW32(state, 0x80), 0x30);
    }

    if (RW32(state, 0x4C) != 0) {
        angle = FieldEng_VecToAngle((int *)(state + 0x50), (int *)(state + 0x40)) & 0xFFF;
        entry = ROOM_M081_ROT_ENTRY(angle);
        table_half = *(short *)((char *)entry + 2);

        RVW16(work, 0x2A) = 0;
        RVW16(work, 0x28) = table_half;
        table_word = entry[0];
        half = RVU16(work, 0x28);
        RVW16(work, 0x2E) = 0;
        RVW16(work, 0x32) = 0;
        RVW16(work, 0x36) = 0;
        RVW16(work, 0x00) = 0;
        RVW16(work, 0x02) = 0;
        RVW16(work, 0x2C) = table_word;
        word = RVU16(work, 0x2C);
        RVW16(work, 0x30) = 0x1000;
        RVW16(work, 0x38) = half;
        RVW16(work, 0x34) = -word;
        heading = RW16(state, 0x4E);
        RVW16(work, 0x04) = heading;

        ROOM_M081_GTE_MVMVA_STORE_LOCAL(work);

        RW32(state, 0x40) = RW32(state, 0x50) + (RW32(work, 0x08) << 16);
        RW32(state, 0x48) = RW32(state, 0x58) + (RW32(work, 0x10) << 16);
    }

    angle = FieldEng_VecToAngle((int *)(state + 0x50), (int *)(state + 0x40)) & 0xFFF;
    entry = ROOM_M081_ROT_ENTRY(angle);
    table_half = *(short *)((char *)entry + 2);

    RVW16(work, 0x2A) = 0;
    RVW16(work, 0x28) = table_half;
    table_word = entry[0];
    half = RVU16(work, 0x28);
    RVW16(work, 0x2E) = 0;
    RVW16(work, 0x32) = 0;
    RVW16(work, 0x36) = 0;
    RVW16(work, 0x2C) = table_word;
    word = RVU16(work, 0x2C);
    RVW16(work, 0x30) = 0x1000;
    ROOM_M081_GTE_MVMVA_STORE_OFFSET_TAIL(work, state, half, word);

    RW32(state, 0x40) += RW32(work, 0x08) << 16;
    RW32(state, 0x44) += RW32(work, 0x0C) << 16;
    RW32(state, 0x48) += RW32(work, 0x10) << 16;
}
