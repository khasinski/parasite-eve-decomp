/* CC1_FLAGS: -fno-schedule-insns */
#include "pe1/font.h"
#include "pe1/cdrom.h"

int Render_LoadFontGlyph(u8 code)
{
    int status;
    int offset;
    int sourceOffset;
    int i;
    u8 *source;
    u8 *bytePtr;
    FontGlyphLoadState *state;
    u16 *range;
    int pollError;
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
    offset = code * sizeof(FontGlyphRecord);
    sourceOffset = offset + PE1_OFFSETOF(FontGlyphTable, groups.unknown[0]);
    source = D_800B0E6C;
    i = 0;
    bytePtr = source + sourceOffset;
    D_8009ECD8.groups.unknown[0] = *bytePtr;
    sourceOffset = offset + PE1_OFFSETOF(FontGlyphTable, groups.unknown[1]);
    bytePtr = source + sourceOffset;
    sourceOffset = offset + PE1_OFFSETOF(FontGlyphTable, groups.count);
    D_8009ECD8.groups.unknown[1] = *bytePtr;
    D_8009ECD8.groups.count = source[sourceOffset];
    sourceOffset = offset + PE1_OFFSETOF(FontGlyphTable, groups.codes);
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
