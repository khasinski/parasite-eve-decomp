/* CC1_FLAGS: -G8 */

/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
void __maspsx_include_asm_hack___main(void);
void __maspsx_include_asm_hack___main(void) {
	__asm__(
		".extern	jtbl_80010000 # maspsx-keep\n"
		"	.set	noreorder # maspsx-keep\n"
		"	.set	nomacro # maspsx-keep\n"
		"	.globl	__main # maspsx-keep\n"
		"__main: # maspsx-keep\n"
		"	lui	$t0,%hi(D_80094538) # maspsx-keep\n"
		"	lw	$t0,%lo(D_80094538)($t0) # maspsx-keep\n"
		"	addiu	$sp,$sp,-0x10 # maspsx-keep\n"
		"	sw	$s0,0x4($sp) # maspsx-keep\n"
		"	sw	$s1,0x8($sp) # maspsx-keep\n"
		"	sw	$ra,0xC($sp) # maspsx-keep\n"
		"	bnez	$t0,.L80072634 # maspsx-keep\n"
		"	ori	$t0,$zero,0x1 # maspsx-keep\n"
		"	lui	$at,%hi(D_80094538) # maspsx-keep\n"
		"	sw	$t0,%lo(D_80094538)($at) # maspsx-keep\n"
		"	lui	$s0,%hi(jtbl_80010000) # maspsx-keep\n"
		"	addiu	$s0,$s0,%lo(jtbl_80010000) # maspsx-keep\n"
		"	lui	$s1,(0x0 >> 16) # maspsx-keep\n"
		"	addiu	$s1,$s1,0x0 # maspsx-keep\n"
		"	beqz	$s1,.L80072634 # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		".L8007261C: # maspsx-keep\n"
		"	lw	$t0,0x0($s0) # maspsx-keep\n"
		"	addiu	$s0,$s0,0x4 # maspsx-keep\n"
		"	jalr	$t0 # maspsx-keep\n"
		"	addiu	$s1,$s1,-0x1 # maspsx-keep\n"
		"	bnez	$s1,.L8007261C # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		".L80072634: # maspsx-keep\n"
		"	lw	$ra,0xC($sp) # maspsx-keep\n"
		"	lw	$s1,0x8($sp) # maspsx-keep\n"
		"	lw	$s0,0x4($sp) # maspsx-keep\n"
		"	addiu	$sp,$sp,0x10 # maspsx-keep\n"
		"	jr	$ra # maspsx-keep\n"
		"	nop # maspsx-keep\n"
		"	.set	macro # maspsx-keep\n"
		"	.set	reorder # maspsx-keep\n"
	);
}
