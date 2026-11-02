/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/boot_disc_check.h"
#include "pe1/field_pad.h"
#include "pe1/menu_flags.h"
#include "pe1/psyq_bios.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_prim.h"
#include "pe1/textbox.h"
#include "pe1/textbox_open.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_ScriptCollisionFlag;
u8 *g_TextboxMessageSource;
unsigned char g_TextboxKernGlyph0F;
TextboxRect g_TextboxPendingRect;
u8 g_DialogueChoiceCursor;
u8 g_PlayerMovementEnabled;
s8 g_TextboxNameGlyphCount;

/* A draw-mode word pair followed by the sprite primitive it prefixes. */
typedef struct GpuModeSprtPacket {
    u32 tag;
    u32 mode;
    u8 sprt[1];
} GpuModeSprtPacket;

/* A draw-mode word pair followed by the tile primitive it prefixes. */
typedef struct GpuModeTilePacket {
    u32 tag;
    u32 mode;
    u8 tile[1];
} GpuModeTilePacket;

void Draw_InitDrawModeSprtPacket(GpuModeSprtPacket *packet, int tpage) {
    u8 *prim = packet->sprt;

    SetDrawTPage(packet, 0, 1, tpage);
    SetSprt(prim);
    if (MargePrim(packet, prim) != 0) {
        exit(-1);
    }
}

void Draw_InitDrawModeTilePacket(GpuModeTilePacket *packet, int tpage) {
    u8 *prim = packet->tile;

    SetDrawTPage(packet, 0, 1, tpage);
    SetTile(prim);
    if (MargePrim(packet, prim) != 0) {
        exit(-1);
    }
}

void Menu_SetEquipSlotIndex(int index) {
    g_TextboxKernGlyph0F = index;
}

void SetDrawTPage(void *, int, int, int);
void SetSprt(void *);
int MargePrim(void *, void *);
void SetShadeTex(void *, int);
void SetSemiTrans(void *, int);
void exit(int);

void Render_SetupFogLayer(void *source) {
    int i;
    u16 *page_source;
    u8 *tile_base;
    register u8 *draw_base asm("$23");
    u8 *tile_payload_base;
    int sentinel;
    u8 *sprite_base;
    int offset;
    u8 *draw_mode;
    u8 *sprite;
    RenderSpritePacket *sprite_fields;
    u8 *tile_mode;
    u8 *tile;
    u8 *payload;
    u32 flags;
    int tpage;
    int color;
    int zero_arg;
    int x_arg;
    int y_arg;
    int dither_enabled;
    u32 mask1;
    u32 mask2;
    u32 mask3;
    u32 mask4;
    u32 mask5;
    i = 0;
    mask1 =0xFFF0FFFF;
    mask2 =0xFFEFFFFF;
    mask3 =0xFFDFFFFF;
    mask4 =0xFE3FFFFF;
    sprite_base = (u8 *)&g_TextboxChoiceCursor[0].sprite;
    mask5 =0xFDFFFFFF;
    sentinel = -1;
    for (; (u8)i < 4; ++i) {
        g_TextboxEntries[(u8)i].control.parts.delay = 0;
        g_TextboxEntries[(u8)i].control.parts.elapsed = 0;
        flags = g_TextboxEntries[(u8)i].control.flags;
        g_TextboxEntries[(u8)i].state = 0;
        g_TextboxEntries[(u8)i].message = 0;
        g_TextboxEntries[(u8)i].style = 0;
        g_TextboxEntries[(u8)i].background = 0;
        g_TextboxEntries[(u8)i].page_id = sentinel;
        flags &= mask1;
        flags &= mask2;
        flags &= mask3;
        flags &= mask4;
        flags &= mask5;
        g_TextboxEntries[(u8)i].control.flags = flags;
    }
    i = 0;
    page_source = &g_TextboxFontPages[3].tpage;
    draw_base = (u8 *)g_TextboxChoiceCursor;
    tile_base = g_TextboxFogTilePackets;
    tile_payload_base = tile_base + 8;
    g_DialogueChoiceCursor = 0;
    g_PlayerMovementEnabled = 0;
    g_ScriptCollisionFlag = 0;
    g_TextboxMessageSource = source;
    for (; (u8)i < 2; ++i) {
        {
            TextboxTableAddress at;

            at.bytes = draw_base;
            at.address = (u8)i * 28 + at.address;
            draw_mode = at.bytes;
        }
        sprite = draw_mode + 8;
        SetDrawTPage(draw_mode, 0, 1, page_source[0]);
        SetSprt(sprite);
        if (MargePrim(draw_mode, sprite)) exit(-1);

        sprite_fields = (RenderSpritePacket *)(((u8)i * 28) + (int)sprite_base);
        SetShadeTex(sprite_fields, 1);
        x_arg = 0;

        y_arg = 0;

        zero_arg = 0;

        sprite_fields->u = 0x70;
        color = *(page_source - 3);
        sprite_fields->width = 0x18;
        sprite_fields->height = 0xC;
        sprite_fields->v = color;
        *(u16 *)((u8)i * 28 + (u8 *)&g_TextboxChoiceCursor[0].sprite.clut) = page_source[1];
        mask1 = x_arg;
        tpage = GetTPage(mask1, y_arg, zero_arg, 0);
        {
            TextboxTableAddress at;

            at.bytes = tile_base;
            at.address = (u8)i * 24 + at.address;
            tile_mode = at.bytes;
        }
        tile = tile_mode + 8;
        SetDrawTPage(tile_mode, 0, 1, tpage & 0xFFFF);
        SetTile(tile);
        if (MargePrim(tile_mode, tile)) exit(-1);
        payload = (u8 *)(((u8)i * 24) + (int)tile_payload_base);
        dither_enabled = 1;
        {
            int two = 2;

            payload[4] = two;
            payload[5] = two;
            payload[6] = two;
            ((RenderColorTilePacket *)payload)->w = 0x140;
            ((RenderColorTilePacket *)payload)->h = 0x36;
            ((RenderColorTilePacket *)payload)->x0 = 0;
            ((RenderColorTilePacket *)payload)->y0 = 0xAA;
            SetSemiTrans(payload, dither_enabled);
        }
    }
}

