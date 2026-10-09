/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "common.h"
#include "pe1/textbox_open.h"
#include "pe1/textbox.h"

/* Each textbox entry is 0x38 bytes; preserve the retail field addresses. */
extern u8 D_800BCEA8[];
extern u8 D_800BCEAC[];
extern u8 D_800BCEB0[];
extern u8 D_800BCEB1[];
extern u8 D_800BCEB4[];
extern u8 D_800BCEB5[];
extern u16 D_800BCEB8[];
extern u8 D_8009EC86[];
extern u8 D_8009EC78[];
extern u8 D_8009EC70[];
extern u8 D_8009ECA8[];
extern u16 D_80091680[];
extern int D_8009CED4;
extern u8 *D_8009CE90;
void SetDrawTPage(void *, int, int, int);
void SetSprt(void *);
int MargePrim(void *, void *);
void SetShadeTex(void *, int);
u16 GetTPage(int tp, int abr, int x, int y);
void SetTile(void *);
void SetSemiTrans(void *, int);
void exit(int);

void Render_SetupFogLayer(void *source) {
    int i;
    u16 *page_source;
    u8 *tile_base;
    register u8 *draw_base asm("$23");
    u8 *tile_payload_base;
    int sentinel;
    register u8 *sprite_base asm("$11");
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
    mask5 =0xFDFFFFFF;
    sentinel = -1;
    for (; (u8)i < 4; ++i) {
        offset = ((((u8)i << 3) - (u8)i) << 3);
        *(u8 *)(D_800BCEB4 + offset) = 0;
        *(u8 *)(D_800BCEB5 + offset) = 0;
        asm volatile("" ::: "memory");
        flags = *(u32 *)(D_800BCEB4 + offset);
        *(u8 *)(D_800BCEA8 + offset) = 0;
        *(u32 *)(D_800BCEAC + offset) = 0;
        *(u8 *)(D_800BCEB0 + offset) = 0;
        *(u8 *)(D_800BCEB1 + offset) = 0;
        *(s16 *)((u8 *)D_800BCEB8 + offset) = sentinel;
        flags &= mask1;
        flags &= mask2;
        flags &= mask3;
        flags &= mask4;
        flags &= mask5;
        *(u32 *)(D_800BCEB4 + offset) = flags;
    }
    i = 0;
    page_source = D_80091680;
    draw_base = D_8009EC70;
    tile_base = D_8009ECA8;
    tile_payload_base = tile_base + 8;
    D_8009CEA0 = 0;
    D_8009CED0 = 0;
    D_8009CED4 = 0;
    D_8009CE90 = source;
    for (; (u8)i < 2; ++i) {
        draw_mode = (u8 *)((int)((u8)i * 28) + (int)draw_base);
        sprite = draw_mode + 8;
        SetDrawTPage(draw_mode, 0, 1, page_source[0]);
        SetSprt(sprite);
        if (MargePrim(draw_mode, sprite)) exit(-1);
        sprite_base = D_8009EC78;

        sprite_fields = (RenderSpritePacket *)((int)((u8)i * 28) + (int)sprite_base);
        SetShadeTex(sprite_fields, 1);
        x_arg = 0;

        y_arg = 0;

        zero_arg = 0;

        sprite_fields->u = 0x70;
        color = *(page_source - 3);
        sprite_fields->width = 0x18;
        sprite_fields->height = 0xC;
        sprite_fields->v = color;
        *(u16 *)(D_8009EC86 + (u8)i * 28) = page_source[1];
        tpage = GetTPage(x_arg, y_arg, zero_arg, 0);
        tile_mode = (u8 *)((int)((u8)i * 24) + (int)tile_base);
        tile = tile_mode + 8;
        SetDrawTPage(tile_mode, 0, 1, tpage & 0xFFFF);
        SetTile(tile);
        if (MargePrim(tile_mode, tile)) exit(-1);
        payload = (u8 *)((int)((u8)i * 24) + (int)tile_payload_base);
        dither_enabled = 1;
        asm volatile("" : "=r"(payload), "=r"(dither_enabled)
            : "0"(payload), "1"(dither_enabled));
        {
            int two = 2;

            payload[4] = two;
            payload[5] = two;
            payload[6] = two;
            *(u16 *)(payload + 0xC) = 0x140;
            *(u16 *)(payload + 0xE) = 0x36;
            *(u16 *)(payload + 8) = 0;
            *(u16 *)(payload + 0xA) = 0xAA;
            SetSemiTrans(payload, dither_enabled);
        }
    }
}

