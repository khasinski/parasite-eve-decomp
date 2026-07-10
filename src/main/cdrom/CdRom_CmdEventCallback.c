#include "pe1/psyq_cd.h"

typedef unsigned char u8;
typedef unsigned int u32;

extern struct { char _[16]; } D_8009B558_o __asm__("D_8009B558");
extern struct { char _[16]; } D_8009B56C_o __asm__("D_8009B56C");
extern u32 g_DsSyncCallback[] __asm__("D_800A36A4");

#define D_8009B558 ((u8 *)&D_8009B558_o)
#define D_8009B56C (*(DsReadyEventWindow *)&D_8009B56C_o)
#define U8(base, off) (*(u8 *)((u8 *)(base) + (off)))
#define U32(base, off) (*(u32 *)((u8 *)(base) + (off)))

void CdRom_CmdEventCallback(int event) {
    register u32 event_reg asm("$7");
    register u8 *cmd_state asm("$6");
    register int value asm("$2");
    register int status asm("$3");
    register DsReadyEventWindow *ready asm("$4");
    int stack_pad[4];

    event_reg = event & 0xFF;
    if (event_reg == 2) {
        cmd_state = D_8009B558;
        asm volatile("" : "=r"(cmd_state) : "0"(cmd_state));
        status = U8(cmd_state, 0);
        value = 0xE;
        if (status == value) {
            value = U8(cmd_state, 0x29);
            status = U8(cmd_state, 1);
            if (((value ^ status) & 0x80) != 0) {
                U32(cmd_state, 0x20) = 0xF;
                U32(cmd_state, 0x1C) = event_reg;
                U32(cmd_state, 0x3C) = 3;
            }
            U8(cmd_state, 0x29) = status;
        }
    }

    status = event & 0xFF;
    if (status == 5) {
        asm volatile(
            ".set\tnoreorder\n\t"
            "lui\t$4,%%hi(D_8009B56C)\n\t"
            "addiu\t$4,$4,%%lo(D_8009B56C)\n\t"
            "lbu\t$2,0x0($4)\n\t"
            "nop\n\t"
            "andi\t$2,$2,0x10\n\t"
            "beqz\t$2,1f\n\t"
            "addiu\t$2,$zero,1\n\t"
            "lui\t$3,%%hi(D_800A36A4)\n\t"
            "lw\t$3,%%lo(D_800A36A4)($3)\n\t"
            "addiu\t$2,$zero,2\n\t"
            "sw\t$2,0x8($4)\n\t"
            "addiu\t$2,$zero,0xC\n\t"
            "beqz\t$3,2f\n\t"
            "sw\t$2,0xC($4)\n\t"
            "lw\t$2,-0x18($4)\n\t"
            "nop\n\t"
            "beqz\t$2,2f\n\t"
            "nop\n\t"
            "lui\t$2,%%hi(D_800A36A4)\n\t"
            "lw\t$2,%%lo(D_800A36A4)($2)\n\t"
            "nop\n\t"
            "jalr\t$2\n\t"
            "addiu\t$4,$zero,5\n\t"
            "j\t2f\n\t"
            "nop\n"
            "1:\n\t"
            "sw\t$2,0x8($4)\n\t"
            "addiu\t$2,$zero,0xB\n\t"
            "sw\t$2,0xC($4)\n"
            "2:\n\t"
            ".set\treorder"
            :
            :
            : "$2", "$3", "$4", "$31", "memory");
    }
    asm volatile("" : : "r"(stack_pad));
}
