/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/scene_assets.h"
#include "pe1/cdrom.h"
extern u8 D_8009317C[];
extern u32 D_8009D170;
extern u32 D_8009D174;
extern u32 D_8009D178;
extern u32 D_8009D17C;
extern int Spu_SetStreamModeA(void);
extern int Spu_SetStreamModeB(void);
extern int Akao_StepNoteSequencer(void *, u32);
extern int Spu_UploadSampleBlockBlocking(void *, int);
extern int Spu_UploadStreamBlockB(int, void *);
extern int Spu_UploadStreamBlockA(int, void *, u32);
extern int Spu_GetTransferStatus(void);
/* Matching debt: register pins and empty barriers retain the index,
* zero extension and callback state. The duplicated ready branch preserves
* GCC's block ordering. No ASM construct emits an instruction. */
int CD_ReadSectors(unsigned kind, unsigned int index, int channel, void *buffer, u32 maximum, int blocking)
{
    register Pe1GameState *state asm("$18") = &g_GameState;
    int result = -1;
    u32 *archiveLba;
    u32 transferValue;
    int busy = 0;
    unsigned int isSequence;
    register int finished asm("$16") = 0;
    register u32 scaledIndex asm("$5") = index * 2;
    u32 byteOffset;
    SceneSectorDirectory *root = (SceneSectorDirectory *)D_8009317C;
    u8 *entries;
    volatile u16 *entry;
    byteOffset = scaledIndex;
    entries = (u8 *)root->offsets;
    entry = (u16 *)(scaledIndex + entries);
    do
    {
        asm("" : "=r"(blocking), "=r"(kind) : "0"(blocking), "1"(kind));
        archiveLba = &((SceneSectorDirectory *)D_8009317C)->base_lba;
        switch (state->cd_read_phase)
        {
        case 0:
            {
                u8 *table = (u8 *)root;
                u32 first;
                register u32 length asm("$5");
                register u32 start asm("$3");
                entries = table + byteOffset;
                first = *(u16 *)entry;
                asm("" : "=r"(first) : "0"(first), "r"(entries) : "memory");
                length = *(volatile u16 *)(entries + 6);
                asm("" : "=r"(length) : "0"(length) : "memory");
                length -= entry[0];
                transferValue = *archiveLba + first;
                start = state->pe_image_base_lba + transferValue;
                D_8009D174 = length;
                D_8009D178 = D_8009D174;
                isSequence = kind == 0;
                D_8009D170 = start;
                if (isSequence)
                {
                    result = Spu_SetStreamModeA();
                    if (result == (-1))
                    {
                        busy = 1;
                        finished = (blocking ^ 1) & 1;
                    }
                }
                else
                if (kind == 3)
                {
                    result = Spu_SetStreamModeB();
                    if (result == (-1))
                    {
                        busy = 1;
                        finished = (blocking ^ 1) & 1;
                    }
                }
                transferValue = 7;
                state->cd_read_phase = transferValue;
                break;
            }

        case 7:
            if (D_8009D178 != 0)
            {
                u32 count = D_8009D178;
                if (maximum < count)
                {
                    count = maximum;
                }
                D_8009D17C = count;
                result = CdRom_ReadSectors(D_8009D170, D_8009D174 - D_8009D178, buffer, count);
                if (result != (-1))
                {
                    state->cd_read_phase = 8;
                }
                busy = 1;
                finished = (blocking ^ 1) & 1;
            }
            else
            {
                state->cd_read_phase = 0;
                busy = 0;
                finished = 1;
            }
            break;

        case 8:
            result = CdRom_PollReady();
            if (result == (-1))
            {
                transferValue = 7;
                state->cd_read_phase = transferValue;
            }
            else
            if (result == 0)
            {
                if (D_8009D178)
                {
                    state->cd_read_phase = 9;
                    break;
                }
                else
                {
                    state->cd_read_phase = 9;
                    break;
                }
            }
            busy = 1;
            finished = (blocking ^ 1) & 1;
            break;

        case 9:
            switch (kind)
            {
            case 0:
                result = Akao_StepNoteSequencer(buffer, D_8009D17C << 11);
                break;

            case 1:
                result = Spu_UploadSampleBlockBlocking(buffer, 0);
                break;

            case 2:
                result = Spu_UploadStreamBlockB(channel, buffer);
                break;

            case 3:
                result = Spu_UploadStreamBlockA(channel, buffer, D_8009D17C << 11);
                break;

            }

            if (result == (-1))
            {
                state->cd_read_phase = 0;
                busy = 1;
                finished = (blocking ^ 1) & 1;
            }
            else
            {
                state->cd_read_phase = 10;
            }
            break;

        case 10:
            result = Spu_GetTransferStatus();
            if (result == -1) {
                state->cd_read_phase = 0;
                busy = 1;
                finished = (blocking ^ 1) & 1;
            } else if (result != 0) {
                busy = 1;
                finished = (blocking ^ 1) & 1;
            } else {
                u32 remaining, remainingSnapshot;
                u32 transferred;
                register u32 nextPhase asm("$3");
                busy = 1;
                remainingSnapshot = D_8009D178;
                finished = (blocking ^ 1) & 1;
                asm("" : : "r"(busy), "r"(finished) : "memory");
                remaining = remainingSnapshot;
                transferred = D_8009D17C;
                nextPhase = 7;
                state->cd_read_phase = nextPhase;
                D_8009D178 = remaining - transferred;
            }
            break;
        }
    } while (!finished);
    return busy;
}

int CD_FindNextDataSector(void)
{
    Pe1GameState *state = &g_GameState;
    u8 *base = (u8 *)state->loaded_scene_assets;
    CdArchiveHeader *header = (CdArchiveHeader *)(base + ((CdArchiveRoot *)base)->directoryOffset);
    CdRange *ranges = (CdRange *)(base + (header->range_info & 0x3fffff));

dispatch:
    switch (state->cd_range_state) {
        case 0:
            D_8009CDCC = 0;
            state->cd_range_state = 0x28;
            goto dispatch;
        case 0x28:
        {
            if (CD_ReadSectors(1, 1, 0, state->scene_load_scratch, 0x21, 0) == 1) return 1;
            if (state->cd_range_read_mode >= 2) {
                state->cd_range_state = 0x29;
                return 1;
            }
            goto set_scan_state;
        }
        case 0x29:
        {
            if (CD_ReadSectors(1, state->cd_range_read_mode, 0, state->scene_load_scratch, 0x21, 0) == 1) return 1;
set_scan_state:
            state->cd_range_state = 0x2a;
            goto dispatch;
        }
        case 0x2a:
        {
            int index;
            CdRange *entry;
            int byte_offset;
            index = D_8009CDCC;
            if (index < (int)(header->range_info >> 22)) {
                byte_offset = index * sizeof(CdRange);
                entry = (CdRange *)(byte_offset + (u32)ranges);
                if (entry->flags & 0x10) {
                    if (entry->first >= 2) goto read_range;
                }
                D_8009CDCC = index + 1;
                goto dispatch;
read_range:
                state->cd_range_state = 0x2b;
                goto dispatch;
            } else {
                state->cd_range_state = 0;
                return 0;
            }
        }
        case 0x2b:
        {
            CdRange *entry = (CdRange *)(D_8009CDCC * sizeof(CdRange) + (u32)ranges);
            if (CD_ReadSectors(3, entry->first, entry->last, state->scene_load_scratch, 0x21, 0) == 1) return 1;
            {
                state->cd_range_state = 0x2a;
                D_8009CDCC = D_8009CDCC + 1;
            }
            goto dispatch;
        }
        default:
            return 0;
    }
}
