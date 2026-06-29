/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)

s16 GetClut(s32, s32);
void AddPrim(void *, void *);

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
            __asm__("" : "=r"(e0a) : "0"(e0a));
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
            register s32 t3 asm("$3");
            s32 rFD;
            s32 tE;
            cC = CDDCC;
            __asm__ __volatile__("li %0,0x1FD" : "=r"(rFD) : "r"(cC));
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
                __asm__ __volatile__("" :: "r"(r130));
                pl[0xC] = 0xD8;
                c1 = CDDCF;
                ((u8 *)(c1 * 0x118 + s0) + (u32)e508)[0xD] = e0;
                clut = GetClut(r130, 0x1FB);
            } else if ((*(s32 *)(d68 + 0x10) & 0xC0) == 0x80) {
                r130 = 0x130;
                __asm__ __volatile__("" :: "r"(r130));
                pl[0xC] = 0xDC;
                c1 = CDDCG;
                ((u8 *)(c1 * 0x118 + s0) + (u32)e508)[0xD] = e0;
                clut = GetClut(r130, 0x1FC);
            } else {
                u8 dv = 0xD4;
                r130 = 0x130;
                __asm__ __volatile__("" :: "r"(r130));
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
                __asm__("" : "=r"(ti) : "0"(ti));
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
