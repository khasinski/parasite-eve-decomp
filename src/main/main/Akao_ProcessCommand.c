#include "common.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */


#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define S8_AT(ptr, off) (*(s8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

typedef struct AkaoCommandArgs {
    int *opcode;
    int *arg1;
    int *arg2;
    int *arg3;
    int *arg4;
    int *result;
} AkaoCommandArgs;

typedef struct DeferredAkaoCommand {
    u8 actor_type;
    u8 actor_id;
    s8 command;
    s8 arg1;
    s16 arg2;
    s16 arg3;
} DeferredAkaoCommand;

typedef struct AkaoAssetEntry {
    u8 pad0[3];
    u8 flags;
    u32 data_offset;
    u8 value8;
    u8 pad9;
    unsigned int id : 16;
} AkaoAssetEntry;

typedef struct AkaoStackLocals {
    int slot;
    u8 pad4[4];
    s8 volumes[4];
    u8 pad12[8];
} AkaoStackLocals;

typedef struct AkaoRuntimeState {
    u8 reset_pending;
    u8 deferred_count;
    s8 defer_enabled;
    u8 pad003[0xCD];
    s8 selected_id;
    u8 selected_value;
    u8 pad0D2[4];
    s8 cd_volume;
    u8 pad0D7[0xF];
    volatile s8 position_x;
    volatile s8 position_z;
    volatile s16 position_y;
    volatile s16 position_w;
    u8 pad0EC[0x28];
    u8 *selected_data;
    u8 *voice_banks[25];
    u8 *archive;
} AkaoRuntimeState;

extern u8 D_800944A8[];
extern u8 *D_8009CE00;
extern u8 *D_8009D20C[];
extern u8 *D_8009D254[];
extern u8 *D_8009D2F0[];
extern u8 *D_8009D300;
extern AkaoRuntimeState g_AkaoState __asm__("D_800B0CE8");
extern AkaoRuntimeState akao_state_command __asm__("D_800B0CE8");
extern AkaoRuntimeState akao_state_header __asm__("D_800B0CE8");
extern AkaoRuntimeState akao_state_base __asm__("D_800B0CE8");
extern AkaoRuntimeState akao_state_result __asm__("D_800B0CE8");

void Akao_Cmd_11(int slot);
void Akao_Cmd_21(int command, int arg1);
void Akao_Cmd_90(int command);
void Akao_Cmd_92(int command);
void Akao_Cmd_98_9A_9C(int command);
void Akao_Cmd_99_9B_9D(int command);
void Akao_Cmd_A0(int command, int arg1, int arg2);
void Akao_Cmd_A1(int command, int arg1, int arg2, int arg3);
void Akao_Cmd_A2(int command, int arg1, int arg2);
void Akao_Cmd_A3(int command, int arg1, int arg2, int arg3);
void Akao_Cmd_A4(int command, int arg1, int arg2);
void Akao_Cmd_A5(int command, int arg1, int arg2, int arg3);
void Akao_Cmd_A8(int command);
void Akao_Cmd_A9(int command, int arg1);
void Akao_Cmd_AA(int command);
void Akao_Cmd_AB(int command, int arg1);
void Akao_Cmd_AC(int command);
void Akao_Cmd_AD(int command, int arg1);
void Akao_Cmd_C0_WithSlot(int slot, int value);
void Akao_Cmd_C1_WithSlot(int slot, int value, int arg2);
void Akao_Cmd_C2_WithSlot(int slot, int value, int arg2, int arg3);
void Akao_Cmd_D0(int command);
void Akao_Cmd_D1(int command, int arg1);
void Akao_Cmd_D2(int command, int arg1, int arg2);
void Akao_Cmd_D4(int command);
void Akao_Cmd_D5(int command, int arg1);
void Akao_Cmd_D6(int command, int arg1, int arg2);
void Akao_Cmd_D8(int command);
void Akao_Cmd_D9(int command, int arg1);
void Akao_Cmd_DA(int command, int arg1, int arg2);
void Akao_Cmd_F1(void);
void Akao_ResetPositionalState(void);
void Akao_SetCdMixVolume(s8 *volumes);
int Akao_Cmd_10(u8 *data, int status);
int Akao_Cmd_19_Then_C0(u8 *data, int value);
int Akao_SendTableCommand(u8 *base, int command, int arg1, int arg2,
                          int arg3);
