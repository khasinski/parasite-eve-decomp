/* Incomplete array: retail materializes the flag word's address
 * (0x800C0E24) with lui/addiu before the read-modify-write. */
extern unsigned int g_AyaParasiteSpellFlags[];

void Aya_UnlockParasiteSpell(int spell) {
    g_AyaParasiteSpellFlags[0] |= 1u << spell;
}
