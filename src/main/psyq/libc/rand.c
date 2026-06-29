asm(".text");
asm(".set noreorder");
asm(".globl rand");
asm("rand:");
asm("addiu   $10,$0,0xA0");
asm("jr      $10");
asm("addiu   $9,$0,0x2F");
asm("nop");
