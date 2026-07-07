/* CC1_FLAGS: -g3 */
typedef unsigned char u8;
typedef int s32;

extern volatile u8 *g_CdRegIndexBase;
extern volatile u8 *g_CdRegDataWrite;
extern volatile u8 *g_CdRegResponse;
extern s32 *g_CdRegRequest;

void CD_flush(void) {
    *g_CdRegIndexBase = 1;
    __asm__ volatile(
        "\t.set\tnoreorder\n"
        "\t.set\tnomacro\n"
        "lui $v0, %hi(g_CdRegResponse)\n"
        "lw $v0, %lo(g_CdRegResponse)($v0)\n"
        "nop\n"
        "lbu $v0, 0x0($v0)\n"
        "nop\n"
        "andi $v0, $v0, 0x7\n"
        "beqz $v0, 1f\n"
        "addiu $a0, $zero, 0x1\n"
        "addiu $v1, $zero, 0x7\n"
        "0:\n"
        "lui $v0, %hi(g_CdRegIndexBase)\n"
        "lw $v0, %lo(g_CdRegIndexBase)($v0)\n"
        "nop\n"
        "sb $a0, 0x0($v0)\n"
        "lui $v0, %hi(g_CdRegResponse)\n"
        "lw $v0, %lo(g_CdRegResponse)($v0)\n"
        "nop\n"
        "sb $v1, 0x0($v0)\n"
        "lui $v0, %hi(g_CdRegDataWrite)\n"
        "lw $v0, %lo(g_CdRegDataWrite)($v0)\n"
        "nop\n"
        "sb $v1, 0x0($v0)\n"
        "lui $v0, %hi(g_CdRegResponse)\n"
        "lw $v0, %lo(g_CdRegResponse)($v0)\n"
        "nop\n"
        "lbu $v0, 0x0($v0)\n"
        "nop\n"
        "andi $v0, $v0, 0x7\n"
        "bnez $v0, 0b\n"
        "nop\n"
        "1:\n"
        "lui $v1, %hi(D_8009B294)\n"
        "addiu $v1, $v1, %lo(D_8009B294)\n"
        "sb $zero, 0x2($v1)\n"
        "lbu $v0, 0x2($v1)\n"
        "nop\n"
        "sb $v0, 0x1($v1)\n"
        "lui $a0, %hi(g_CdRegIndexBase)\n"
        "lw $a0, %lo(g_CdRegIndexBase)($a0)\n"
        "addiu $v0, $zero, 0x2\n"
        "sb $v0, 0x0($v1)\n"
        "sb $zero, 0x0($a0)\n"
        "lui $v0, %hi(g_CdRegResponse)\n"
        "lw $v0, %lo(g_CdRegResponse)($v0)\n"
        "nop\n"
        "sb $zero, 0x0($v0)\n"
        "\t.set\tmacro\n"
        "\t.set\treorder\n");
    *g_CdRegRequest = 0x1325;
}
