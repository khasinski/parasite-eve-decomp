asm(".text");
asm(".set noreorder");
asm(".globl _card_write");
asm("_card_write:");
asm("addiu   $10,$0,0xB0");
asm("jr      $10");
asm("addiu   $9,$0,0x4E");
asm("nop");
