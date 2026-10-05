#include "pe1/battle_runtime.h"
#include "pe1/battle_status.h"
#include "pe1/render_prim.h"

/* Builds the rotating target pointer and the two connector lines that tie
 * the floating status panel to the selected combatant. */

/* Packets of the active draw slot. Every access re-reads the slot, which
 * retail does after each byte store. */
#define STATUS_POINTER_SPRITE (&D_8009E460[g_ActiveDrawSlot].sprite)
#define STATUS_POINTER_TRI (&D_8009E4D8[g_ActiveDrawSlot])

void Battle_BuildStatusPrimHeader(RenderObjectEntity *object, s8 outOfRange)
{
    GteMatrix rotation;
    GteMatrix translation;
    GteShortVector pointer[3] = { { -3, -16, 0 }, { 3, -16, 0 }, { 0, 0, 0 } };
    GteVector projected[3];
    GteShortVector angles = { 0, 0, (D_8009D250 << 6) & 0xFFF };
    GteVector offset = { object->projected_target_x, object->projected_target_y, 0 };
    s32 flag;
    s16 x;
    s16 y;

    STATUS_POINTER_SPRITE->x = object->projected_target_x - 12;
    STATUS_POINTER_SPRITE->y = object->projected_target_y - 12;
    if (outOfRange == 0) {
        STATUS_POINTER_SPRITE->color.bytes.r = 0x96;
        STATUS_POINTER_SPRITE->color.bytes.g = 0x14;
        STATUS_POINTER_SPRITE->color.bytes.b = 0x14;
        STATUS_POINTER_TRI->r = 0x96;
        STATUS_POINTER_TRI->g = 0x14;
        STATUS_POINTER_TRI->b = 0x14;
    } else {
        STATUS_POINTER_SPRITE->color.bytes.r = 0x32;
        STATUS_POINTER_SPRITE->color.bytes.g = 0xA;
        STATUS_POINTER_SPRITE->color.bytes.b = 0xA;
        STATUS_POINTER_TRI->r = 0x32;
        STATUS_POINTER_TRI->g = 0xA;
        STATUS_POINTER_TRI->b = 0xA;
    }

    RotMatrix(&angles, &rotation);
    SetRotMatrix(&rotation);
    TransMatrix(&translation, &offset);
    SetTransMatrix(&translation);
    RotTrans(&pointer[0], &projected[0], &flag);
    RotTrans(&pointer[1], &projected[1], &flag);
    RotTrans(&pointer[2], &projected[2], &flag);
    STATUS_POINTER_TRI->x0 = projected[0].x;
    STATUS_POINTER_TRI->y0 = projected[0].y;
    STATUS_POINTER_TRI->x1 = projected[1].x;
    STATUS_POINTER_TRI->y1 = projected[1].y;
    STATUS_POINTER_TRI->x2 = projected[2].x;
    STATUS_POINTER_TRI->y2 = projected[2].y;
    AddPrim((unsigned int *)D_800B0E38.ordering[g_ActiveDrawSlot] + 5,
            (unsigned int *)STATUS_POINTER_TRI);

    /* Connector lines run to the panel on whichever side has room. */
    x = object->projected_target_x;
    if (x + 134 >= 301) {
        RenderLinePacket *lines;
        s16 panelX;

        x -= 8;
        lines = D_8009E498[g_ActiveDrawSlot];
        lines[1].x0 = x;
        D_8009E498[g_ActiveDrawSlot][0].x0 = x;
        panelX = object->projected_target_x;
        lines[1].x1 = panelX - 35;
        D_8009E498[g_ActiveDrawSlot][0].x1 = panelX - 35;
        D_8009E358[g_ActiveDrawSlot * 3].x0 = panelX - 115;
    } else {
        RenderLinePacket *lines;
        s16 lineX;
        s16 panelX;

        lineX = x + 8;
        lines = D_8009E498[g_ActiveDrawSlot];
        lines[1].x0 = lineX;
        D_8009E498[g_ActiveDrawSlot][0].x0 = lineX;
        panelX = object->projected_target_x + 35;
        D_8009E358[g_ActiveDrawSlot * 3].x0 = panelX;
        lines[1].x1 = panelX;
        D_8009E498[g_ActiveDrawSlot][0].x1 = panelX;
    }
    y = object->projected_target_y;
    if (y - 82 < 0) {
        D_8009E498[g_ActiveDrawSlot][0].y0 = y + 8;
        D_8009E498[g_ActiveDrawSlot][1].y0 = object->projected_target_y + 9;
        D_8009E498[g_ActiveDrawSlot][0].y1 = D_8009E358[g_ActiveDrawSlot * 3].y0 =
            object->projected_target_y + 35;
        D_8009E498[g_ActiveDrawSlot][1].y1 = object->projected_target_y + 36;
    } else {
        D_8009E498[g_ActiveDrawSlot][0].y0 = y - 8;
        D_8009E498[g_ActiveDrawSlot][1].y0 = object->projected_target_y - 7;
        D_8009E498[g_ActiveDrawSlot][0].y1 = D_8009E358[g_ActiveDrawSlot * 3].y0 =
            object->projected_target_y - 35;
        D_8009E498[g_ActiveDrawSlot][1].y1 = object->projected_target_y - 34;
    }
    AddPrim((unsigned int *)D_800B0E38.ordering[g_ActiveDrawSlot] + 7,
            (unsigned int *)&D_8009E498[g_ActiveDrawSlot][1]);
    AddPrim((unsigned int *)D_800B0E38.ordering[g_ActiveDrawSlot] + 6,
            (unsigned int *)&D_8009E498[g_ActiveDrawSlot][0]);
    AddPrim((unsigned int *)D_800B0E38.ordering[g_ActiveDrawSlot] + 5,
            (unsigned int *)&D_8009E460[g_ActiveDrawSlot]);
}

