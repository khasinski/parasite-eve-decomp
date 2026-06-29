/* CC1_FLAGS: -g3 */

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
void __maspsx_include_asm_hack_func_80073554(void);
void __maspsx_include_asm_hack_func_80073554(void) {
	__asm__(
		"	.set	noreorder # maspsx-keep\n"
		"	.set	nomacro # maspsx-keep\n"
		"	.globl	Math_Neg64 # maspsx-keep\n"
		"Math_Neg64: # maspsx-keep\n"
		"	addiu	$sp,$sp,-0x28 # maspsx-keep\n"
		"	sw	$s0,0x20($sp) # maspsx-keep\n"
		"	addu	$s0,$a0,$zero # maspsx-keep\n"
		"	addiu	$v0,$zero,0x1 # maspsx-keep\n"
		"	sw	$a2,0x30($sp) # maspsx-keep\n"
		"	nor	$a2,$zero,$a2 # maspsx-keep\n"
		"	sw	$a1,0x2C($sp) # maspsx-keep\n"
		"	nor	$a1,$zero,$a1 # maspsx-keep\n"
		"	sw	$zero,0x1C($sp) # maspsx-keep\n"
		"	sw	$v0,0x18($sp) # maspsx-keep\n"
		"	sw	$a2,0x30($sp) # maspsx-keep\n"
		"	sw	$a1,0x2C($sp) # maspsx-keep\n"
		"	sw	$zero,0x10($sp) # maspsx-keep\n"
		"	lw	$a3,0x18($sp) # maspsx-keep\n"
		"	lw	$a1,0x2C($sp) # maspsx-keep\n"
		"	lw	$a2,0x30($sp) # maspsx-keep\n"
		"	sw	$ra,0x24($sp) # maspsx-keep\n"
		"	jal	Math_Add64 # maspsx-keep\n"
		"	addiu	$a0,$sp,0x2C # maspsx-keep\n"
		"	lw	$v0,0x2C($sp) # maspsx-keep\n"
		"	lw	$v1,0x30($sp) # maspsx-keep\n"
		"	sw	$v0,0x0($s0) # maspsx-keep\n"
		"	sw	$v1,0x4($s0) # maspsx-keep\n"
		"	addu	$v0,$s0,$zero # maspsx-keep\n"
		"	lw	$ra,0x24($sp) # maspsx-keep\n"
		"	lw	$s0,0x20($sp) # maspsx-keep\n"
		"	jr	$ra # maspsx-keep\n"
		"	addiu	$sp,$sp,0x28 # maspsx-keep\n"
		"	.set	macro # maspsx-keep\n"
		"	.set	reorder # maspsx-keep\n"
	);
}
