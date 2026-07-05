/* CC1_PROFILE: build0001 */
typedef unsigned int u32;
typedef unsigned short u16;

extern char g_AkaoTrackStateBackup[];
extern char g_AkaoVoiceStateBackup[];
extern char g_AkaoVoiceStateTable[];
extern char *g_AkaoCurTrack;

void Util_CopyWords(unsigned int *src, unsigned int *dst, unsigned int size);

void Seq_RestorePrimaryState(void) {
    u32 i;
    char *ptr;
    u32 flags;
    u16 value;

    if (*(u32 *)(g_AkaoCurTrack + 4) != 0) {
        Util_CopyWords(g_AkaoCurTrack, g_AkaoTrackStateBackup, 0x68);
        Util_CopyWords(g_AkaoVoiceStateTable, g_AkaoVoiceStateBackup, 0x1AA0);

        flags = *(u32 *)g_AkaoTrackStateBackup;
        if ((flags & 0x100) != 0) {
            i = 0;
            ptr = g_AkaoVoiceStateBackup + 0x5A;
            do {
                value = *(u16 *)ptr;
                if (value >= 0x50) {
                    *(u16 *)ptr = value - 0x30;
                }
                i++;
                ptr += 0x11C;
            } while (i < 24);
        }
    }
}
