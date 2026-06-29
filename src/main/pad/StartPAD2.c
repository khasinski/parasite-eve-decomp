asm(".text");
asm(".set noreorder");
asm(".globl StartPAD2");
asm("StartPAD2:");
asm("addiu   $10,$0,0xB0");
asm("jr      $10");
asm("addiu   $9,$0,0x13");
asm("nop");
