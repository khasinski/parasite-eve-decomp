asm(".text");
asm(".set noreorder");
asm(".globl InitPAD2");
asm("InitPAD2:");
asm("addiu   $10,$0,0xB0");
asm("jr      $10");
asm("addiu   $9,$0,0x12");
asm("nop");
