typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct AkaoCommandArgs {
    int *opcode;
    int *arg0;
    int *arg1;
    int *arg2;
    int *arg3;
    int *ret;
} AkaoCommandArgs;

extern int D_8009CE00;
extern int *D_8009D300;
extern int D_800B0E00[];
extern u8 *D_800B0E64;
extern u8 *D_8009D254;
extern u8 *D_8009D20C;
extern u8 *D_8009D2F0;
extern u8 D_800944A8[];
extern u8 D_800B0CE9;
extern u8 D_800B0CEA;
extern u8 D_800B0DB8;
extern u8 D_800B0DB9;
extern u8 D_800B0DBE;
extern u8 D_800B0DCE;
extern u8 D_800B0DCF;
extern u8 *D_800B0DFC;
extern s16 D_800B0DD0;
extern s16 D_800B0DD2;

int Akao_Cmd_10(int arg0);
void Akao_Cmd_11(int arg0);
void Akao_Cmd_21(int arg0, int arg1);
void Akao_Cmd_90(int arg0);
void Akao_Cmd_92(int arg0);
void Akao_Cmd_98_9A_9C(int arg0);
void Akao_Cmd_99_9B_9D(int arg0);
int Akao_Cmd_19_Then_C0(int arg0, int arg1);
void Akao_Cmd_A0(int arg0, int arg1, int arg2);
void Akao_Cmd_A1(int arg0, int arg1, int arg2, int arg3);
void Akao_Cmd_A2(int arg0, int arg1, int arg2);
void Akao_Cmd_A3(int arg0, int arg1, int arg2, int arg3);
void Akao_Cmd_A4(int arg0, int arg1, int arg2);
void Akao_Cmd_A5(int arg0, int arg1, int arg2, int arg3);
void Akao_Cmd_A8(int arg0);
void Akao_Cmd_A9(int arg0, int arg1);
void Akao_Cmd_AA(int arg0);
void Akao_Cmd_AB(int arg0, int arg1);
void Akao_Cmd_AC(int arg0);
void Akao_Cmd_AD(int arg0, int arg1);
void Akao_Cmd_C0_WithSlot(int arg0, int arg1);
void Akao_Cmd_C1_WithSlot(int arg0, int arg1, int arg2);
void Akao_Cmd_C2_WithSlot(int arg0, int arg1, int arg2, int arg3);
void Akao_Cmd_D0(int arg0);
void Akao_Cmd_D1(int arg0, int arg1);
void Akao_Cmd_D2(int arg0, int arg1, int arg2);
void Akao_Cmd_D4(int arg0);
void Akao_Cmd_D5(int arg0, int arg1);
void Akao_Cmd_D6(int arg0, int arg1, int arg2);
void Akao_Cmd_D8(int arg0);
void Akao_Cmd_D9(int arg0, int arg1);
void Akao_Cmd_DA(int arg0, int arg1, int arg2);
void Akao_Cmd_F1(void);
void Akao_ResetPositionalState(void);
int Akao_SendTableCommand(int arg0, int arg1, int arg2, int arg3, int arg4);
void Akao_SetCdMixVolume(void *arg0);
int Asset_Find08w(int arg0, int arg1, int arg2, int arg3, int arg4);
int CD_SeekToTrack(int arg0, int arg1, int arg2, int *arg3, int arg4);
int Overlay_FindAudioSlotIndex(int id);
int Overlay_GetAudioSlotByKey(int arg0);
int Overlay_RegisterAudioSlot(int arg0, int arg1, int arg2, int arg3);

#define ARG(ctx, index) (*((ctx)->arg##index))
#define ARG_PTR(ctx, index) ((u8 *)((ctx)->arg##index))

static int Akao_FinishSeekBusy(int seek_result) {
    D_8009CE00 -= 0x20;
    D_8009D300[4] = seek_result;
    return 0;
}

