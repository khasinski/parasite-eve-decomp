/* CC1_PROFILE: build0001 */
typedef unsigned int u32;
typedef unsigned short u16;

extern char g_AkaoVoiceChannelTable[];
extern u32 g_SpuActiveVoiceMask;
extern u32 g_SpuPendingKeyOffMask;
extern u32 g_AkaoVoiceUpdateFlags;

void Spu_ManageVoices(int arg0, int arg1);
void SeqOp_DeactivateVoice(void *arg0, int arg1);
void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

void Spu_ManageVoices(int arg0, int arg1) {
    u32 mask;
    u32 i;
    register char *field asm("$18");
    char *voice;
    u32 id;
    u32 active;
    register int control asm("$22");
    register u32 flag1 asm("$23");
    int value;
    int flags;
    int best;

    control = arg1;
    mask = 0x1000;
    id = (u16)arg0;

    if (id == 0xFFFF) {
        return;
    }

    voice = g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;

    if ((control & 0x0FFFFFFF) != 0) {
        i = 0;
        flag1 = 0x100000;
        id = 0x200000;
        field = voice + 0x38;
        do {
            if ((active & mask) != 0) {
                if ((*(int *)(field - 0xC) & control) != 0) {
                    flags = *(int *)(field + 0);
                    value = flags & flag1;
                    if (value != 0) {
                        value = flags | id;
                        *(int *)(field + 0) = value;
                    } else {
                        g_SpuPendingKeyOffMask |= mask;
                        SeqOp_DeactivateVoice(voice, mask);
                        *(int *)(field + 0) = 0;
                    }
                }
            }
            i++;
            field += 0x11C;
            voice += 0x11C;
            mask <<= 1;
        } while (i < 12);
        goto finish;
    }

    if (control < 0) {
        value = (((id << 3) + id) << 3) - id;
        voice = g_AkaoVoiceChannelTable + (value << 2);
        mask <<= id;
        if ((active & mask) != 0) {
            Spu_ManageVoices(*(int *)(voice + 0x28), 0);
        }
        mask <<= 1;
        voice += 0x11C;
        value = active & mask;
        if (value != 0) {
            Spu_ManageVoices(*(int *)(voice + 0x28), 0);
        }
        return;
    }

    i = 0;
    value = control & 0x40000000;
    if (value != 0) {
        do {
            value = *(int *)(voice + 0x2C);
            if (value != 0) {
                active &= ~mask;
            }
            i++;
            voice += 0x11C;
            mask <<= 1;
        } while (i < 12);

        voice = g_AkaoVoiceChannelTable;
        mask = 0x1000;
        best = 0;
        i = 0;
        do {
            if ((active & mask) != 0) {
                flags = *(int *)(voice + 0x50);
                if (best < flags) {
                    best = flags;
                }
            }
            i++;
            voice += 0x11C;
            mask <<= 1;
        } while (i < 12);

        voice = g_AkaoVoiceChannelTable;
        mask = 0x1000;
        i = 0;
        flag1 = 0x100000;
        control = 0x200000;
        field = voice + 0x38;
        do {
            if ((active & mask) != 0) {
                if (best == *(int *)(field + 0x18)) {
                    flags = *(int *)(field + 0);
                    value = flags & flag1;
                    if (value != 0) {
                        value = flags | control;
                        *(int *)(field + 0) = value;
                    } else {
                        g_SpuPendingKeyOffMask |= mask;
                        SeqOp_DeactivateVoice(voice, mask);
                        *(int *)(field + 0) = 0;
                    }
                }
            }
            i++;
            field += 0x11C;
            voice += 0x11C;
            mask <<= 1;
        } while (i < 12);
        goto finish;
    }

    flag1 = 0x100000;
    control = 0x200000;
    field = voice + 0x38;
    do {
        if ((active & mask) != 0) {
            if (*(int *)(field - 0x10) == id) {
                flags = *(int *)(field + 0);
                value = flags & flag1;
                if (value != 0) {
                    value = flags | control;
                    *(int *)(field + 0) = value;
                } else {
                    g_SpuPendingKeyOffMask |= mask;
                    SeqOp_DeactivateVoice(voice, mask);
                    *(int *)(field + 0) = 0;
                }
            }
        }
        i++;
        field += 0x11C;
        voice += 0x11C;
        mask <<= 1;
    } while (i < 12);

finish:
    g_AkaoVoiceUpdateFlags |= 0x10;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}
