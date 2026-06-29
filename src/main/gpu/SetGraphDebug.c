/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
/* Function body emitted as asm() (PSYQ GCC 2.7.2 register
 * allocation / scheduling diverges from the ROM pervasively).
 * Wrapped via maspsx's INCLUDE_ASM hack so the body is passed
 * through verbatim; bare global loads gp-rel via the .extern size
 * hints + `as -G8`. See tools/maspsx/PE1_IMPROVEMENTS.md. */
asm(".globl SetGraphDebug");
asm("SetGraphDebug = func_80074BB8");
void __maspsx_include_asm_hack_func_80074BB8(void);
void __maspsx_include_asm_hack_func_80074BB8(void) {
	__asm__(
		"	.set	noreorder # maspsx-keep\n"
		"	.set	nomacro # maspsx-keep\n"
		"	.globl	func_80074BB8 # maspsx-keep\n"
		"func_80074BB8: # maspsx-keep\n"
		"	addiu	$sp,$sp,-0x18 # maspsx-keep\n"
		"	lui	$v1,%hi(g_GraphDebug) # maspsx-keep\n"
		"	addiu	$v1,$v1,%lo(g_GraphDebug) # maspsx-keep\n"
		"	sw	$ra,0x14($sp) # maspsx-keep\n"
		"	sw	$s0,0x10($sp) # maspsx-keep\n"
		"	lbu	$s0,0x0($v1) # maspsx-keep\n"
		"	sb	$a0,0x0($v1) # maspsx-keep\n"
		"	andi	$a0,$a0,0xFF # maspsx-keep\n"
		"	beqz	$a0,.L80074C04 # maspsx-keep\n"
		"	addu	$v0,$s0,$zero # maspsx-keep\n"
		"	lui	$v0,%hi(g_GpuDebugPrintf) # maspsx-keep\n"
		"	lw	$v0,%lo(g_GpuDebugPrintf)($v0) # maspsx-keep\n"
		"	lbu	$a1,0x0($v1) # maspsx-keep\n"
		"	lbu	$a2,-0x2($v1) # maspsx-keep\n"
		"	lbu	$a3,0x1($v1) # maspsx-keep\n"
		"	lui	$a0,%hi(D_80011814) # maspsx-keep\n"
		"	jalr	$v0 # maspsx-keep\n"
		"	addiu	$a0,$a0,%lo(D_80011814) # maspsx-keep\n"
		"	addu	$v0,$s0,$zero # maspsx-keep\n"
		".L80074C04: # maspsx-keep\n"
		"	lw	$ra,0x14($sp) # maspsx-keep\n"
		"	lw	$s0,0x10($sp) # maspsx-keep\n"
		"	jr	$ra # maspsx-keep\n"
		"	addiu	$sp,$sp,0x18 # maspsx-keep\n"
		"	.set	macro # maspsx-keep\n"
		"	.set	reorder # maspsx-keep\n"
	);
}
