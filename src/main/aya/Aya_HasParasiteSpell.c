extern unsigned int g_AyaParasiteSpellFlags;

int Aya_HasParasiteSpell(int spell)
{
    return (g_AyaParasiteSpellFlags >> spell) & 1;
}
