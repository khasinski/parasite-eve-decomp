/* CC1_FLAGS: -fno-schedule-insns */

extern unsigned int D_800C0E24;

void Aya_UnlockParasiteSpell(int spell) {
    unsigned int *flags;
    unsigned int mask;
    unsigned int value;

    flags = &D_800C0E24;
    value = (unsigned int)flags;
    mask = 1;
    value = *flags;
    mask <<= spell;
    value |= mask;
    *flags = value;
}