typedef union BattleStatusPrimAddress {
    BattleStatusLinePrim *line;
    unsigned int *words;
    u32 address;
} BattleStatusPrimAddress;

void Battle_LayoutStatusPrimRow(int bottomY)
{
    BattleStatusLinePrim *line;
    BattleStatusPrimAddress bufferBase;
    unsigned int **orderingBase;
    int savedBottomY;
    int slot;
    u8 row;

    savedBottomY = bottomY;
    row = 0;
    bufferBase.line = D_8009E358;
    orderingBase = (unsigned int **)D_800B0E38.ordering;
    do {
        slot = g_ActiveDrawSlot;
        line = (BattleStatusLinePrim *)
            (((slot * 3) << 4) + (row << 4) + bufferBase.address);
        line->x0 = D_8009E358[slot * 3].x0 + row;
        line->y0 = D_8009E358[slot * 3].y0 + row;
        line->x1 = 0x50 - (row * 2);
        line->y1 = (s8)savedBottomY - (row * 2);
        AddPrim(orderingBase[slot] + (7 - row),
                (unsigned int *)(((slot * 3) << 4) +
                    ((row << 4) + bufferBase.address)));
        row++;
    } while ((u8)row < 3);
}

static inline RenderSpritePacket *HighlightSpriteAt(RenderSpritePacket *base, int slot, unsigned byte_offset)
{
    return (RenderSpritePacket *)(slot * 3 * sizeof(BattleGaugePrim) + byte_offset + (u32)base);
}

