/* CC1_FLAGS: -fno-schedule-insns */
#include "pe1/font.h"

int CdRom_ReadSectorsFromLba(int lba, u8 *dst, int count);
int CdRom_PollReady(void);

int Render_LoadFontGlyph(u8 code)
{
    int status;
    register int offset asm("$2");
    register int sourceOffset asm("$6");
    register int i asm("$5");
    register u8 *source asm("$4");
    register u8 *bytePtr asm("$3");
    FontGlyphLoadState *state;
    u16 *range;
    register int pollError asm("$16");
retry:
    state = &D_800B0DD8;
    range = D_80093176;
    do {
        status = CdRom_ReadSectorsFromLba(state->baseLba + range[0], state->buffer,
                                            range[1] - range[0]);
    } while (status == -1);
    pollError = -1;
poll:
    status = CdRom_PollReady();
    if (status == 0)
        goto copy;
    if (status == pollError)
        goto retry;
    goto poll;
copy:
    offset = code * 328;
    PE1_COMPILER_LAUNDER(offset);
    sourceOffset = offset;
    sourceOffset++;
    PE1_COMPILER_LAUNDER(sourceOffset);
    source = D_800B0E6C;
    i = 0;
    bytePtr = source + sourceOffset;
    PE1_COMPILER_LAUNDER(bytePtr);
    D_8009ECD8.groups.unknown[0] = *bytePtr;
    sourceOffset = offset + 2;
    PE1_COMPILER_LAUNDER(sourceOffset);
    bytePtr = source + sourceOffset;
    PE1_COMPILER_LAUNDER(bytePtr);
    sourceOffset = offset + 3;
    PE1_COMPILER_LAUNDER(sourceOffset);
    D_8009ECD8.groups.unknown[1] = *bytePtr;
    D_8009ECD8.groups.count = source[sourceOffset];
    sourceOffset = offset + 4;
    PE1_COMPILER_LAUNDER(sourceOffset);
    for (i = 0; i < 24; i++, sourceOffset++)
        D_8009ECD8.groups.codes[i] = D_800B0E6C[sourceOffset];
    D_8009ECD8.slots.count = D_800B0E6C[sourceOffset++];
    for (i = 0; i < 100; i++, sourceOffset++)
        D_8009ECD8.slots.indices[i] = D_800B0E6C[sourceOffset];
    for (i = 0; i < 100; i++, sourceOffset++)
        D_8009ECD8.slots.groupKeys[i] = D_800B0E6C[sourceOffset];
    for (i = 0; i < 100; i++, sourceOffset++)
        D_8009ECD8.slots.unknownC9[i] = D_800B0E6C[sourceOffset];
    return 0;
}