static int Akao_SeekAndStartTrack(AkaoCommandArgs *cmd, int mode, int use_19) {
    int track;
    int seek_result;
    int command_result;

    seek_result = CD_SeekToTrack(ARG(cmd, 0), 1, mode, &track, ARG(cmd, 1) < 1);
    if (seek_result == 1) {
        return Akao_FinishSeekBusy(seek_result);
    }

    if ((unsigned int)(track + 2) < 2) {
        return 1;
    }

    if (use_19) {
        command_result = Akao_Cmd_19_Then_C0(D_800B0E00[track], ARG(cmd, 1));
        *cmd->ret = command_result;
        Overlay_RegisterAudioSlot(track, ARG(cmd, 0), command_result, ARG(cmd, 1));
    } else {
        command_result = Akao_Cmd_10(D_800B0E00[track]);
        Akao_Cmd_C0_WithSlot(0, 0x7F);
        *cmd->ret = command_result;
        Overlay_RegisterAudioSlot(track, ARG(cmd, 0), command_result, 0x7F);
    }

    return 1;
}

static int Akao_SeekTrackOnly(AkaoCommandArgs *cmd, int mode) {
    int track;
    int seek_result;

    seek_result = CD_SeekToTrack(ARG(cmd, 0), 1, mode, &track, ARG(cmd, 1) < 1);
    if (seek_result == 1) {
        return Akao_FinishSeekBusy(seek_result);
    }

    *cmd->ret = track;
    return 1;
}

static int Akao_StopTrack(AkaoCommandArgs *cmd) {
    int slot;
    int seek_result;

    slot = Overlay_GetAudioSlotByKey(ARG(cmd, 0));
    if (slot == -1) {
        return 1;
    }

    Akao_Cmd_11(slot);
    seek_result = CD_SeekToTrack(ARG(cmd, 0), 0, 0, &slot, 1);
    if (seek_result == 1) {
        return Akao_FinishSeekBusy(seek_result);
    }

    return 1;
}

static int Akao_RegisterExistingSlot(AkaoCommandArgs *cmd, int arity) {
    int slot;
    int command_result;
    int key;

    key = ARG(cmd, 0);
    slot = Overlay_GetAudioSlotByKey(key);
    if (slot == -1) {
        return 1;
    }

    if (arity == 1) {
        Akao_Cmd_C0_WithSlot(slot, ARG(cmd, 1));
        command_result = Overlay_FindAudioSlotIndex(key);
        slot = Overlay_GetAudioSlotByKey(key);
        Overlay_RegisterAudioSlot(command_result, key, slot, ARG(cmd, 1));
    } else if (arity == 2) {
        Akao_Cmd_C1_WithSlot(slot, ARG(cmd, 1) << 1, ARG(cmd, 2));
        command_result = Overlay_FindAudioSlotIndex(key);
        slot = Overlay_GetAudioSlotByKey(key);
        Overlay_RegisterAudioSlot(command_result, key, slot, ARG(cmd, 2));
    } else {
        Akao_Cmd_C2_WithSlot(slot, ARG(cmd, 1) << 1, ARG(cmd, 2), ARG(cmd, 3));
        command_result = Overlay_FindAudioSlotIndex(key);
        slot = Overlay_GetAudioSlotByKey(key);
        Overlay_RegisterAudioSlot(command_result, key, slot, ARG(cmd, 3));
    }

    return 1;
}

static int Akao_FindEntityForCommand(AkaoCommandArgs *cmd, u8 **out) {
    u8 *node;
    int target_a;
    int target_b;

    target_a = ARG(cmd, 1);
    if (target_a == 0) {
        if (D_8009D254 == 0) {
            return 0;
        }
        *out = D_8009D254;
        return 1;
    }

    target_b = ARG(cmd, 2);
    for (node = D_8009D20C; node != 0; node = *(u8 **)(node + 4)) {
        if (node[0x0C] == target_a && node[0x0D] == target_b && ((*(u32 *)(node + 0x98) & 0x10) == 0)) {
            *out = node;
            return 1;
        }
    }

    return 0;
}

static int Akao_FindAssetAtEntity(AkaoCommandArgs *cmd, u8 *entity) {
    int result;

    result = Asset_Find08w(ARG(cmd, 0), 0, *(s16 *)(entity + 0x2A), *(s16 *)(entity + 0x2E), *(s16 *)(entity + 0x32));
    *cmd->ret = result;
    return 1;
}

