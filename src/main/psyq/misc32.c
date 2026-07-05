/* CC1_FLAGS: -g3 */

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
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

void __maspsx_include_asm_hack__spu_FwriteByIO(void) {
	__asm__(
		"	.set	noreorder # maspsx-keep\n"
		"	.set	nomacro # maspsx-keep\n"
		"	.globl	_spu_FwriteByIO # maspsx-keep\n"
		"_spu_FwriteByIO: # maspsx-keep\n"
		"	lui	$v0,%hi(_spu_RXX) # maspsx-keep\n"
		"	lw	$v0,%lo(_spu_RXX)($v0) # maspsx-keep\n"
		"	lui	$v1,%hi(g_SpuTransferAddr) # maspsx-keep\n"
		"	lhu	$v1,%lo(g_SpuTransferAddr)($v1) # maspsx-keep\n"
		"	addiu	$sp,$sp,-0x28 # maspsx-keep\n"
		"	sw	$s1,0x14($sp) # maspsx-keep\n"
		"	addu	$s1,$a1,$zero # maspsx-keep\n"
		"	sw	$ra,0x20($sp) # maspsx-keep\n"
		"	sw	$s3,0x1C($sp) # maspsx-keep\n"
		"	sw	$s2,0x18($sp) # maspsx-keep\n"
		"	sw	$s0,0x10($sp) # maspsx-keep\n"
		"	lhu	$a1,0x1AE($v0) # maspsx-keep\n"
		"	addu	$s2,$a0,$zero # maspsx-keep\n"
		"	sh	$v1,0x1A6($v0) # maspsx-keep\n"
		"	jal	_spu_Fw1ts # maspsx-keep\n"
		"	andi	$s3,$a1,0x7FF # maspsx-keep\n"
		"	beqz	$s1,.L8007D57C # maspsx-keep\n"
		"	sltiu	$v0,$s1,0x41 # maspsx-keep\n"
		".L8007D49C: # maspsx-keep\n"
		"	beqz	$v0,.L8007D4A8 # maspsx-keep\n"
		"	addiu	$s0,$zero,0x40 # maspsx-keep\n"
		"	addu	$s0,$s1,$zero # maspsx-keep\n"
		".L8007D4A8: # maspsx-keep\n"
		"	blez	$s0,.L8007D4D4 # maspsx-keep\n"
		"	addu	$v1,$zero,$zero # maspsx-keep\n"
		"	lui	$a0,%hi(_spu_RXX) # maspsx-keep\n"
		"	lw	$a0,%lo(_spu_RXX)($a0) # maspsx-keep\n"
		".L8007D4B8: # maspsx-keep\n"
		"	lhu	$v0,0x0($s2) # maspsx-keep\n"
		"	addiu	$s2,$s2,0x2 # maspsx-keep\n"
		"	addiu	$v1,$v1,0x2 # maspsx-keep\n"
		"	sh	$v0,0x1A8($a0) # maspsx-keep\n"
		"	slt	$v0,$v1,$s0 # maspsx-keep\n"
		"	bnez	$v0,.L8007D4B8 # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		".L8007D4D4: # maspsx-keep\n"
		"	lui	$v1,%hi(_spu_RXX) # maspsx-keep\n"
		"	lw	$v1,%lo(_spu_RXX)($v1) # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		"	lhu	$a0,0x1AA($v1) # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		"	andi	$v0,$a0,0xFFCF # maspsx-keep\n"
		"	ori	$v0,$v0,0x10 # maspsx-keep\n"
		"	jal	_spu_Fw1ts # maspsx-keep\n"
		"	sh	$v0,0x1AA($v1) # maspsx-keep\n"
		"	lui	$v0,%hi(_spu_RXX) # maspsx-keep\n"
		"	lw	$v0,%lo(_spu_RXX)($v0) # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		"	lhu	$v0,0x1AE($v0) # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		"	andi	$v0,$v0,0x400 # maspsx-keep\n"
		"	beqz	$v0,.L8007D564 # maspsx-keep\n"
		"	addu	$v1,$zero,$zero # maspsx-keep\n"
		"	addiu	$v1,$v1,0x1 # maspsx-keep\n"
		".L8007D51C: # maspsx-keep\n"
		"	sltiu	$v0,$v1,0xF01 # maspsx-keep\n"
		"	bnez	$v0,.L8007D544 # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		"	lui	$a0,%hi(D_80011C4C) # maspsx-keep\n"
		"	addiu	$a0,$a0,%lo(D_80011C4C) # maspsx-keep\n"
		"	lui	$a1,%hi(D_80011C6C) # maspsx-keep\n"
		"	jal	printf # maspsx-keep\n"
		"	addiu	$a1,$a1,%lo(D_80011C6C) # maspsx-keep\n"
		"	j	.L8007D564 # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		".L8007D544: # maspsx-keep\n"
		"	lui	$v0,%hi(_spu_RXX) # maspsx-keep\n"
		"	lw	$v0,%lo(_spu_RXX)($v0) # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		"	lhu	$v0,0x1AE($v0) # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		"	andi	$v0,$v0,0x400 # maspsx-keep\n"
		"	bnez	$v0,.L8007D51C # maspsx-keep\n"
		"	addiu	$v1,$v1,0x1 # maspsx-keep\n"
		".L8007D564: # maspsx-keep\n"
		"	jal	_spu_Fw1ts # maspsx-keep\n"
		"	subu	$s1,$s1,$s0 # maspsx-keep\n"
		"	jal	_spu_Fw1ts # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		"	bnez	$s1,.L8007D49C # maspsx-keep\n"
		"	sltiu	$v0,$s1,0x41 # maspsx-keep\n"
		".L8007D57C: # maspsx-keep\n"
		"	lui	$v0,%hi(_spu_RXX) # maspsx-keep\n"
		"	lw	$v0,%lo(_spu_RXX)($v0) # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		"	lhu	$a0,0x1AA($v0) # maspsx-keep\n"
		"	andi	$a1,$s3,0xFFFF # maspsx-keep\n"
		"	andi	$v1,$a0,0xFFCF # maspsx-keep\n"
		"	sh	$v1,0x1AA($v0) # maspsx-keep\n"
		"	lhu	$v0,0x1AE($v0) # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		"	andi	$v0,$v0,0x7FF # maspsx-keep\n"
		"	beq	$v0,$a1,.L8007D5F8 # maspsx-keep\n"
		"	addu	$v1,$zero,$zero # maspsx-keep\n"
		"	addiu	$v1,$v1,0x1 # maspsx-keep\n"
		".L8007D5B0: # maspsx-keep\n"
		"	sltiu	$v0,$v1,0xF01 # maspsx-keep\n"
		"	bnez	$v0,.L8007D5D8 # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		"	lui	$a0,%hi(D_80011C4C) # maspsx-keep\n"
		"	addiu	$a0,$a0,%lo(D_80011C4C) # maspsx-keep\n"
		"	lui	$a1,%hi(D_80011C80) # maspsx-keep\n"
		"	jal	printf # maspsx-keep\n"
		"	addiu	$a1,$a1,%lo(D_80011C80) # maspsx-keep\n"
		"	j	.L8007D5F8 # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		".L8007D5D8: # maspsx-keep\n"
		"	lui	$v0,%hi(_spu_RXX) # maspsx-keep\n"
		"	lw	$v0,%lo(_spu_RXX)($v0) # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		"	lhu	$v0,0x1AE($v0) # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		"	andi	$v0,$v0,0x7FF # maspsx-keep\n"
		"	bne	$v0,$a1,.L8007D5B0 # maspsx-keep\n"
		"	addiu	$v1,$v1,0x1 # maspsx-keep\n"
		".L8007D5F8: # maspsx-keep\n"
		"	lw	$ra,0x20($sp) # maspsx-keep\n"
		"	lw	$s3,0x1C($sp) # maspsx-keep\n"
		"	lw	$s2,0x18($sp) # maspsx-keep\n"
		"	lw	$s1,0x14($sp) # maspsx-keep\n"
		"	lw	$s0,0x10($sp) # maspsx-keep\n"
		"	jr	$ra # maspsx-keep\n"
		"	addiu	$sp,$sp,0x28 # maspsx-keep\n"
		"	.set	macro # maspsx-keep\n"
		"	.set	reorder # maspsx-keep\n"
	);
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
