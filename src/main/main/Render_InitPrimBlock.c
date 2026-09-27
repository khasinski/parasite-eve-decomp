#include "pe1/render_object.h"

/* Equivalent branches preserve the stock compiler\'s register allocation.
 * No packet operation depends on which of these identical branches is taken. */

void Render_InitPrimBlock(RenderObjectEntity *object, s16 x, s16 y, int unused, unsigned int palette_row)
{
    unsigned int page;
    int page_offset;
    unsigned int scaled;
    u16 signed_page;
    unsigned int palette_offset;
    unsigned int row_copy;
    s16 half_page;
    int flag_copy;
    RenderPacket34 *quad;
    RenderPacket28 *tri;
    s16 i;
    s16 side;
    half_page = y == 128;
    page = ((((x >> 6) - 8) * 2) + (y >> 7)) + ((y >> 8) * 30);
    signed_page = page;
    row_copy = palette_row;
    quad = (RenderPacket34 *) object->primitive_buffer;
    if (x != 960)
    {
        page_offset = page >> 1;
        scaled = palette_row * 64;
    }
    else
    {
        if (quad->page_bits == 31)
        {
            return;
        }
        if (quad)
        {
            page_offset = ((int) (((unsigned int) signed_page) << 16)) >> 17;
        }
        else
        {
            page_offset = ((int) (((unsigned int) signed_page) << 16)) >> 17;
        }
        if (quad)
        {
            scaled = row_copy * 64;
        }
        else
        {
            scaled = row_copy * 64;
        }
    }
    signed_page = page;
    if (signed_page)
    {
        palette_offset = scaled - 0x7080;
    }
    else
    {
        palette_offset = scaled - 0x7080;
    }
    for (i = 0; i < object->header->packet34_count; i++)
    {
        flag_copy = half_page;
        for (side = 0; side < 2; side++, quad++)
        {
            if (flag_copy)
            {
                u8 v = quad->v0;
                if (v < 128)
                {
                    quad->v0 = v + 128;
                    quad->v1 += 128;
                    quad->v2 += 128;
                    quad->v3 += 128;
                    quad->page_bits += page_offset;
                }
                else
                    if (object)
                {
                    quad->v0 = v + 128;
                    quad->v1 += 128;
                    quad->v2 += 128;
                    quad->v3 += 128;
                    quad->page_bits += page_offset + 1;
                }
                else
                {
                    quad->v0 = v + 128;
                    quad->v1 += 128;
                    quad->v2 += 128;
                    quad->v3 += 128;
                    quad->page_bits += page_offset + 1;
                }
            }
            else
            {
                quad->page_bits += page_offset;
            }
            quad->clut += palette_offset;
        }

    }

    tri = (RenderPacket28 *) quad;
    for (i = 0; i < object->header->packet28_count; i++)
    {
        for (side = 0; side < 2; side++, tri++)
        {
            if (half_page)
            {
                u8 v = tri->v0;
                if (v < 128)
                {
                    tri->v0 = v + 128;
                    tri->v1 += 128;
                    tri->v2 += 128;
                    scaled = palette_row * 64;
                    tri->page_bits += page_offset;
                }
                else
                {
                    tri->v0 = v + 128;
                    tri->v1 = tri->v1 + 128;
                    tri->v2 += 128;
                    tri->page_bits += page_offset + 1;
                }
            }
            else
            {
                tri->page_bits += page_offset;
            }
            tri->clut += palette_offset;
        }

    }

}
