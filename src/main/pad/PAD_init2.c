asm(".text");
asm(".set noreorder");
asm(".globl PAD_init2");
asm("PAD_init2:");
asm("addiu   $10,$0,0xB0");
asm("jr      $10");
asm("addiu   $9,$0,0x15");
asm("nop");
