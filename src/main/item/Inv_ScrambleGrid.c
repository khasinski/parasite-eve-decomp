/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_ItemDropRngIndex;
extern unsigned char g_ItemDropRngXorKeyHigh[];
extern unsigned char g_ItemDropRngTable[];
extern unsigned char g_ItemDropRngXorKeyLow[];

unsigned char Inv_ScrambleGrid(void)
{
    int i;

    g_ItemDropRngIndex++;
    if (g_ItemDropRngIndex >= 0x209) {
        for (i = 0; i < 0x20; i++) {
            g_ItemDropRngTable[i] = g_ItemDropRngTable[i] ^ g_ItemDropRngXorKeyLow[i];
        }
        for (i = 0x20; i < 0x209; i++) {
            g_ItemDropRngTable[i] = g_ItemDropRngTable[i] ^ g_ItemDropRngXorKeyHigh[i];
        }
        g_ItemDropRngIndex = 0;
    }

    return g_ItemDropRngTable[g_ItemDropRngIndex];
}