void Battle_DrawTargetHighlight(void)
{
    unsigned char index = 0;
    RenderSpritePacket *base;
    RenderSpritePacket *sprite;
    BattleGaugePrim *packets;
    int slot;
    unsigned slot_offset;
    register unsigned first_offset asm("$4");
    register unsigned index_offset asm("$5");
    register unsigned char texture_v asm("$18");
    if (((D_8009D278->action->turnWord >> 4) & 3) == 0) {
        return;
    }
    texture_v = 224;
    base = &D_8009E3B8[0][0].sprite;
    packets = (BattleGaugePrim *)((char *)base - PE1_OFFSETOF(BattleGaugePrim, sprite));
    do {
        if (index == g_BattleActiveTurnSlot - 1) {
            first_offset = index * sizeof(BattleGaugePrim);
            HighlightSpriteAt(base, g_ActiveDrawSlot, first_offset)->u = index * 24 + 104;
        } else {
            first_offset = index * sizeof(BattleGaugePrim);
            HighlightSpriteAt(base, g_ActiveDrawSlot, first_offset)->u = 176;
        }
        HighlightSpriteAt(base, g_ActiveDrawSlot, first_offset)->v = texture_v;
        /* Preserve the retail store before recomputing the next packet offset. */
        __asm__ volatile("" : : : "memory");
        index_offset = index * sizeof(BattleGaugePrim);
        slot = g_ActiveDrawSlot;
        slot_offset = slot * 3 * sizeof(BattleGaugePrim);
        sprite = (RenderSpritePacket *)(slot_offset + index_offset + (u32)base);
        sprite->x = D_8009E358[slot * 3].x0 + index * 24;
        sprite->y = D_8009E358[slot * 3].y0 - 8;
        AddPrim((unsigned *)D_800B0E38.ordering[slot] + 7,
                (unsigned *)(slot_offset + (index_offset + (u32)packets)));
        ++index;
    } while (index < (int)((D_8009D278->action->turnWord >> 4) & 3));
}

#define NULL ((void *)0)

s16 GetClut(s32, s32);

extern struct { char _[16]; } D_8009CDDC_oa __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_ob __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_oc __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_od __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_oe __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_of __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_og __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_oh __asm__("g_ActiveDrawSlot");
extern struct { char _[16]; } D_8009CDDC_oi __asm__("g_ActiveDrawSlot");
#define CDDCA (*(s32 *)&D_8009CDDC_oa)
#define CDDCB (*(s32 *)&D_8009CDDC_ob)
#define CDDCC (*(s32 *)&D_8009CDDC_oc)
#define CDDCD (*(s32 *)&D_8009CDDC_od)
#define CDDCE (*(s32 *)&D_8009CDDC_oe)
#define CDDCF (*(s32 *)&D_8009CDDC_of)
#define CDDCG (*(s32 *)&D_8009CDDC_og)
#define CDDCH (*(s32 *)&D_8009CDDC_oh)
#define CDDCI (*(s32 *)&D_8009CDDC_oi)

extern struct { char _[16]; } g_ActiveActor_o __asm__("g_ActiveActor");
#define g_ActiveActor (*(u8 **)&g_ActiveActor_o)
extern struct { char _[16]; } D_8009D1DC_o __asm__("g_BattleRemainingActions");
#define g_BattleRemainingActions (*(u8 *)&D_8009D1DC_o)

extern u8 D_8009E360[];
extern u8 g_BattleStatusRowPosY[];
extern u8 D_8009E508[];
extern u8 D_8009E516[];
extern u8 g_OtBufferTable[];

