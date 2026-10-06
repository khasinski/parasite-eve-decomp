/* CC1_FLAGS: -fno-strength-reduce -fno-expensive-optimizations */
#include "pe1/geom_state.h"
#include "pe1/render_prim.h"
#include "pe1/psyq_gpu.h"

/* Build both frame buffers of room-tile sprites and texture-page packets. */
int Geo_LoadMeshEntry(GeomEntry *entry, RenderTilePacket *buffer, void **end)
{
    register unsigned int paletteStart asm("$5") = D_800BD024;
    unsigned int count = entry->prim_count;
    u32 *positions = (u32 *)((u8 *)entry + entry->u28.pos_ptr);
    RenderTileTexture *textures = (RenderTileTexture *)((u8 *)entry + entry->u2C.uv_ptr);
    unsigned int pass = 0;
    RenderTexturePagePacket *pages = (RenderTexturePagePacket *)(buffer + count * 2);
    unsigned int shade = 128;
    int paletteMax = paletteStart + 479;

    entry->u30.prim = buffer;
    /* Byte-storage view retains the retail alias scheduling for this pointer
     * store. Direct u34.pagePackets assignment changes twelve instructions. */
    *(RenderTexturePagePacket **)entry->u34.storage = pages;
    entry->disp_x = entry->scr_x + D_800B1624->out_disp_x;
    asm("" : "=m"(entry->disp_x) : "m"(entry->disp_x));
    entry->disp_y = entry->scr_y + D_800B1624->out_disp_y;

    do {
        unsigned int i = 0;
        /* Keep the relational guard: GCC's eliminated comparison leaves a
         * reload slot that is part of the retail 104-byte stack frame. */
        if (i < count) {
            RenderTileTexture *texture = textures;
            u32 *position = positions;
            RenderTexturePagePacket *page = pages;
            RenderTilePacket *sprite = buffer;
            do {
                sprite->length = 3;
                sprite->code = 0x7c;
                sprite->r = shade;
                sprite->g = shade;
                sprite->b = shade;
                sprite->x = entry->disp_x + (*position >> 22);
                sprite->y = entry->disp_y + ((*position >> 12) & 1023);
                sprite->u = texture->bytes[4];
                sprite->v = texture->bytes[3];
                if (texture->words[1] & 0x10000000)
                    sprite->code |= 2;
                else
                    sprite->code &= ~2;
                sprite->clut = GetClut((texture->words[0] >> 5) & 0x3f0,
                                      texture->words[0] & 511);
                if (paletteMax < (int)(texture->words[0] & 511))
                    paletteMax = texture->words[0] & 511;
                page->length = 1;
                {
                    u32 word = texture->words[0];
                    page->command = (GetTPage(1, (word >> 22) & 3,
                                             (word >> 10) & 0x3c0,
                                             (word >> 7) & 256) & 0x9ff) | 0xe1000600;
                }
                texture++;
                position++;
                sprite++;
                page++;
            } while (++i < count);
        }
        buffer += count;
        pages += count;
    } while (++pass < 2);
    *end = pages;
    D_800BD024 = paletteMax + 33;
    return 0;
}