void Menu_SetTextCursorRect(int x, int y, int w, int h) {
    g_TextboxPendingRect.x = x;
    g_TextboxPendingRect.y = y;
    g_TextboxPendingRect.width = w;
    g_TextboxPendingRect.height = h;
}

void Draw_CloseTextboxPage(int arg0) {
    int i;
    int idx;

    i = 0;
    arg0 = (s16)arg0;
    while ((unsigned char)i < 4) {
        idx = (unsigned char)i;
        if (g_TextboxEntries[idx].page_id == arg0) {
            if (g_TextboxEntries[idx].state != 0) {
                g_TextboxEntries[idx].state = 0;
                break;
            }
        }
        i++;
    }
}

void Draw_ResetTextboxes(void) {
    int i;
    int idx;
    u32 value;

    for (i = 0; (unsigned char)i < 4; i++) {
        idx = (unsigned char)i;
        value = g_TextboxEntries[idx].control.flags;
        g_TextboxEntries[idx].state = 0;
        value &= 0xFDFFFFFF;
        g_TextboxEntries[idx].control.flags = value;
    }
}

s8 Draw_GetTextboxPageState(int arg0) {
    int i;
    int idx;
    int value;

    value = 0;
    i = 0;
    arg0 = (s16)arg0;
    while ((unsigned char)i < 4) {
        idx = (unsigned char)i;
        if (g_TextboxEntries[idx].page_id == arg0) {
            value = g_TextboxEntries[idx].state;
            break;
        }
        i++;
    }
    return value;
}

void Script_EnableMovement(void) {
    g_PlayerMovementEnabled = 1;
}

void Script_DisableMovement(void) {
    g_PlayerMovementEnabled = 0;
}

void Script_SetCollisionFlag(int value) {
    g_ScriptCollisionFlag = value != 0;
}

/* Opens the first free textbox for message `index`. A styled box takes the
 * current text rectangle; `values` (ended by -1, at most five) fill the
 * message's number slots as decimal digits.
 * Matching debt: five register pins and three empty barriers. The reciprocal
 * operand preserves constant-hoist order; the digit memory operand keeps its
 * base across the inner loop; the final count operand preserves the counter.
 * The page-index register is reused for slot initialization. No CPU ASM. */
