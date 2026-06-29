asm(".text");
asm(".set noreorder");
asm(".globl _card_load");
asm("_card_load:");
asm("addiu   $10,$0,0xA0");
asm("jr      $10");
asm("addiu   $9,$0,0xAC");
asm("nop");
