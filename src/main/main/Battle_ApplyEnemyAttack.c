/* CC1_FLAGS: -g3 -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)

#define BARRIER(x) __asm__("" : "=r"(x) : "0"(x))

s32 rand();
s32 Inv_PickRandomItem(s32);
int Inv_FindItemById(int arg0);
s32 Inv_GetItemEffectData(s16, s32);
void Inv_RollRandomItemType(void *, void *);

extern struct { char _[16]; } D278_o0 __asm__("g_ActiveActor");
#define D278_0 (*(u8 **)&D278_o0)
extern struct { char _[16]; } D278_o1 __asm__("g_ActiveActor");
#define D278_1 (*(u8 **)&D278_o1)
extern struct { char _[16]; } D278_o2 __asm__("g_ActiveActor");
#define D278_2 (*(u8 **)&D278_o2)
extern struct { char _[16]; } D278_o3 __asm__("g_ActiveActor");
#define D278_3 (*(u8 **)&D278_o3)
extern struct { char _[16]; } D278_o4 __asm__("g_ActiveActor");
#define D278_4 (*(u8 **)&D278_o4)
extern struct { char _[16]; } D278_o5 __asm__("g_ActiveActor");
#define D278_5 (*(u8 **)&D278_o5)
extern struct { char _[16]; } D278_o6 __asm__("g_ActiveActor");
#define D278_6 (*(u8 **)&D278_o6)
extern struct { char _[16]; } D278_o7 __asm__("g_ActiveActor");
#define D278_7 (*(u8 **)&D278_o7)
extern struct { char _[16]; } D278_o8 __asm__("g_ActiveActor");
#define D278_8 (*(u8 **)&D278_o8)
extern struct { char _[16]; } D278_o9 __asm__("g_ActiveActor");
#define D278_9 (*(u8 **)&D278_o9)
extern struct { char _[16]; } D278_o10 __asm__("g_ActiveActor");
#define D278_10 (*(u8 **)&D278_o10)
extern struct { char _[16]; } D278_o11 __asm__("g_ActiveActor");
#define D278_11 (*(u8 **)&D278_o11)
extern struct { char _[16]; } D278_o12 __asm__("g_ActiveActor");
#define D278_12 (*(u8 **)&D278_o12)
extern struct { char _[16]; } D278_o13 __asm__("g_ActiveActor");
#define D278_13 (*(u8 **)&D278_o13)
extern struct { char _[16]; } D278_o14 __asm__("g_ActiveActor");
#define D278_14 (*(u8 **)&D278_o14)
extern struct { char _[16]; } D278_o15 __asm__("g_ActiveActor");
#define D278_15 (*(u8 **)&D278_o15)
extern struct { char _[16]; } D278_o16 __asm__("g_ActiveActor");
#define D278_16 (*(u8 **)&D278_o16)
extern struct { char _[16]; } D278_o17 __asm__("g_ActiveActor");
#define D278_17 (*(u8 **)&D278_o17)

extern struct { char _[16]; } D228_o __asm__("g_ActorEffectFlag100Timer");
#define g_ActorEffectFlag100Timer (*(s16 *)&D228_o)
extern struct { char _[16]; } D1CE_o __asm__("g_BattleSaveOverlayActive");
#define g_BattleSaveOverlayActive (*(s8 *)&D1CE_o)
extern struct { char _[16]; } D1F8_o __asm__("g_CurItemEffectData");
#define g_CurItemEffectData (*(s32 *)&D1F8_o)
extern struct { char _[16]; } D2E8_oa __asm__("g_FieldMoveLock");
extern struct { char _[16]; } D2E8_ob __asm__("g_FieldMoveLock");
#define D2E8A (*(s32 *)&D2E8_oa)
#define D2E8B (*(s32 *)&D2E8_ob)

void Battle_ApplyEnemyAttack(u8 *ent) {
    s32 *ps = (s32 *)(D278_0 + 0x4C);

    switch (*(u8 *)(*(u8 **)(ent + 0x18) + 1)) {
    case 1:
        if ((*ps & 3) == 1) {
            break;
        }
        if (*(s32 *)(*(u8 **)(D278_1 + 0x6C) + 4) & 1) {
            if ((rand() % 100) < 0x46) {
                break;
            }
        }
        {
            s32 f = *ps;
            if ((f & 3) == 3) {
                *ps = f & ~3;
                break;
            }
            {
                u8 *pD = D278_2;
                u8 lvl;
                *ps = (f & ~3) | 1;
                *(s16 *)(pD + 0x40) = 0x2328;
                lvl = *(u8 *)(ent + 0x94);
                *(s8 *)(pD + 0x3A) = 0;
                *(s16 *)(pD + 0x38) = lvl;
            }
        }
        {
            u8 *pD = D278_3;
            *(s8 *)(pD + 0x3B) = *(u8 *)(ent + 0x95);
        }
        break;
    case 2:
        {
            u8 *pD;
            register s32 f asm("$2");
            pD = D278_4;
            f = *(s32 *)(pD + 0x4C);
            g_ActorEffectFlag100Timer = 0;
            *(s32 *)(pD + 0x4C) = f & ~0x100;
            f = *(volatile s32 *)(pD + 0x4C);
            f &= ~0x200;
            BARRIER(f);
            f &= ~0x400;
            BARRIER(f);
            f &= ~0x800;
            *(s32 *)(pD + 0x4C) = f;
        }
        break;
    case 3:
        if ((*ps & 0xC) == 4) {
            break;
        }
        if (*(s32 *)(*(u8 **)(D278_5 + 0x6C) + 4) & 2) {
            if ((rand() % 100) < 0x3C) {
                break;
            }
        }
        {
            s32 f = *ps;
            s32 r;
            s32 a3v;
            s32 d;
            u8 *pD;
            if ((f & 0xC) == 0xC) {
                *ps = f & ~0xC;
                break;
            }
            d = D2E8A;
            *ps = (f & ~0xC) | 4;
            D2E8B = d | 0x10;
            r = rand();
            pD = D278_6;
            a3v = r;
            if (r < 0) {
                a3v = r + 3;
            }
            *(s16 *)(pD + 0x42) = 0x2328;
            *(s32 *)(pD + 0x4C) = (*(s32 *)(pD + 0x4C) & ~0x60000) | (((r - ((a3v >> 2) << 2)) & 3) << 17);
        }
        break;
    case 4:
        if ((*ps & 0x30) == 0x10) {
            break;
        }
        if (*(s32 *)(*(u8 **)(D278_7 + 0x6C) + 4) & 4) {
            if ((rand() % 100) < 0x3C) {
                break;
            }
        }
        {
            s32 f = *ps;
            u8 *pD;
            if ((f & 0x30) == 0x30) {
                *ps = f & ~0x30;
                break;
            }
            pD = D278_8;
            *ps = (f & ~0x30) | 0x10;
            *(s16 *)(pD + 0x44) = 0x2328;
        }
        break;
    case 5:
        if ((*ps & 0xC0) == 0x80) {
            break;
        }
        if (*(s32 *)(*(u8 **)(D278_9 + 0x6C) + 4) & 8) {
            if ((rand() % 100) < 0x32) {
                break;
            }
        }
        {
            s32 f = *ps;
            if ((f & 0xC0) == 0xC0) {
                *ps = f & ~0xC0;
                break;
            }
            if (f & 0x100) {
                *ps = f & ~0x100;
            }
            {
                s32 nf = (*ps & ~0xC0) | 0x80;
                u8 *pD = D278_10;
                *ps = nf;
                *(s16 *)(pD + 0x46) = 0x2328;
            }
        }
        break;
    case 6:
        {
            s32 m = *ps & 0xC0;
            if (m == 0x80) {
                break;
            }
            if (m == 0x40) {
                break;
            }
        }
        if (*(s32 *)(*(u8 **)(D278_11 + 0x6C) + 4) & 8) {
            if ((rand() % 100) < 0x46) {
                break;
            }
        }
        {
            s32 f = *ps;
            if ((f & 0xC0) == 0xC0) {
                *ps = f & ~0xC0;
                break;
            }
            if (f & 0x100) {
                *ps = f & ~0x100;
            }
            {
                s32 nf = (*ps & ~0xC0) | 0x40;
                u8 *pD = D278_12;
                *ps = nf;
                *(s16 *)(pD + 0x46) = 0x2328;
            }
        }
        break;
    case 7:
        *ps = *ps | 0x1000;
        break;
    case 8:
        if (!(*ps & 0x200)) {
            u8 *pD = D278_13;
            s32 tmp = *(u16 *)(pD + 0xC) << 16;
            if ((tmp >> 16) >= 2) {
                *(s16 *)(pD + 0xC) = tmp >> 17;
            }
        }
        break;
    case 9:
        {
            u8 *pD = D278_14;
            s32 hp;
            s32 f;
            g_ActorEffectFlag100Timer = 0;
            hp = *(s16 *)(pD + 0xC);
            *(s32 *)(pD + 0x4C) = *(s32 *)(pD + 0x4C) & ~0x100;
            f = *(volatile s32 *)(pD + 0x4C);
            f &= ~0x200;
            BARRIER(f);
            f &= ~0x400;
            BARRIER(f);
            f &= ~0x800;
            *(s32 *)(pD + 0x4C) = f;
            if (hp >= 2) {
                *(s16 *)(pD + 0xC) = 1;
            } else if (hp == 1) {
                *(s16 *)(pD + 0xC) = -1;
            }
        }
        break;
    case 10:
    case 11:
        if (*(s32 *)(*(u8 **)(D278_15 + 0x6C) + 4) & 0x10) {
            s32 r = rand();
            if ((r % 100) >= 0x3C) {
                *(s16 *)(ent + 0xA0) = Inv_PickRandomItem(r / 100);
                *(u8 *)(*(u8 **)(ent + 0x18) + 1) = 0;
                Inv_FindItemById(*(s16 *)(ent + 0xA0));
                {
                    s32 it = *(s16 *)(ent + 0xA0);
                    g_BattleSaveOverlayActive = 1;
                    g_CurItemEffectData = Inv_GetItemEffectData(it, 0);
                }
            }
        }
        break;
    case 12:
    case 13:
        if (*(s32 *)(*(u8 **)(D278_16 + 0x6C) + 4) & 0x10) {
            if ((rand() % 100) >= 0x3C) {
                Inv_RollRandomItemType(ent + 0xA0, ent + 0xA2);
                *(u8 *)(*(u8 **)(ent + 0x18) + 1) = 0;
                {
                    s32 it = *(s16 *)(ent + 0xA0);
                    g_BattleSaveOverlayActive = 1;
                    g_CurItemEffectData = Inv_GetItemEffectData(it, 0);
                }
            }
        }
        break;
    case 16:
        if (!(*ps & 0x200)) {
            u8 *pD = D278_17;
            s16 t = *(s16 *)(pD + 0xC);
            if (t >= 2) {
                *(s16 *)(pD + 0xC) = (t * 3) / 4;
            }
        }
        break;
    }
}