static int Akao_FindAssetFromArgs(AkaoCommandArgs *cmd) {
    int result;

    result = Asset_Find08w(ARG(cmd, 0), 0, *(s16 *)(ARG_PTR(cmd, 1) + 2), *(s16 *)(ARG_PTR(cmd, 2) + 2),
                           *(s16 *)(ARG_PTR(cmd, 3) + 2));
    *cmd->ret = result;
    return 1;
}

static int Akao_FindArchiveEntry(AkaoCommandArgs *cmd) {
    u8 *base;
    u8 *entry;
    u32 packed;
    int i;
    int count;
    int key;

    base = D_800B0E64;
    packed = *(u32 *)(base + *(u32 *)(base + 4) + 0x30);
    count = packed >> 22;
    if (count == 0) {
        return 1;
    }

    key = ARG(cmd, 0);
    entry = base + (packed & 0x3FFFFF);
    for (i = 0; i < count; i++, entry += 0x0C) {
        if ((entry[3] & 0x10) && *(u16 *)(entry + 0x0A) == key) {
            D_800B0DB8 = key;
            D_800B0DB9 = entry[8];
            D_800B0DFC = base + (*(u32 *)(entry + 4) & 0xFFFFFF);
            return 1;
        }
    }

    return 1;
}

static int Akao_SaveFieldPosition(AkaoCommandArgs *cmd) {
    u8 *entry;

    if (D_800B0CE9 >= 0x10) {
        return 1;
    }

    entry = &D_800944A8[D_800B0CE9 * 8];
    entry[0] = D_8009D2F0[0x0C];
    entry[1] = D_8009D2F0[0x0D];
    entry[2] = ARG(cmd, 0);
    entry[3] = ARG(cmd, 1);
    *(u16 *)(entry + 4) = ARG(cmd, 2);
    if (*cmd->opcode == 0x196) {
        *(u16 *)(entry + 6) = ARG(cmd, 2);
    } else {
        *(u16 *)(entry + 6) = ARG(cmd, 3);
    }
    D_800B0CE9++;

    return 1;
}

