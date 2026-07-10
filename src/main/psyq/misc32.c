typedef unsigned short u16;

typedef struct SpuRegs {
    char pad0[0x1A6];
    volatile u16 transfer_addr;
    volatile u16 transfer_fifo;
    volatile u16 spucnt;
    char pad1[2];
    volatile u16 transfer_status;
} SpuRegs;

extern SpuRegs *_spu_RXX;
extern void (*volatile _spu_transferCallback)(void);
extern int g_SpuDmaDirection;
extern u16 D_8009B414;

void _spu_Fw1ts(void);
int printf(char *fmt, char *arg);
void DeliverEvent(unsigned int event, int spec);

extern char D_80011C4C[];
extern char D_80011C6C[];
extern char D_80011C80[];

void _spu_FwriteByIO(u16 *src, int size) {
    SpuRegs *regs;
    int chunk;
    int written;
    int timeout;
    u16 status;
    u16 restore_status;

    regs = _spu_RXX;
    restore_status = regs->transfer_status & 0x7FF;
    regs->transfer_addr = D_8009B414;
    _spu_Fw1ts();

    while (size != 0) {
        if (size < 0x41) {
            chunk = size;
        } else {
            chunk = 0x40;
        }

        written = 0;
        while (written < chunk) {
            regs = _spu_RXX;
            regs->transfer_fifo = *src++;
            written += 2;
        }

        regs = _spu_RXX;
        regs->spucnt = (regs->spucnt & 0xFFCF) | 0x10;
        _spu_Fw1ts();

        regs = _spu_RXX;
        if ((regs->transfer_status & 0x400) != 0) {
            timeout = 1;
            while (timeout < 0xF01) {
                regs = _spu_RXX;
                if ((regs->transfer_status & 0x400) == 0) {
                    break;
                }
                timeout++;
            }
            if (timeout >= 0xF01) {
                printf(D_80011C4C, D_80011C6C);
            }
        }

        _spu_Fw1ts();
        _spu_Fw1ts();
        size -= chunk;
    }

    regs = _spu_RXX;
    regs->spucnt &= 0xFFCF;
    status = regs->transfer_status & 0x7FF;
    if (status != restore_status) {
        timeout = 1;
        while (timeout < 0xF01) {
            regs = _spu_RXX;
            status = regs->transfer_status & 0x7FF;
            if (status == restore_status) {
                break;
            }
            timeout++;
        }
        if (timeout >= 0xF01) {
            printf(D_80011C4C, D_80011C80);
        }
    }
}

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
