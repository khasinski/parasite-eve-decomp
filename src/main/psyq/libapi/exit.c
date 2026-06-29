asm(".text");
asm(".set noreorder");
asm(".globl exit");
asm("exit:");
asm("addiu   $10,$0,0xB0");
asm("jr      $10");
asm("addiu   $9,$0,0x38");
asm("nop");