void Render_SetupColorTable(int inputIndex, int inputStyle, short *inputValues)
{
    unsigned char style = inputStyle;
    register short *values asm("$12") = inputValues;
    unsigned char i;
    register short index asm("$9") = inputIndex;

    for (i = 0; i < 4; i++) {
        unsigned char slot;

        if (g_TextboxEntries[i].state != 0)
            continue;

        g_TextboxEntries[i].state = 1;
        g_TextboxEntries[i].background = 0;
        g_TextboxEntries[i].page_id = index;
        g_DialogueChoiceCursor = 0;
        {
            int terminator = -1;
            g_MenuEquipSlotIndex = terminator;
        }
        g_TextboxEntries[i].style = style;
        g_TextboxEntries[i].control.flags &= ~0x100000;
        {
            int reciprocal = 0x66666667;
            asm("" : : "r"(reciprocal));
        }
        g_TextboxEntries[i].control.flags &= ~0x200000;
        if (style) {
            g_TextboxEntries[i].x = g_TextboxPendingRect.x;
            g_TextboxEntries[i].y = g_TextboxPendingRect.y;
            g_TextboxEntries[i].width = g_TextboxPendingRect.width;
            g_TextboxEntries[i].height = g_TextboxPendingRect.height;
            if (g_TextboxEntries[i].style == 3)
                g_TextboxEntries[i].control.flags |= 0x100000;
        } else if (g_PlayerMovementEnabled) {
            g_TextboxEntries[i].background = 1;
        }

        index = 0;
        for (slot = index; slot < 5; slot++) {
            register unsigned short inputNumber asm("$7") = *values++;
            short value = inputNumber;
            register unsigned short quotient asm("$5");
            register unsigned char count asm("$8");

            if (value == -1)
                return;
            count = 0;
            g_TextboxEntries[i].numbers[slot].digits[count] = value - (quotient = value / 10) * 10;
            asm volatile("" : : "m"(g_TextboxEntries[i].numbers[slot].digits[count]) : "$6");
            value = quotient;
            while (value != 0) {
                unsigned short quotient;
                quotient = value / 10;
                count++;
                g_TextboxEntries[i].numbers[slot].digits[count] = value - quotient * 10;
                value = quotient;
            }
            g_TextboxEntries[i].numbers[slot].count = count + 1;
            asm("" : : "r"(count));
        }
        return;
    }
}

int Menu_GetEquipSlotIndex(void) {
    return g_MenuEquipSlotIndex;
}

/* Tentative small-data definition: stock maspsx must recognize this store
 * as GP-relative to retain its load-delay nop. The linker pins the COMMON
 * symbol to the retail address, as for other unplaced game globals. */
signed char g_MenuEquipSlotIndex;
u8 g_TextboxGlyphColorR;
u8 g_TextboxGlyphColorG;
u8 g_TextboxGlyphColorB;