void Battle_DrawSpellName(void) {
    s32 c;
    register s32 r130 asm("$4");
    s32 c1;
    s16 ys;
    s16 xs;
    s16 i;
    u8 *e508;
    s32 cidx;
    s16 clut;
    u8 *p;
    u8 *d68;

    c = CDDCA;
    {
        s32 tv;
        tv = *(u16 *)(g_BattleStatusRowPosY + c * 0x30);
        ys = tv + 4;
    }
    if ((*(s32 *)(*(u8 **)(g_ActiveActor + 0x68) + 0xC) & 0x300000) == 0x200000) {
        u8 *q508;
        r130 = 0x130;
        __asm__ __volatile__("" :: "r"(r130));
        q508 = D_8009E508;
        {
            register u8 *pa asm("$2");
            u8 e0a;
            pa = q508 + c * 0x118;
            e0a = 0xE0;
                        pa[0xC] = e0a;
            {
                s32 cB;
                cB = CDDCB;
                *(s16 *)(pa + 0x10) = 8;
                *(s16 *)(pa + 0x12) = 0x10;
                (q508 + cB * 0x118)[0xD] = e0a;
            }
        }
        {
            s32 cC;
            s32 t3;
            s32 rFD;
            s32 tE;
            cC = CDDCC;
            rFD = 0x1FD;
            t3 = cC * 0x118;
            tE = *(u16 *)(D_8009E360 + cC * 0x30);
            t3 = t3 + (s32)q508;
            *(s16 *)((u8 *)t3 + 0xA) = ys;
            *(s16 *)((u8 *)t3 + 8) = tE + 0x3C;
            clut = GetClut(r130, rFD);
        }
        {
            s32 cD;
            s32 eo;
            cD = CDDCD;
            q508 -= 8;
            eo = cD * 0x118;
            cD = cD * 4;
            *(s16 *)(D_8009E516 + eo) = clut;
            __asm__ __volatile__("");
            AddPrim(*(u8 **)(g_OtBufferTable + cD) + 0x10, eo + q508);
        }
        return;
    }
    i = 0;
    {
        s32 tx;
        tx = *(u16 *)(D_8009E360 + c * 0x30);
        xs = tx + 0x40;
    }
    if (g_BattleRemainingActions != 0) {
        u8 e0 = 0xE0;
        u8 *e5m8;
        e508 = D_8009E508;
        e5m8 = e508 - 8;
        do {
            s32 s0;
            u8 *pl;
            {
                register s32 t118 asm("$3");
                {
                    s32 cE;
                    cE = CDDCE;
                    t118 = cE * 0x118;
                    s0 = i * 0x1C;
                }
                t118 += s0;
                pl = (u8 *)t118 + (u32)e508;
            }
            {
                u8 *dval;
                dval = g_ActiveActor;
                *(s16 *)(pl + 0x10) = 4;
                *(s16 *)(pl + 0x12) = 0x10;
                d68 = *(u8 **)(dval + 0x68);
            }
            if ((*(s32 *)(d68 + 0xC) & 0x300000) == 0x300000) {
                r130 = 0x130;
                                pl[0xC] = 0xD8;
                c1 = CDDCF;
                ((u8 *)(c1 * 0x118 + s0) + (u32)e508)[0xD] = e0;
                clut = GetClut(r130, 0x1FB);
            } else if ((*(s32 *)(d68 + 0x10) & 0xC0) == 0x80) {
                r130 = 0x130;
                                pl[0xC] = 0xDC;
                c1 = CDDCG;
                ((u8 *)(c1 * 0x118 + s0) + (u32)e508)[0xD] = e0;
                clut = GetClut(r130, 0x1FC);
            } else {
                u8 dv = 0xD4;
                r130 = 0x130;
                                pl[0xC] = dv;
                c1 = CDDCH;
                ((u8 *)(c1 * 0x118 + s0) + (u32)e508)[0xD] = e0;
                clut = GetClut(r130, 0x1FA);
            }
            {
                register s32 cI asm("$4");
                register s32 ti asm("$3");
                cI = CDDCI;
                ti = cI * 0x118;
                ti = s0 + ti;
                                *(s16 *)(D_8009E516 + ti) = clut;
                __asm__ __volatile__("");
            }
            {
                s32 j7;
                s32 t2;
                register s32 cJ asm("$6");
                register s32 a2v asm("$5");
                {
                    register s32 ja asm("$4");
                    ja = i;
                    __asm__("" : "=r"(ja) : "0"(ja));
                    j7 = ja * 7;
                }
                j7 <<= 2;
                cJ = CDDCA;
                a2v = j7 + (s32)e5m8;
                t2 = cJ * 0x118;
                a2v = t2 + a2v;
                t2 = t2 + j7;
                t2 = t2 + (s32)e508;
                *(s16 *)((u8 *)t2 + 8) = xs;
                *(s16 *)((u8 *)t2 + 0xA) = ys;
                xs -= 6;
                AddPrim(*(u8 **)(g_OtBufferTable + cJ * 4) + 0x10, (void *)a2v);
            }
            {
                s16 inext = i + 1;
                i = inext;
                if (!(inext < g_BattleRemainingActions)) break;
            }
        } while (1);
    }
}

