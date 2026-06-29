asm(".text");
asm(".set noreorder");
asm(".globl srand");
asm("srand:");
asm("addiu   $10,$0,0xA0");
asm("jr      $10");
asm("addiu   $9,$0,0x30");
asm("nop");