int Akao_ProcessCommand(AkaoCommandArgs *cmd) {
    u8 mix[4];
    u8 *entity;

    switch (*cmd->opcode) {
    case 0x64:
    case 0x198:
        Akao_Cmd_99_9B_9D(ARG(cmd, 0));
        return 1;
    case 0x65:
        Akao_Cmd_98_9A_9C(ARG(cmd, 0));
        return 1;
    case 0xC8:
        return Akao_SeekAndStartTrack(cmd, 0, 0);
    case 0xC9:
        return Akao_StopTrack(cmd);
    case 0xCB:
        return Akao_SeekAndStartTrack(cmd, 1, 1);
    case 0xCC:
        Akao_Cmd_90(ARG(cmd, 0));
        return 1;
    case 0xCD:
        return Akao_RegisterExistingSlot(cmd, 1);
    case 0xCE:
        return Akao_RegisterExistingSlot(cmd, 2);
    case 0xCF:
        return Akao_RegisterExistingSlot(cmd, 3);
    case 0xD0:
        Akao_Cmd_D0(ARG(cmd, 0));
        return 1;
    case 0xD1:
        Akao_Cmd_D1(ARG(cmd, 0) << 1, ARG(cmd, 1));
        return 1;
    case 0xD2:
        Akao_Cmd_D2(ARG(cmd, 0) << 1, ARG(cmd, 1), ARG(cmd, 2));
        return 1;
    case 0xD3:
        Akao_Cmd_D4(ARG(cmd, 0));
        return 1;
    case 0xD4:
        Akao_Cmd_D5(ARG(cmd, 0) << 1, ARG(cmd, 1));
        return 1;
    case 0xD5:
        Akao_Cmd_D6(ARG(cmd, 0) << 1, ARG(cmd, 1), ARG(cmd, 2));
        return 1;
    case 0xD6:
        Akao_Cmd_D8(ARG(cmd, 0));
        return 1;
    case 0xD7:
        Akao_Cmd_D9(ARG(cmd, 0) << 1, ARG(cmd, 1));
        return 1;
    case 0xD8:
        Akao_Cmd_DA(ARG(cmd, 0) << 1, ARG(cmd, 1), ARG(cmd, 2));
        return 1;
    case 0xD9:
        Akao_ResetPositionalState();
        return 1;
    case 0x12C:
        *cmd->ret = Akao_SendTableCommand((int)D_800B0E64, ARG(cmd, 0), ARG(cmd, 1), ARG(cmd, 2), ARG(cmd, 3));
        return 1;
    case 0x12D:
        Akao_Cmd_21(ARG(cmd, 0), ARG(cmd, 1));
        return 1;
    case 0x12E:
        Akao_Cmd_A0(ARG(cmd, 0), ARG(cmd, 1), ARG(cmd, 2));
        return 1;
    case 0x12F:
        Akao_Cmd_A1(ARG(cmd, 0), ARG(cmd, 1), ARG(cmd, 2) << 1, ARG(cmd, 3));
        return 1;
    case 0x130:
        Akao_Cmd_A8(ARG(cmd, 0));
        return 1;
    case 0x131:
        Akao_Cmd_A9(ARG(cmd, 0) << 1, ARG(cmd, 1));
        return 1;
    case 0x132:
        Akao_Cmd_A2(ARG(cmd, 0), ARG(cmd, 1), ARG(cmd, 2));
        return 1;
    case 0x133:
        Akao_Cmd_A3(ARG(cmd, 0), ARG(cmd, 1), ARG(cmd, 2) << 3, ARG(cmd, 3));
        return 1;
    case 0x134:
        Akao_Cmd_AA(ARG(cmd, 0));
        return 1;
    case 0x135:
        Akao_Cmd_AB(ARG(cmd, 0) << 1, ARG(cmd, 1));
        return 1;
    case 0x136:
        Akao_Cmd_A4(ARG(cmd, 0), ARG(cmd, 1), ARG(cmd, 2));
        return 1;
    case 0x137:
        Akao_Cmd_A5(ARG(cmd, 0), ARG(cmd, 1), ARG(cmd, 2) << 1, ARG(cmd, 3));
        return 1;
    case 0x138:
        Akao_Cmd_AC(ARG(cmd, 0));
        return 1;
    case 0x139:
        Akao_Cmd_AD(ARG(cmd, 0) << 1, ARG(cmd, 1));
        return 1;
    case 0x13A:
        Akao_Cmd_F1();
        return 1;
    case 0x15E:
        D_800B0DD0 = ARG(cmd, 0);
        D_800B0DCE = ARG(cmd, 1);
        D_800B0DD2 = ARG(cmd, 2);
        D_800B0DCF = ARG(cmd, 3);
        return 1;
    case 0x15F:
        return Akao_FindAssetAtEntity(cmd, D_8009D2F0);
    case 0x160:
        if (!Akao_FindEntityForCommand(cmd, &entity)) {
            return 1;
        }
        return Akao_FindAssetAtEntity(cmd, entity);
    case 0x161:
        return Akao_FindAssetFromArgs(cmd);
    case 0x190:
        return Akao_FindArchiveEntry(cmd);
    case 0x191:
        return Akao_SeekTrackOnly(cmd, 0);
    case 0x192:
        return Akao_SeekTrackOnly(cmd, 1);
    case 0x195:
        D_800B0CEA = 1;
        return 1;
    case 0x196:
    case 0x197:
        return Akao_SaveFieldPosition(cmd);
    case 0x199:
        D_800B0DBE = ARG(cmd, 0);
        mix[0] = ARG(cmd, 0);
        mix[1] = 0;
        mix[2] = ARG(cmd, 0);
        mix[3] = 0;
        Akao_SetCdMixVolume(mix);
        return 1;
    case 0x19A:
        Akao_Cmd_92(ARG(cmd, 0));
        return 1;
    default:
        return 1;
    }
}
