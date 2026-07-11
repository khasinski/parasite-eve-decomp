/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef int s32;

s32 VSync(s32 mode);
s32 CdRom_IsBusy(s32 dst, s32 sector_size);
s32 CdRom_IsBusy2(s32 dst, s32 sector_size);
void Save_ProcessDataCallback(void);

extern char D_8009B6B0[];
extern void (*g_CdReadCompleteCallback[])(s32, s32);
#define g_CdReadCompleteCallback (g_CdReadCompleteCallback[0])

#define CD_READ_WORD(offset) (*(s32 *)(D_8009B6B0 + (offset)))

void CdRom_ReadProgressCallback(s32 event, s32 data) {
    void (*callback)(s32, s32);
    s32 mode;

    CD_READ_WORD(0x18) = VSync(-1);

    if (CD_READ_WORD(8) & 1) {
        if (CD_READ_WORD(4) > 0) {
            CdRom_IsBusy2(CD_READ_WORD(0), CD_READ_WORD(-4));
            CD_READ_WORD(0xC) = data;
            return;
        }

        Save_ProcessDataCallback();
        callback = g_CdReadCompleteCallback;
        if (callback == 0) {
            return;
        }
        mode = event;
    } else {
        if (CD_READ_WORD(4) > 0) {
            CdRom_IsBusy(CD_READ_WORD(0), CD_READ_WORD(-4));
            CD_READ_WORD(0) += CD_READ_WORD(-4) << 2;
            CD_READ_WORD(4) -= 1;
        }

        if (CD_READ_WORD(0x14) + 0x4B0 < VSync(-1)) {
            CD_READ_WORD(4) = -1;
        }

        if (CD_READ_WORD(4) != 0) {
            if (CD_READ_WORD(0x14) + 0x4B0 >= VSync(-1)) {
                return;
            }
        }

        Save_ProcessDataCallback();
        callback = g_CdReadCompleteCallback;
        if (callback == 0) {
            return;
        }
        mode = 2;
    }

    if (CD_READ_WORD(4) < 0) {
        mode = 5;
    }
    callback(mode & 0xFF, data);
}