void Menu_SetTextCursorRect(int x, int y, int w, int h) {
    D_8009CE98.x = x;
    D_8009CE98.y = y;
    D_8009CE98.width = w;
    D_8009CE98.height = h;
}

void Tbl_ClearEntry(int arg0) {
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

void Tbl_ResetAll(void) {
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

s8 Tbl_LookupEntry(int arg0) {
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


void Task_EnableMovement(void) {
    D_8009CED0 = 1;
}

void Task_DisableMovement(void) {
    D_8009CED0 = 0;
}

void Task_SetCollisionFlag(int value) {
    D_8009CED4 = value != 0;
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
        D_8009CEA0 = 0;
        {
            int terminator = -1;
            D_8009CEA4 = terminator;
        }
        g_TextboxEntries[i].style = style;
        g_TextboxEntries[i].control.flags &= ~0x100000;
        {
            int reciprocal = 0x66666667;
            asm("" : : "r"(reciprocal), "r"(index));
        }
        g_TextboxEntries[i].control.flags &= ~0x200000;
        if (style) {
            g_TextboxEntries[i].x = D_8009CE98.x;
            g_TextboxEntries[i].y = D_8009CE98.y;
            g_TextboxEntries[i].width = D_8009CE98.width;
            g_TextboxEntries[i].height = D_8009CE98.height;
            if (g_TextboxEntries[i].style == 3)
                g_TextboxEntries[i].control.flags |= 0x100000;
        } else if (D_8009CED0) {
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
    return D_8009CEA4;
}

void AddPrim(u32 *, void *);
extern TextboxFontPage D_80091644[4];

extern TextboxNameGlyphs D_80091694;

extern TextboxGlyphSpacing D_800916A0[256];

extern s32 D_8009CDDC[];
extern u8 D_8009CE94;
/* Tentative small-data definition: stock maspsx must recognize this store
 * as GP-relative to retain its load-delay nop. The linker pins the COMMON
 * symbol to the retail address, as for other unplaced game globals. */
signed char D_8009CEA4;
extern u8 D_8009CEA8;
extern u8 D_8009CEAC;
extern u8 D_8009CEB0;
extern u8 D_8009CEC4;
extern u8 D_8009CEC8;
extern u8 D_8009CECC;
extern s32 D_8009D1F4[];
extern RenderBufferPrefix D_800B0E38;


void Menu_DrawTextboxEntries(void) {
    TextboxPoint cursor;
    TextboxPoint uv;
    u8 textboxIndex;
    u8 sectionIndex;
    u8 backgroundDrawn;
    /* Matching debt: the exit barrier reserves the unused 0x38 stack slot. */
    long long matchingStackSlot;
    /* Matching debt: seven register bindings and six empty barriers preserve
     * the retail allocation. No instruction bodies are embedded in C. */
    register s32 nameIndex asm("$20");
    u32 *orderingTable;
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
    TextboxFontPage *font;
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
    register u32 nameGlyph asm("$6");
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
    temp_a0 = D_8009CDDC[0] * 4;
    nextPacket = *(TextboxGlyphPacket **)((u8 *)&D_800B0E38.other_buffers[1] + temp_a0);
    initialBufferTable = &D_800B0E38.other_buffers[1];
    asm("" : "=r"(initialBufferTable) : "0"(initialBufferTable), "r"(nextPacket));
    if (D_8009CED4 != 0) {
        AddPrim((*(u32 **)((u8 *)initialBufferTable + temp_a0 - 0xC)) + 2,
                (D_8009CDDC[0] * 0x18) + (u8 *)D_8009ECA8);
    }
    textboxIndex = 0;
    sectionMask = 0xFFF0FFFF;
    do {
        if (g_TextboxEntries[textboxIndex].state != 0) {
            orderingTable = ((u32 *)D_800B0E38.ordering[D_8009CDDC[0]]) + 1;
            textbox = &g_TextboxEntries[textboxIndex];
            glyphWidth = 12;
            stopDrawing = 0;
            if (textbox->state == 1) {
                textbox->control.parts.delay = 0U;
                textbox->control.parts.elapsed = 0U;
                textbox->control.flags = (s32)((s32)textbox->control.flags & sectionMask);
                textbox->message = (u8 *)D_8009CE90;
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
            D_8009CEB0 = 0x80;
            D_8009CEAC = 0x80;
            D_8009CEA8 = 0x80;
            temp_v1_2 = textbox->style;
            if (temp_v1_2 != 0) {
                if (temp_v1_2 == 3) {
                    cursor.y = 0xAE;
                    cursor.x = 0x14;
                    D_8009CEA8 = D_8009CEC4;
                    D_8009CEAC = D_8009CEC8;
                    D_8009CEB0 = D_8009CECC;
                } else {
                    temp_t1 = textbox->x;
                    baseX = temp_t1;
                    cursor.x = temp_t1;
                    if (!((s32)textbox->control.flags & 0x02000000)) {
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
                if ((textbox->background != 0) && (backgroundDrawn == 0) && (D_8009CED4 == 0)) {
                    backgroundDrawn = 1;
                    AddPrim(((u32 *)D_800B0E38.ordering[D_8009CDDC[0]]) + 2,
                            (D_8009CDDC[0] * 0x18) + (u8 *)D_8009ECA8);
                }
            }
            numberStart = textbox->numbers;

            stream = textbox->message;
            number = numberStart;
            glyphAdvance = glyphWidth;
            currentPacket = nextPacket;
            lineHeight = 12;
            for (;;) {
                font = &D_80091644[0];
                asm("" : "=r"(currentPacket) : "0"(currentPacket));
                temp_v1_3 = *stream;
                switch (temp_v1_3) {
                case 0xF7:
                    stream += 1;
                    cursor.x = baseX;
                    cursor.y += lineHeight;
                    break;
                case 0xF8:
                    if ((textbox->state == 2) && (D_8009D1F4[0] & 0x100)) {
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
                    if ((textbox->state == 2) && (D_8009D1F4[0] & 0x100) &&
                        !((s32)textbox->control.flags & 0x02000000)) {
                        textbox->state = 0U;
                    }
                    stopDrawing = 1;
                    break;
                case 0xFA:
                    nameIndex = 0;
                    if (D_80091694.count > 0) {
                        auxiliaryPacket = nextPacket;
                        do {
                            register u32 fontRow asm("$5");
                            u16 nameU, nameV;
                            register u32 spacingOffset asm("$23");
                            nameGlyph = D_80091694.glyphs[(s8)nameIndex];
                            spacingOffset = nameGlyph * 2;

                            fontRow = (u32)nameGlyph / 21;
                            nameU = ((nameGlyph - (fontRow * 0x15)) & 0xFF) * 0xC;
                            nameV = (fontRow & 0xFF) * 0xC;
                            uv.x = nameU;
                            uv.y = nameV;
                            cursor.x -= *((u8 *)D_800916A0 + spacingOffset);
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
                            auxiliaryPacket->sprite.color.bytes.r = D_8009CEA8;
                            auxiliaryPacket->sprite.color.bytes.g = D_8009CEAC;
                            nextPacket++;
                            auxiliaryPacket->sprite.width = 12;
                            auxiliaryPacket->sprite.height = 12;
                            auxiliaryPacket->sprite.color.bytes.b = D_8009CEB0;
                            auxiliaryPacket->sprite.clut = font->clut;

                            temp_a1_2 = auxiliaryPacket;
                            auxiliaryPacket++;
                            AddPrim(orderingTable, temp_a1_2);
                            nextNameIndex = nameIndex + 1;
                            nameIndex = nextNameIndex;
                            asm("" : "=r"(nextNameIndex) : "0"(nextNameIndex));
                            cursor.x += *((u8 *)D_800916A0 + spacingOffset + 1);
                        } while ((s8)nextNameIndex < D_80091694.count);
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
                        uv.y = D_80091644[2].textureY;
                        SetDrawTPage(nextPacket, 0, 1, D_80091644[2].tpage);
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
                        currentPacket->sprite.color.bytes.r = (u8)D_8009CEA8;
                        currentPacket->sprite.width = 0xC;
                        currentPacket->sprite.color.bytes.g = (u8)D_8009CEAC;
                        linkPacket = currentPacket;
                        currentPacket->sprite.height = 0xC;
                        currentPacket->sprite.color.bytes.b = (u8)D_8009CEB0;
                        currentPacket->sprite.clut = (u16)D_80091644[2].clut;
                        stream++;
                        AddPrim(linkOrdering, linkPacket);
                        currentPacket++;
                        var_v0_3 = cursor.x + glyphAdvance;
                        cursor.x = var_v0_3;
                        break;
                    case 4:
                        D_8009CEA8 = 0xFF;
                        D_8009CEB0 = 0;
                        D_8009CEAC = 0;
                        stream += 1;
                        break;
                    case 5:
                        D_8009CEB0 = 0x80;
                        D_8009CEAC = 0x80;
                        D_8009CEA8 = 0x80;
                        stream += 1;
                        break;
                    case 6: {
                        s32 sectionTerminator = 6;
                        if (D_8009D1F4[0] & 0x100) {
                        section_again:
                            workingValue = (u32)textbox->control.flags;
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
                            goto section_next;
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
                            goto section_next;
                        }
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
                        if (textbox->control.parts.elapsed == 0)
                            goto section_next;
                    section_skip:
                        stream++;
                    section_next:
                        sectionIndex++;
                        break;
                    case 8:
                        temp_v0_4 = number->count;
                        digitIndex = temp_v0_4;
                        if (temp_v0_4 > 0) {
                            auxiliaryPacket = nextPacket;
                            for (; digitIndex > 0; digitIndex--) {
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
                                auxiliaryPacket->sprite.color.bytes.r = (u8)D_8009CEA8;
                                nextPacket++;
                                auxiliaryPacket->sprite.color.bytes.g = (u8)D_8009CEAC;
                                auxiliaryPacket->sprite.width = 12;
                                auxiliaryPacket->sprite.height = 12;
                                auxiliaryPacket->sprite.color.bytes.b = (u8)D_8009CEB0;
                                temp_a1_3 = auxiliaryPacket;
                                auxiliaryPacket->sprite.clut = (u16)font->clut;
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
                        temp_v1_7 = (s32)textbox->control.flags | 0x200000;
                        textbox->control.flags = temp_v1_7;
                        workingValue = (temp_v1_7 & 0xFE3FFFFF) | ((*stream & 7) << 0x16);
                        textbox->control.flags = workingValue;
                        temp_v1_9 = (workingValue >> 0x16) & 7;
                        if (D_8009D1F4[0] & 0x20) {
                            temp_v0_5 = D_8009CEA0 + 1;
                            D_8009CEA0 = temp_v0_5;
                            if ((s8)temp_v0_5 >= temp_v1_9) {
                                D_8009CEA0 = temp_v1_9 - 1;
                            }
                        }
                        if (D_8009D1F4[0] & 8) {
                            temp_v0_6 = D_8009CEA0 - 1;
                            D_8009CEA0 = temp_v0_6;
                            if ((s8)temp_v0_6 < 0) {
                                D_8009CEA0 = 0;
                            }
                        }
                        if (D_8009D1F4[0] & 0x100) {
                            D_8009CEA4 = D_8009CEA0;
                        }
                        stream++;
                        selectionFrame = D_8009CDDC[0];
                        selectionOffset = selectionFrame * 0x1C;
                        selectionOrderingOffset = selectionFrame * 4;
                        selectionSprite =
                            (RenderSpritePacket *)(selectionOffset + (u8 *)D_8009EC78);
                        selectionSprite->x = (u16)selectionCursor->x;
                        selectionBase = (u8 *)D_8009EC78 - 8;
                        selectionSprite->y = (s16)(selectionCursor->y + ((s8)D_8009CEA0 * 0xC));
                        AddPrim(*(u32 **)((u8 *)D_800B0E38.ordering + selectionOrderingOffset) + 1,
                                selectionOffset + selectionBase);
                        break;
                    }
                    default:
                        divisionHintfb = 0x30C30C31;
                        uv.x = ((temp_a0_3 + 0xED) % 21) * 0xC;
                        temp_v1_10 = ((*stream + 0xED) / 21) * 0xC;
                        uv.y = temp_v1_10;
                        asm("" : "=r"(divisionHintfb) : "0"(divisionHintfb));
                        if ((u32)(temp_v1_10 & 0xFFFF) >= 0xF1U) {
                            uv.y = temp_v1_10 - 0xFC;
                            font = &D_80091644[1];
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
                        currentPacket->sprite.color.bytes.r = (u8)D_8009CEA8;
                        currentPacket->sprite.width = 0xC;
                        currentPacket->sprite.color.bytes.g = (u8)D_8009CEAC;
                        linkPacket = currentPacket;
                        currentPacket->sprite.height = 0xC;
                        currentPacket->sprite.color.bytes.b = (u8)D_8009CEB0;
                        currentPacket->sprite.clut = (u16)font->clut;
                        stream++;
                        AddPrim(linkOrdering, linkPacket);
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
                    SetDrawTPage(nextPacket, 0, 1, D_80091644[1].tpage);
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
                    currentPacket->sprite.color.bytes.r = (u8)D_8009CEA8;
                    currentPacket->sprite.width = 0xC;
                    currentPacket->sprite.color.bytes.g = (u8)D_8009CEAC;
                    linkPacket = currentPacket;
                    currentPacket->sprite.height = 0xC;
                    currentPacket->sprite.color.bytes.b = (u8)D_8009CEB0;
                    currentPacket->sprite.clut = (u16)D_80091644[1].clut;
                    stream++;
                    AddPrim(linkOrdering, linkPacket);
                    currentPacket++;
                    var_v0_3 = cursor.x + glyphAdvance;
                    cursor.x = var_v0_3;
                    break;
                case 0xFD:
                    stream++;
                    uv.x = ((*stream + 0x134) % 21) * 0xC;
                    uv.y = ((*stream + 0x134) / 21) * 0xC;
                    SetDrawTPage(nextPacket, 0, 1, D_80091644[1].tpage);
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
                    currentPacket->sprite.color.bytes.r = (u8)D_8009CEA8;
                    currentPacket->sprite.width = 0xC;
                    currentPacket->sprite.color.bytes.g = (u8)D_8009CEAC;
                    linkPacket = currentPacket;
                    currentPacket->sprite.height = 0xC;
                    currentPacket->sprite.color.bytes.b = (u8)D_8009CEB0;
                    currentPacket->sprite.clut = (u16)D_80091644[1].clut;
                    stream++;
                    AddPrim(linkOrdering, linkPacket);
                    currentPacket++;
                    var_v0_3 = cursor.x + glyphAdvance;
                    cursor.x = var_v0_3;
                    break;
                case 0xF:
                    if (D_8009CE94 == 1) {
                        cursor.x -= 7;
                    }
                    /* fallthrough */
                default:
                    temp_a0_5 = *stream;

                    uv.x = (u8)((u32)temp_a0_5 % 21) * 12;
                    temp_v0_7 = *stream;

                    workingValue = (u8)((u32)temp_v0_7 / 21);
                    uv.y = workingValue * 12;
                    cursor.x -= D_800916A0[*stream].left;
                    SetDrawTPage(nextPacket, 0, 1, font->tpage);
                    SetSprt(&nextPacket->sprite);
                    if (MargePrim(nextPacket, &nextPacket->sprite) != 0) {
                        exit(-1);
                    }

                    currentPacket->sprite.x = cursor.x;
                    currentPacket->sprite.y = cursor.y;
                    currentPacket->sprite.u = (s8)uv.x;
                    currentPacket->sprite.v = (s8)uv.y;
                    currentPacket->sprite.color.bytes.r = (u8)D_8009CEA8;
                    currentPacket->sprite.width = 12;
                    currentPacket->sprite.color.bytes.g = (u8)D_8009CEAC;
                    currentPacket->sprite.height = 12;
                    nextPacket++;
                    currentPacket->sprite.color.bytes.b = (u8)D_8009CEB0;
                    currentPacket->sprite.clut = (u16)font->clut;

                    AddPrim(orderingTable, currentPacket++);
                    temp_v1_11 = D_800916A0[*stream].right;
                    stream += 1;
                    var_v0_3 = cursor.x + temp_v1_11;
                    cursor.x = var_v0_3;
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
                 : "$2", "$4", "$6", "$8", "$10", "$12", "$14", "$16", "$18", "$19", "$21", "$22",
                   "$24");
}

unsigned char D_8009CEB4;
short D_8009CEB8;
short D_8009CEBC;
short D_8009CEC0;
unsigned char D_8009CEC4;
unsigned char D_8009CEC8;
unsigned char D_8009CECC;

unsigned char D_8009CEC4;
unsigned char D_8009CEC8;
unsigned char D_8009CECC;

void Gpu_SetLightingParams(short arg0, short arg1, short arg2, unsigned char arg3, unsigned char arg4, unsigned char arg5) {
    D_8009CEB8 = arg0;
    D_8009CEB4 = 1;
    D_8009CEBC = arg1;
    D_8009CEC0 = arg2;
    D_8009CEC4 = arg3;
    D_8009CEC8 = arg4;
    D_8009CECC = arg5;
}

void Menu_SetHighlightColor(int arg0, int arg1, int arg2, int arg3) {
    D_8009CEC4 = arg1;
    D_8009CEC8 = arg2;
    D_8009CECC = arg3;
}
