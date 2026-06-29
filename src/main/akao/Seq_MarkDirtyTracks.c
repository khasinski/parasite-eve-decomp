/* CC1_PROFILE: build0001 */
typedef unsigned int u32;

extern char *g_AkaoCurTrack;

void Seq_MarkDirtyTracks(char *arg0) {
    u32 mask;
    u32 bit;

    mask = *(u32 *)(g_AkaoCurTrack + 4);
    if (mask != 0) {
        bit = 1;
        arg0 += 0xF4;
        do {
            if (mask & bit) {
                mask ^= bit;
                *(u32 *)arg0 |= 3;
            }
            arg0 += 0x11C;
            bit <<= 1;
        } while (mask != 0);
    }
}