int Asset_Find08w(int id, int mode, int x, int y, int z);
int CD_SeekToTrack(int key, int mode, int group, int *slot, int immediate);
int CD_SeekTrack0(int key, int mode, int group, int *slot, int immediate)
    __asm__("CD_SeekToTrack");
int CD_SeekTrack1(int key, int mode, int group, int *slot, int immediate)
    __asm__("CD_SeekToTrack");
int Overlay_FindAudioSlotIndex(int key);
int Overlay_GetAudioSlotByKey(int key);
void Overlay_RegisterAudioSlot(int slot, int key, int handle, int value);

int Akao_ProcessCommand(AkaoCommandArgs *args) {
    AkaoStackLocals locals;
    int status;
    int handle;
    int wait_one;
    int slot_index0;
    int slot_index1;
    int slot_index2;
    int count;
    u32 entry_count;
    u32 entry_limit;
    int volume;
    u32 table_info;
    u8 *actor;
    u8 *base;
    u8 *player;
    AkaoAssetEntry *current;
    AkaoAssetEntry *entries;
    DeferredAkaoCommand *deferred;
    u8 *deferred_count;

    switch (*args->opcode) {
    case 0x64:
        Akao_Cmd_99_9B_9D(*args->arg1);
        goto done;
    case 0x65:
        Akao_Cmd_98_9A_9C(*args->arg1);
        goto done;
    case 0xC8:
    case 0x198:
        status = CD_SeekToTrack(*args->arg1, 1, 0, &locals.slot,
                                *args->arg2 == 0);
        if (status == 1) {
            goto wait_for_cd;
        }
        if ((u32)(locals.slot + 2) < 2) {
            goto done;
        }
        *args->result = Akao_Cmd_10(g_AkaoState.voice_banks[locals.slot], status);
        Akao_Cmd_C0_WithSlot(0, 0x7F);
        Overlay_RegisterAudioSlot(locals.slot, *args->arg1, *args->result, 0x7F);
        goto done;
    case 0xCB:
        status = CD_SeekToTrack(*args->arg1, 1, 1, &locals.slot,
                                *args->arg3 == 0);
        if (status == 1) {
            goto wait_for_cd;
        }
        if ((u32)(locals.slot + 2) < 2) {
            goto done;
        }
        *args->result = Akao_Cmd_19_Then_C0(
            g_AkaoState.voice_banks[locals.slot], *args->arg2);
        Overlay_RegisterAudioSlot(locals.slot, *args->arg1, *args->result,
                                  *args->arg2);
        goto done;
    case 0xC9:
        locals.slot = Overlay_GetAudioSlotByKey(*args->arg1);
        if (locals.slot == -1) {
            goto done;
        }
        Akao_Cmd_11(locals.slot);
        wait_one = 1;
        status = CD_SeekToTrack(*args->arg1, 0, 0, &locals.slot, wait_one);
        if (status != wait_one) {
            goto done;
        }
        goto wait_for_cd;
    case 0xCC:
        Akao_Cmd_90(*args->arg1);
        goto done;
    case 0xCD:
        locals.slot = Overlay_GetAudioSlotByKey(*args->arg1);
        if (locals.slot == -1) {
            goto done;
        }
        Akao_Cmd_C0_WithSlot(locals.slot, *args->arg2);
        slot_index0 = Overlay_FindAudioSlotIndex(*args->arg1);
        handle = Overlay_GetAudioSlotByKey(*args->arg1);
        Overlay_RegisterAudioSlot(slot_index0, *args->arg1, handle,
                                  *args->arg2);
        goto done;
    case 0xCE:
        locals.slot = Overlay_GetAudioSlotByKey(*args->arg1);
        if (locals.slot == -1) {
            goto done;
        }
        Akao_Cmd_C1_WithSlot(locals.slot, *args->arg2 * 2, *args->arg3);
        slot_index1 = Overlay_FindAudioSlotIndex(*args->arg1);
        handle = Overlay_GetAudioSlotByKey(*args->arg1);
        Overlay_RegisterAudioSlot(slot_index1, *args->arg1, handle,
                                  *args->arg3);
        goto done;
    case 0xCF:
        locals.slot = Overlay_GetAudioSlotByKey(*args->arg1);
        if (locals.slot == -1) {
            goto done;
        }
        Akao_Cmd_C2_WithSlot(locals.slot, *args->arg2 * 2, *args->arg3,
                             *args->arg4);
        slot_index2 = Overlay_FindAudioSlotIndex(*args->arg1);
        handle = Overlay_GetAudioSlotByKey(*args->arg1);
        Overlay_RegisterAudioSlot(slot_index2, *args->arg1, handle,
                                  *args->arg4);
        goto done;
    case 0xD0:
        Akao_Cmd_D0(*args->arg1);
        goto done;
    case 0xD1:
        Akao_Cmd_D1(*args->arg1 * 2, *args->arg2);
        goto done;
    case 0xD2:
        Akao_Cmd_D2(*args->arg1 * 2, *args->arg2, *args->arg3);
        goto done;
    case 0xD3:
        Akao_Cmd_D4(*args->arg1);
        goto done;
    case 0xD4:
        Akao_Cmd_D5(*args->arg1 * 2, *args->arg2);
        goto done;
    case 0xD5:
        Akao_Cmd_D6(*args->arg1 * 2, *args->arg2, *args->arg3);
        goto done;
    case 0xD6:
        Akao_Cmd_D8(*args->arg1);
        goto done;
    case 0xD7:
        Akao_Cmd_D9(*args->arg1 * 2, *args->arg2);
        goto done;
    case 0xD8:
        Akao_Cmd_DA(*args->arg1 * 2, *args->arg2, *args->arg3);
        goto done;
    case 0xD9:
        Akao_ResetPositionalState();
        goto done;
    case 0x12C:
        *args->result = Akao_SendTableCommand(
            akao_state_command.archive, *args->arg1, *args->arg2, *args->arg3,
            *args->arg4);
        goto done;
    case 0x12D:
        Akao_Cmd_21(*args->arg1, *args->arg2);
        goto done;
    case 0x12E:
        Akao_Cmd_A0(*args->arg1, *args->arg2, *args->arg3);
        goto done;
    case 0x12F:
        Akao_Cmd_A1(*args->arg1, *args->arg2, *args->arg3 * 2,
                    *args->arg4);
        goto done;
    case 0x130:
        Akao_Cmd_A8(*args->arg1);
        goto done;
    case 0x131:
        Akao_Cmd_A9(*args->arg1 * 2, *args->arg2);
        goto done;
    case 0x132:
        Akao_Cmd_A2(*args->arg1, *args->arg2, *args->arg3);
        goto done;
    case 0x133:
        Akao_Cmd_A3(*args->arg1, *args->arg2, *args->arg3 * 8,
                    *args->arg4);
        goto done;
    case 0x134:
        Akao_Cmd_AA(*args->arg1);
        goto done;
    case 0x135:
        Akao_Cmd_AB(*args->arg1 * 2, *args->arg2);
        goto done;
    case 0x136:
        Akao_Cmd_A4(*args->arg1, *args->arg2, *args->arg3);
        goto done;
    case 0x137:
        Akao_Cmd_A5(*args->arg1, *args->arg2, *args->arg3 * 2,
                    *args->arg4);
        goto done;
    case 0x138:
        Akao_Cmd_AC(*args->arg1);
        goto done;
    case 0x139:
        Akao_Cmd_AD(*args->arg1 * 2, *args->arg2);
        goto done;
    case 0x13A:
        Akao_Cmd_F1();
        goto done;
    case 0x15E:
        g_AkaoState.position_y = U32_AT(PTR_AT(args, 4), 0);
        g_AkaoState.position_x = U32_AT(PTR_AT(args, 8), 0);
        g_AkaoState.position_w = U32_AT(PTR_AT(args, 0xC), 0);
        g_AkaoState.position_z = U32_AT(PTR_AT(args, 0x10), 0);
        goto done;
    case 0x15F:
        *args->result = Asset_Find08w(
            *args->arg1, 0, S16_AT(D_8009D2F0[0], 0x2A),
            S16_AT(D_8009D2F0[0], 0x2E), S16_AT(D_8009D2F0[0], 0x32));
        goto done;
    case 0x160:
        if (*args->arg2 == 0) {
            player = D_8009D254[0];
            if (player == 0) {
                goto done;
            }
            actor = player;
            goto find_actor_asset;
        } else {
            actor = D_8009D20C[0];
            while (actor != 0) {
                if (U8_AT(actor, 0xC) == *args->arg2 &&
                    U8_AT(actor, 0xD) == *args->arg3 &&
                    !(U32_AT(actor, 0x98) & 0x10)) {
                    break;
                }
                actor = PTR_AT(actor, 4);
            }
            if (actor != 0) {
find_actor_asset:
                *args->result = Asset_Find08w(
                    *args->arg1, 0, S16_AT(actor, 0x2A),
                    S16_AT(actor, 0x2E), S16_AT(actor, 0x32));
                goto done;
            }
        }
        goto done;
    case 0x161:
        *args->result = Asset_Find08w(*args->arg1, 0,
                                     S16_AT(args->arg2, 2),
                                     S16_AT(args->arg3, 2),
                                     S16_AT(args->arg4, 2));
        goto done;
    case 0x190:
        base = akao_state_base.archive;
        table_info =
            U32_AT(base + U32_AT(akao_state_header.archive, 4), 0x30);
        base += table_info & 0x3FFFFF;
        count = 0;
        entry_count = table_info >> 22;
        if (entry_count == 0) {
            goto done;
        }
        entry_limit = entry_count;
        entries = (AkaoAssetEntry *)base;
        do {
            current = &entries[count];
            if (current->flags & 0x10) {
                int entry_id = current->id;
                if (entry_id == *args->arg1) {
                    goto archive_match;
                }
            }
        } while (++count < entry_limit);
        goto done;
    case 0x191:
        status = CD_SeekTrack0(*args->arg1, 1, 0, &locals.slot,
                               *args->arg2 == 0);
        if (status == 1) {
            goto wait_for_cd;
        }
        goto store_audio_slot;
    case 0x192:
        status = CD_SeekTrack1(*args->arg1, 1, 1, &locals.slot,
                               *args->arg2 == 0);
        if (status == 1) {
            goto wait_for_cd;
        }
        goto store_audio_slot;
wait_for_cd:
        D_8009CE00 -= 0x20;
        U32_AT(D_8009D300, 0x10) = status;
        return 0;
store_audio_slot:
        *args->result = locals.slot;
        goto done;
    case 0x195:
        g_AkaoState.defer_enabled = 1;
        goto done;
    case 0x196:
    case 0x197:
        if (g_AkaoState.deferred_count < 0x10) {
            deferred = (DeferredAkaoCommand *)(
                D_800944A8 + g_AkaoState.deferred_count * 8);
            deferred->actor_type = U8_AT(D_8009D2F0[0], 0xC);
            deferred->actor_id = U8_AT(D_8009D2F0[0], 0xD);
            deferred->command = *args->arg1;
            deferred->arg1 = *args->arg2;
            deferred->arg2 = *args->arg3;
            if (*args->opcode == 0x196) {
                deferred->arg3 = *args->arg3;
            } else {
                deferred->arg3 = *args->arg4;
            }
            deferred_count = &g_AkaoState.deferred_count;
            (*deferred_count)++;
        }
        goto done;
    case 0x199:
        volume = *args->arg1;
        locals.volumes[3] = 0;
        locals.volumes[1] = 0;
        g_AkaoState.cd_volume = volume;
        locals.volumes[2] = volume;
        locals.volumes[0] = volume;
        Akao_SetCdMixVolume(locals.volumes);
        goto done;
archive_match:
        g_AkaoState.selected_id = current->id;
        g_AkaoState.selected_value = current->value8;
        g_AkaoState.selected_data = akao_state_result.archive +
                                    (current->data_offset & 0xFFFFFF);
        goto done;
    case 0x19A:
        Akao_Cmd_92(*args->arg1);
        goto done;
    }
    goto done;
done:
    return 1;
}