void Menu_DrawTextboxEntries(void) {
    TextboxPoint cursor;
    TextboxPoint uv;
    u8 textboxIndex;
    u8 sectionIndex;
    u8 backgroundDrawn;
    /* Matching debt: the exit barrier reserves the unused 0x38 stack slot. */
    long long matchingStackSlot;
    /* Matching debt: four register bindings and six empty barriers preserve
     * the retail allocation. No instruction bodies are embedded in C. */
    register s32 nameIndex asm("$22");
    u32 *orderingTable;
    TextboxFontPage *font;
    register u32 *linkOrdering asm("$4");
    TextboxNumber *number;
    u8 stopDrawing;
    u16 baseX;
    s32 sectionMask;
    u16 glyphAdvance;
    u16 fontClut;
    register TextboxGlyphPacket *linkPacket asm("$5");
    unsigned char lineHeight;
    s32 divisionHintfb;
    s32 divisionHintfc;
    u8 glyphWidth;
    s32 temp_a0;
    void *initialBufferTable;
    s32 selectionOffset;
    s32 selectionFrame;
    s32 temp_v1_7;
    s32 temp_v1_9;
    s32 var_v0_2;
    s32 nextNameIndex;
    s8 temp_v0_4;
    s8 digitIndex;
    u16 temp_t1;
    u16 temp_v1_10;
    u16 var_v0;
    u16 var_v0_3;
    u32 workingValue;
    u32 temp_v0_3;
    u8 *temp_a0_2;
    u8 *stream;
    s32 temp_a0_3;
    u8 temp_a0_4;
    u8 temp_a0_5;
    u8 nameGlyph;
    u8 temp_v0;
    u8 temp_v0_2;
    u8 temp_v0_5;
    u8 temp_v0_6;
    u8 temp_v0_7;
    u8 temp_v1_11;
    u8 temp_v1_2;
    u8 temp_v1_3;
    void *temp_a1_2;
    void *temp_a1_3;
    RenderSpritePacket *selectionSprite;
    void *selectionBase;
    TextboxEntry *textbox;
    register TextboxGlyphPacket *auxiliaryPacket asm("$17");
    TextboxGlyphPacket *currentPacket;
    TextboxGlyphPacket *nextPacket;
    TextboxNumber *numberStart;

    sectionIndex = 0;
    backgroundDrawn = 0;
    temp_a0 = g_ActiveDrawSlot * 4;
    {
        TextboxTableAddress buffers;

        buffers.pointer = &D_800B0E38.other_buffers[1];
        buffers.address = buffers.address + temp_a0;
        nextPacket = *buffers.packets;
    }
    initialBufferTable = &D_800B0E38.other_buffers[1];
    asm("" : "=r"(initialBufferTable) : "0"(initialBufferTable), "r"(nextPacket));
    if (g_ScriptCollisionFlag != 0) {
        TextboxTableAddress ordering;

        ordering.pointer = initialBufferTable;
        ordering.address = ordering.address + temp_a0 - 0xC;
        AddPrim(*ordering.ordering + 2,
                (unsigned int *)((g_ActiveDrawSlot * 0x18) + (u8 *)g_TextboxFogTilePackets));
    }
    textboxIndex = 0;
    sectionMask = 0xFFF0FFFF;
    do {
        if (g_TextboxEntries[textboxIndex].state != 0) {
            orderingTable = ((u32 *)D_800B0E38.ordering[g_ActiveDrawSlot]) + 1;
            textbox = &g_TextboxEntries[textboxIndex];
            glyphWidth = 14;
            stopDrawing = 0;
            if (textbox->state == 1) {
                textbox->control.parts.delay = 0U;
                textbox->control.parts.elapsed = 0U;
                textbox->control.flags = textbox->control.flags & sectionMask;
                textbox->message = g_TextboxMessageSource;
                while (1) {
                    temp_a0_2 = textbox->message;
                    temp_v0 = *temp_a0_2;
                    if (temp_v0 == 0xFF || temp_v0 == 0xF9) {
                        textbox->message = temp_a0_2 + 2;
                        if (temp_a0_2[1] == 0xFE && temp_a0_2[2] == textbox->page_id) {
                            textbox->message = temp_a0_2 + 3;
                            break;
                        }
                    }
                    textbox->message++;
                }
            }
            baseX = 0x14;
            cursor.y = 0xAE;
            cursor.x = 0x14;
            g_TextboxGlyphColorB = 0x80;
            g_TextboxGlyphColorG = 0x80;
            g_TextboxGlyphColorR = 0x80;
            temp_v1_2 = textbox->style;
            if (temp_v1_2 != 0) {
                if (temp_v1_2 == 3) {
                    cursor.y = 0xAE;
                    cursor.x = 0x14;
                    g_TextboxGlyphColorR = g_TextboxHighlightColorR;
                    g_TextboxGlyphColorG = g_TextboxHighlightColorG;
                    g_TextboxGlyphColorB = g_TextboxHighlightColorB;
                } else {
                    temp_t1 = textbox->x;
                    baseX = temp_t1;
                    cursor.x = temp_t1;
                    if (!(textbox->control.flags & 0x02000000)) {
                        var_v0 = textbox->y + 6;
                    } else {
                        var_v0 = textbox->y;
                    }
                    cursor.y = var_v0;
                    if (textbox->style == 1) {
                        Draw_SetCursor(textbox->x, textbox->y);
                        Draw_AllocColorGradient(textbox->width, textbox->height, 0, 0);
                    }
                }
            } else {
                if ((textbox->background != 0) && (backgroundDrawn == 0) && (g_ScriptCollisionFlag == 0)) {
                    backgroundDrawn = 1;
                    AddPrim(((u32 *)D_800B0E38.ordering[g_ActiveDrawSlot]) + 2,
                            (unsigned int *)((g_ActiveDrawSlot * 0x18) + (u8 *)g_TextboxFogTilePackets));
                }
            }
            numberStart = textbox->numbers;

            stream = textbox->message;
            number = numberStart;
            glyphAdvance = glyphWidth;
            currentPacket = nextPacket;
            lineHeight = 14;
            for (;;) {
                font = &g_TextboxFontPages[0];
                asm("" : "=r"(currentPacket) : "0"(currentPacket));
                temp_v1_3 = *stream;
                switch (temp_v1_3) {
                case 0xF7:
                    stream += 1;
                    cursor.x = baseX;
                    cursor.y += lineHeight;
                    break;
                case 0xF8:
                    if ((textbox->state == 2) && (g_FieldPadPressed & 0x100)) {
                        do {
                            temp_v0 = *stream;
                            stream++;
                        } while (temp_v0 != 0xF8);
                        textbox->message = stream;
                    }
                    stopDrawing = 1;
                    break;
                case 0xF9:
                    textbox->state = 0U;
                    stopDrawing = 1;
                    break;
                case 0xFF:
                    if ((textbox->state == 2) && (g_FieldPadPressed & 0x100) &&
                        !(textbox->control.flags & 0x02000000)) {
                        textbox->state = 0U;
                    }
                    stopDrawing = 1;
                    break;
                case 0xFA:
                    nameIndex = 0;
                    if (g_TextboxNameGlyphCount > 0) {
                        auxiliaryPacket = nextPacket;
                        do {
                            nameGlyph = g_TextboxNameGlyphBuf[(s8)nameIndex];

                            uv.x = (u8)((unsigned int)nameGlyph % 21) * 12;
                            uv.y = (u8)((unsigned int)nameGlyph / 21) * 12;
                            if ((u8)(nameGlyph - 0x10) < 0x3C) {
                                cursor.x -= g_TextboxGlyphSpacing[nameGlyph - 0x10].left;
                            }
                            SetDrawTPage(nextPacket, 0, 1, font->tpage);
                            SetSprt(&nextPacket->sprite);
                            if (MargePrim(nextPacket, &nextPacket->sprite) != 0) {
                                exit(-1);
                            }

                            auxiliaryPacket->sprite.x = cursor.x;
                            auxiliaryPacket->sprite.y = cursor.y;
                            auxiliaryPacket->sprite.u = (s8)uv.x;
                            auxiliaryPacket->sprite.v = (s8)uv.y;
                            currentPacket++;
                            auxiliaryPacket->sprite.color.bytes.r = g_TextboxGlyphColorR;
                            auxiliaryPacket->sprite.color.bytes.g = g_TextboxGlyphColorG;
                            nextPacket++;
                            auxiliaryPacket->sprite.width = 12;
                            auxiliaryPacket->sprite.height = 12;
                            auxiliaryPacket->sprite.color.bytes.b = g_TextboxGlyphColorB;
                            auxiliaryPacket->sprite.clut = font->clut;

                            temp_a1_2 = auxiliaryPacket;
                            auxiliaryPacket++;
                            AddPrim(orderingTable, temp_a1_2);
                            if ((u8)(nameGlyph - 0x10) < 0x3C) {
                                cursor.x += g_TextboxGlyphSpacing[nameGlyph - 0x10].right;
                            } else {
                                cursor.x += glyphAdvance;
                            }
                            nextNameIndex = nameIndex + 1;
                            nameIndex = nextNameIndex;
                            asm("" : "=r"(nextNameIndex) : "0"(nextNameIndex));
                        } while ((s8)nextNameIndex < g_TextboxNameGlyphCount);
                    }
                    stream += 1;
                    break;
                case 0xFB:
                    stream += 1;
                    temp_a0_3 = *stream;
                    switch (temp_a0_3) {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                        uv.x = (*stream * 0xC) + 0x40;
                        uv.y = g_TextboxFontPages[2].textureY;
                        SetDrawTPage(nextPacket, 0, 1, g_TextboxFontPages[2].tpage);
                        SetSprt(&nextPacket->sprite);
                        if (MargePrim(nextPacket, &nextPacket->sprite) != 0) {
                            exit(-1);
                        }
                        nextPacket++;
                        currentPacket->sprite.x = cursor.x;
                        linkOrdering = orderingTable;
                        currentPacket->sprite.y = cursor.y;
                        currentPacket->sprite.u = (s8)uv.x;
                        currentPacket->sprite.v = (s8)uv.y;
                        currentPacket->sprite.color.bytes.r = g_TextboxGlyphColorR;
                        currentPacket->sprite.width = 0xC;
                        currentPacket->sprite.color.bytes.g = g_TextboxGlyphColorG;
                        linkPacket = currentPacket;
                        currentPacket->sprite.height = 0xC;
                        currentPacket->sprite.color.bytes.b = g_TextboxGlyphColorB;
                        currentPacket->sprite.clut = g_TextboxFontPages[2].clut;
                        stream++;
                        AddPrim(linkOrdering, (unsigned int *)linkPacket);
                        currentPacket++;
                        var_v0_3 = cursor.x + glyphAdvance;
                        cursor.x = var_v0_3;
                        break;
                    case 4:
                        g_TextboxGlyphColorR = 0xFF;
                        g_TextboxGlyphColorB = 0;
                        g_TextboxGlyphColorG = 0;
                        stream += 1;
                        break;
                    case 5:
                        g_TextboxGlyphColorB = 0x80;
                        g_TextboxGlyphColorG = 0x80;
                        g_TextboxGlyphColorR = 0x80;
                        stream += 1;
                        break;
                    case 6: {
                        s32 sectionTerminator = 6;
                        if (g_FieldPadPressed & 0x100) {
                        section_again:
                            workingValue = textbox->control.flags;
                            if (sectionIndex == ((workingValue >> 16) & 15)) {
                                stream++;
                                textbox->control.flags = (workingValue & sectionMask) |
                                                         (((sectionIndex + 1) & 15) << 16);
                                stopDrawing = 1;
                                goto section_again;
                            }
                            if (*stream != sectionTerminator)
                                goto section_next;
                        } else {
                            if (sectionIndex != (textbox->control.parts.bits & 15))
                                goto section_skip;
                            stopDrawing = 1;
                            sectionIndex++;
                            break;
                        }
                        goto section_skip;
                    }
                    case 7:
                        stream++;
                        temp_v0_2 = *stream;
                        temp_a0_4 = textbox->control.parts.elapsed;
                        textbox->control.parts.delay = temp_v0_2;
                        if (temp_a0_4 < temp_v0_2) {
                            if (sectionIndex != (textbox->control.parts.bits & 15))
                                goto section_skip;
                            textbox->control.parts.elapsed = temp_a0_4 + 1;
                            stopDrawing = 1;
                        }
                        else
                        {
                    section_delay_again:
                        if (sectionIndex == (textbox->control.parts.bits & 15)) {
                            textbox->control.parts.elapsed = 0;
                            textbox->control.parts.delay = *stream;
                            temp_v0_3 = textbox->control.flags;
                            stream++;
                            textbox->control.flags = (temp_v0_3 & sectionMask) |
                                                     (((((temp_v0_3 >> 16) & 15) + 1) & 15) << 16);
                            goto section_delay_again;
                        }
                        if (textbox->control.parts.elapsed != 0) {
                    section_skip:
                        stream++;
                        }
                        }
                    section_next:
                        sectionIndex++;
                        break;
                    case 8:
                        temp_v0_4 = number->count;
                        digitIndex = temp_v0_4;
                        if (temp_v0_4 > 0) {
                            auxiliaryPacket = nextPacket;
                            for (; digitIndex > 0; digitIndex--) {
                                asm("" : : : "$20");
                                uv.x = number->digits[digitIndex - 1] * 0xC;
                                uv.y = 0;
                                SetDrawTPage(nextPacket, 0, 1, font->tpage);
                                SetSprt(&nextPacket->sprite);
                                if (MargePrim(nextPacket, &nextPacket->sprite) !=
                                    0) {
                                    exit(-1);
                                }

                                auxiliaryPacket->sprite.x = cursor.x;
                                auxiliaryPacket->sprite.y = cursor.y;
                                auxiliaryPacket->sprite.u = (s8)uv.x;
                                auxiliaryPacket->sprite.v = (s8)uv.y;
                                currentPacket++;
                                auxiliaryPacket->sprite.color.bytes.r = g_TextboxGlyphColorR;
                                nextPacket++;
                                auxiliaryPacket->sprite.color.bytes.g = g_TextboxGlyphColorG;
                                auxiliaryPacket->sprite.width = 12;
                                auxiliaryPacket->sprite.height = 12;
                                auxiliaryPacket->sprite.color.bytes.b = g_TextboxGlyphColorB;
                                temp_a1_3 = auxiliaryPacket;
                                auxiliaryPacket->sprite.clut = font->clut;
                                auxiliaryPacket++;
                                AddPrim(orderingTable, temp_a1_3);
                                cursor.x += glyphAdvance;
                            }
                        }
                        stream += 1;
                        number++;
                        break;
                    case 9: {
                        TextboxPoint *selectionCursor = &cursor;
                        s32 selectionOrderingOffset;
                        stream++;
                        temp_v1_7 = textbox->control.flags | 0x200000;
                        textbox->control.flags = temp_v1_7;
                        workingValue = (temp_v1_7 & 0xFE3FFFFF) | ((*stream & 7) << 0x16);
                        textbox->control.flags = workingValue;
                        temp_v1_9 = (workingValue >> 0x16) & 7;
                        if (g_FieldPadPressed & 0x20) {
                            temp_v0_5 = g_DialogueChoiceCursor + 1;
                            g_DialogueChoiceCursor = temp_v0_5;
                            if ((s8)temp_v0_5 >= temp_v1_9) {
                                g_DialogueChoiceCursor = temp_v1_9 - 1;
                            }
                        }
                        if (g_FieldPadPressed & 8) {
                            temp_v0_6 = g_DialogueChoiceCursor - 1;
                            g_DialogueChoiceCursor = temp_v0_6;
                            if ((s8)temp_v0_6 < 0) {
                                g_DialogueChoiceCursor = 0;
                            }
                        }
                        if (g_FieldPadPressed & 0x100) {
                            g_MenuEquipSlotIndex = g_DialogueChoiceCursor;
                        }
                        stream++;
                        selectionFrame = g_ActiveDrawSlot;
                        selectionOffset = selectionFrame * 0x1C;
                        selectionOrderingOffset = selectionFrame * 4;
                        selectionSprite =
                            (RenderSpritePacket *)(selectionOffset + (u8 *)&g_TextboxChoiceCursor[0].sprite);
                        selectionSprite->x = selectionCursor->x;
                        selectionBase = (u8 *)g_TextboxChoiceCursor;
                        selectionSprite->y = (s16)(selectionCursor->y + ((s8)g_DialogueChoiceCursor * 0xE));
                        {
                            TextboxTableAddress ordering;

                            ordering.ordering = D_800B0E38.ordering;
                            ordering.address = ordering.address + selectionOrderingOffset;
                            AddPrim(*ordering.ordering + 1, selectionOffset + selectionBase);
                        }
                        break;
                    }
                    default:
                        divisionHintfb = 0x30C30C31;
                        uv.x = ((temp_a0_3 + 0xED) % 21) * 0xC;
                        temp_v1_10 = ((*stream + 0xED) / 21) * 0xC;
                        uv.y = temp_v1_10;
                        asm("" : "=r"(divisionHintfb) : "0"(divisionHintfb));
                        if ((temp_v1_10 & 0xFFFF) >= 0xF1U) {
                            uv.y = temp_v1_10 - 0xFC;
                            font = &g_TextboxFontPages[1];
                        }
                        SetDrawTPage(nextPacket, 0, 1, font->tpage);
                        SetSprt(&nextPacket->sprite);
                        if (MargePrim(nextPacket, &nextPacket->sprite) != 0) {
                            exit(-1);
                        }
                        nextPacket++;
                        currentPacket->sprite.x = cursor.x;
                        linkOrdering = orderingTable;
                        currentPacket->sprite.y = cursor.y;
                        currentPacket->sprite.u = (s8)uv.x;
                        currentPacket->sprite.v = (s8)uv.y;
                        currentPacket->sprite.color.bytes.r = g_TextboxGlyphColorR;
                        currentPacket->sprite.width = 0xC;
                        currentPacket->sprite.color.bytes.g = g_TextboxGlyphColorG;
                        linkPacket = currentPacket;
                        currentPacket->sprite.height = 0xC;
                        currentPacket->sprite.color.bytes.b = g_TextboxGlyphColorB;
                        currentPacket->sprite.clut = font->clut;
                        stream++;
                        AddPrim(linkOrdering, (unsigned int *)linkPacket);
                        currentPacket++;
                        var_v0_3 = cursor.x + glyphAdvance;
                        cursor.x = var_v0_3;
                        break;
                    }
                    break;
                case 0xFC:
                    stream++;
                    divisionHintfc = 0x30C30C31;
                    uv.x = ((*stream + 0x34) % 21) * 0xC;
                    uv.y = ((*stream + 0x34) / 21) * 0xC;
                    asm("" : "=r"(divisionHintfc) : "0"(divisionHintfc));
                    SetDrawTPage(nextPacket, 0, 1, g_TextboxFontPages[1].tpage);
                    SetSprt(&nextPacket->sprite);
                    if (MargePrim(nextPacket, &nextPacket->sprite) != 0) {
                        exit(-1);
                    }
                    nextPacket++;
                    currentPacket->sprite.x = cursor.x;
                    linkOrdering = orderingTable;
                    currentPacket->sprite.y = cursor.y;
                    currentPacket->sprite.u = (s8)uv.x;
                    currentPacket->sprite.v = (s8)uv.y;
                    currentPacket->sprite.color.bytes.r = g_TextboxGlyphColorR;
                    currentPacket->sprite.width = 0xC;
                    currentPacket->sprite.color.bytes.g = g_TextboxGlyphColorG;
                    linkPacket = currentPacket;
                    currentPacket->sprite.height = 0xC;
                    currentPacket->sprite.color.bytes.b = g_TextboxGlyphColorB;
                    currentPacket->sprite.clut = g_TextboxFontPages[1].clut;
                    stream++;
                    AddPrim(linkOrdering, (unsigned int *)linkPacket);
                    currentPacket++;
                    var_v0_3 = cursor.x + glyphAdvance;
                    cursor.x = var_v0_3;
                    break;
                case 0xFD:
                    stream++;
                    uv.x = ((*stream + 0x134) % 21) * 0xC;
                    uv.y = ((*stream + 0x134) / 21) * 0xC;
                    SetDrawTPage(nextPacket, 0, 1, g_TextboxFontPages[1].tpage);
                    SetSprt(&nextPacket->sprite);
                    if (MargePrim(nextPacket, &nextPacket->sprite) != 0) {
                        exit(-1);
                    }
                    nextPacket++;
                    currentPacket->sprite.x = cursor.x;
                    linkOrdering = orderingTable;
                    currentPacket->sprite.y = cursor.y;
                    currentPacket->sprite.u = (s8)uv.x;
                    currentPacket->sprite.v = (s8)uv.y;
                    currentPacket->sprite.color.bytes.r = g_TextboxGlyphColorR;
                    currentPacket->sprite.width = 0xC;
                    currentPacket->sprite.color.bytes.g = g_TextboxGlyphColorG;
                    linkPacket = currentPacket;
                    currentPacket->sprite.height = 0xC;
                    currentPacket->sprite.color.bytes.b = g_TextboxGlyphColorB;
                    currentPacket->sprite.clut = g_TextboxFontPages[1].clut;
                    stream++;
                    AddPrim(linkOrdering, (unsigned int *)linkPacket);
                    currentPacket++;
                    var_v0_3 = cursor.x + glyphAdvance;
                    cursor.x = var_v0_3;
                    break;
                case 0xF:
                    if (g_TextboxKernGlyph0F == 1) {
                        cursor.x -= 7;
                    }
                    /* fallthrough */
                default:
                    temp_a0_5 = *stream;

                    uv.x = (u8)((unsigned int)temp_a0_5 % 21) * 12;
                    temp_v0_7 = *stream;

                    workingValue = (u8)((unsigned int)temp_v0_7 / 21);
                    uv.y = workingValue * 12;
                    if ((u8)(*stream - 0x10) < 0x3C) {
                        cursor.x -= g_TextboxGlyphSpacing[*stream - 0x10].left;
                    }
                    SetDrawTPage(nextPacket, 0, 1, font->tpage);
                    SetSprt(&nextPacket->sprite);
                    if (MargePrim(nextPacket, &nextPacket->sprite) != 0) {
                        exit(-1);
                    }

                    currentPacket->sprite.x = cursor.x;
                    currentPacket->sprite.y = cursor.y;
                    currentPacket->sprite.u = (s8)uv.x;
                    currentPacket->sprite.v = (s8)uv.y;
                    currentPacket->sprite.color.bytes.r = g_TextboxGlyphColorR;
                    currentPacket->sprite.width = 12;
                    currentPacket->sprite.color.bytes.g = g_TextboxGlyphColorG;
                    currentPacket->sprite.height = 12;
                    nextPacket++;
                    currentPacket->sprite.color.bytes.b = g_TextboxGlyphColorB;
                    currentPacket->sprite.clut = font->clut;

                    AddPrim(orderingTable, (unsigned int *)(currentPacket++));
                    if ((u8)(*stream - 0x10) < 0x3C) {
                        cursor.x += g_TextboxGlyphSpacing[*stream - 0x10].right;
                    } else {
                        cursor.x += glyphAdvance;
                    }
                    stream += 1;
                    break;
                }
                if (stopDrawing != 0)
                    break;
            }
            glyphWidth = 12;
            if (textbox->state == 1) {
                textbox->state = 2U;
            }
        }
        textboxIndex += 1;
    } while (textboxIndex < 4U);
    asm volatile(""
                 : "=g"(matchingStackSlot)
                 :
                 : "$2", "$4", "$6", "$8", "$10", "$12", "$14", "$16", "$18", "$19", "$20", "$22",
                   "$24");
}

