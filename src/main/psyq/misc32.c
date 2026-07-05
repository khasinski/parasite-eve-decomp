/* CC1_FLAGS: -g3 */

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */

#include "include_asm.h"
void __maspsx_include_asm_hack__spu_FwriteByIO(void);

typedef unsigned short u16;

typedef struct SpuRegs {
    char pad0[0x1AA];
    volatile u16 spucnt;
} SpuRegs;

extern SpuRegs *_spu_RXX;
extern void (*volatile _spu_transferCallback)(void);
extern int g_SpuDmaDirection;

void _spu_Fw1ts(void);
void DeliverEvent(unsigned int event, int spec);

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/misc32", _spu_FwriteByIO);

void _spu_FiDMA(void) {
    register SpuRegs *regs asm("$4");

    if (g_SpuDmaDirection == 0) {
        _spu_Fw1ts();
    }

    regs = _spu_RXX;
    regs->spucnt &= ~0x30;
    __asm__ volatile(
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "lhu $v0, 0x1AA($a0)\n"
        "nop\n"
        "andi $v0, $v0, 0x30\n"
        "beq $v0, $zero, 1f\n"
        "move $v1, $zero\n"
        "addiu $v1, $v1, 1\n"
        "0:\n"
        "sltiu $v0, $v1, 0xF01\n"
        "beq $v0, $zero, 1f\n"
        "nop\n"
        "lhu $v0, 0x1AA($a0)\n"
        "nop\n"
        "andi $v0, $v0, 0x30\n"
        "bne $v0, $zero, 0b\n"
        "addiu $v1, $v1, 1\n"
        "1:\n"
        "\t.set\tmacro\n"
        "\t.set\treorder\n");

    if (_spu_transferCallback != 0) {
        _spu_transferCallback();
    } else {
        DeliverEvent(0xF0000009, 0x20);
    }
}