typedef struct StatusSymbolTemplate {
    u8 bytes[18];
} StatusSymbolTemplate;
typedef struct StatusSymbolSlot {
    StatusSymbolTemplate data;
    u8 padding[6];
} StatusSymbolSlot;
extern StatusSymbolTemplate D_80010DFC;
extern StatusSymbolTemplate D_80010E10;
extern StatusSymbolTemplate D_80010E24;
extern u8 D_8009E93C[];
extern u8 D_8009E93D[];
extern u8 D_8009E930[];
extern u8 D_8009E360[];

void Battle_DrawStatusSymbol(s8 which) {
    StatusSymbolSlot first;
    StatusSymbolSlot second;
    StatusSymbolSlot third;
    u8 spare[8]; /* Preserve the original stack frame around the copied templates. */
    u32 slot;
    u8 *packet;
    u32 packetOffset;
    u32 index;
    u8 *a, *b, *c;
    register u8 *packetBase asm("$9");
    u8 *thirdBase;
    first.data = D_80010DFC;
    second.data = D_80010E10;
    third.data = D_80010E24;
    index = which * 2;
    a = first.data.bytes + index;
    b = second.data.bytes + index;
    packetBase = D_8009E930;
    thirdBase = third.data.bytes;
    D_8009E93C[D_8009CDDC * 28] = a[0];
    c = thirdBase + index;
    D_8009E93D[D_8009CDDC * 28] = a[1];
    slot = D_8009CDDC;
    packetOffset = slot * 28;
    packet = packetBase + packetOffset;
    *(u16 *)(packet + 16) = b[0];
    *(u16 *)(packet + 18) = b[1];
    *(u16 *)(packet + 8) = *(u16 *)(D_8009E360 + slot * 48) + c[0];
    *(u16 *)(packet + 10) = *(u16 *)(D_8009E360 + slot * 48 + 2) + c[1];
    packetBase -= 8;
    AddPrim((u8 *)D_800B0E38.ordering[slot] + 16, packetBase + packetOffset);
}

void Battle_DrawATBGauge(void)
{
    short ammo;
    signed char i;
    short y;
    RenderSpritePacket *sprite;
    BattleGaugePrim *packet;
    RenderSpritePacket *base;
    unsigned category;
    /* Keep the queue index initialized across the ammo-base callback. */
    i = 0;
    y = D_8009E358[g_ActiveDrawSlot * 3].y0 + 22;
    category = (D_8009D278->action->attackWord >> 20) & 3;
    ammo = Inv_GetWeaponCategoryAmmoBase(category - 1);
    base = &D_8009E768[0].sprite;
    packet = (BattleGaugePrim *)((char *)base - PE1_OFFSETOF(BattleGaugePrim, sprite));

    packet += g_ActiveDrawSlot;
    sprite = (RenderSpritePacket *)((char *)base + sizeof(BattleGaugePrim) * g_ActiveDrawSlot);
    ammo += *(unsigned short *)&D_8009D278->action->attackWord & 0x3ff;
    sprite->x = D_8009E358[g_ActiveDrawSlot * 3].x0 + 8;
    sprite->y = y;
    AddPrim((unsigned *)D_800B0E38.ordering[g_ActiveDrawSlot] + 4, (unsigned *)packet);
    while (i < (signed char)Pad_GetMenuPressedBitOrDisabled()) {
        int kind = D_800BE830[i].field04;
        if (kind == 1) {
            --ammo;
        } else if (kind == 2) {
            unsigned mode = D_8009D278->action->turnWord & 0xc0;
            if (mode == 0xc0) {
                ammo -= *(unsigned char *)&D_8009D278->action->turnWord & 15;
            } else if (mode == 0x40) {
                --ammo;
            }
        } else if (kind == 0x189) {
            --ammo;
        }
        ++i;
    }
    if (ammo < 0) {
        ammo = 0;
    }
    Battle_DrawDecimalNumber(D_8009E7A0[g_ActiveDrawSlot], D_8009E358[g_ActiveDrawSlot * 3].x0 + 64, y - 1, ammo, 0);
}