/* Stored only by Draw_SetLightingParams, which nothing calls; no reader. */
unsigned char g_DrawLightingParamsSetWriteOnly;
short g_DrawLightingParam0WriteOnly;
short g_DrawLightingParam1WriteOnly;
short g_DrawLightingParam2WriteOnly;
unsigned char g_TextboxHighlightColorR;
unsigned char g_TextboxHighlightColorG;
unsigned char g_TextboxHighlightColorB;

unsigned char g_TextboxHighlightColorR;
unsigned char g_TextboxHighlightColorG;
unsigned char g_TextboxHighlightColorB;

void Draw_SetLightingParams(short arg0, short arg1, short arg2, unsigned char arg3, unsigned char arg4, unsigned char arg5) {
    g_DrawLightingParam0WriteOnly = arg0;
    g_DrawLightingParamsSetWriteOnly = 1;
    g_DrawLightingParam1WriteOnly = arg1;
    g_DrawLightingParam2WriteOnly = arg2;
    g_TextboxHighlightColorR = arg3;
    g_TextboxHighlightColorG = arg4;
    g_TextboxHighlightColorB = arg5;
}

void Menu_SetHighlightColor(int arg0, int arg1, int arg2, int arg3) {
    g_TextboxHighlightColorR = arg1;
    g_TextboxHighlightColorG = arg2;
    g_TextboxHighlightColorB = arg3;
}
