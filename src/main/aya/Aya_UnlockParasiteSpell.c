/* CC1_FLAGS: -fno-schedule-insns */

extern unsigned int D_800C0E24;

void Aya_UnlockParasiteSpell(int spell) {
    register unsigned int *flags asm("$2");
    register unsigned int mask asm("$3");
    register unsigned int value asm("$5");

    flags = &D_800C0E24;
    value = (unsigned int)flags;
    mask = 1;
    value = *flags;
    mask <<= spell;
    value |= mask;
    *flags = value;
}
