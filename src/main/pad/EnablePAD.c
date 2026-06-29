asm(".text");
asm(".set noreorder");
asm(".set noat");

asm(".globl EnablePAD");
asm("EnablePAD:");
asm("lui $9,%hi(jtbl_800A34C8)");
asm("lw $9,%lo(jtbl_800A34C8)($9)");
asm("nop");
asm("jr $9");
asm("nop");

asm(".globl DisablePAD");
asm("DisablePAD:");
asm("lui $9,%hi(jtbl_800A34CC)");
asm("lw $9,%lo(jtbl_800A34CC)($9)");
asm("nop");
asm("jr $9");
asm("nop");
