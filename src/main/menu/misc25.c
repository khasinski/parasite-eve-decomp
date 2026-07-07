#include "include_asm.h"

extern unsigned int *g_PlayerEntity;

extern unsigned short g_AyaStatAgility;

extern unsigned short g_AyaStatActiveTimeRate;

int Battle_IsActive(void);

int Pad_IsMenuConfirmAvailable(void);

extern unsigned int g_GameStateFlags;

extern void * volatile g_AkaoBgmHandle;
int Akao_SendTableCommand(void *arg0, int arg1, int arg2, int arg3, int arg4);
extern unsigned char D_80091694[];
extern unsigned char D_8009169D;

int Battle_GetEnemyEscapeFlag(void)
{
    unsigned int *ptr;

    if (g_PlayerEntity != 0) {
        ptr = (int *)g_PlayerEntity[0];
        if (ptr != 0) {
            return (ptr[0x13] >> 9) & 1;
        }
    }

    return 0;
}

int Battle_GetAgilityBonus(void)
{
    return g_AyaStatAgility;
}

unsigned int Aya_GetActiveTimeRate(void)
{
    return g_AyaStatActiveTimeRate;
}

int Battle_IsActiveWrapped(void)
{
    return (signed char)Battle_IsActive();
}

int Battle_IsInputAllowedWrapped(void)
{
    return (signed char)Pad_IsMenuConfirmAvailable();
}

int Battle_GetStateFlag1(void)
{
    return (g_GameStateFlags >> 1) & 1;
}

void Menu_CopyPromptCodes(unsigned char *src) {
    register unsigned char *dst asm("$5");
    register unsigned char *end asm("$3");
    int value;

    dst = D_80091694;
    end = dst + 8;
    while (dst < end) {
        value = *src;
        if (value == 0xFF) {
            break;
        }
        *dst++ = value;
        src++;
    }

    {
        register unsigned char *len_slot asm("$3");
        register unsigned char *base asm("$2");
        register int length asm("$2");

        len_slot = &D_8009169D;
        base = len_slot - 9;
        length = dst - base;
        *len_slot = length;
    }
}

void Menu_PlayConfirmSound(void) {
    void * volatile *slot;

    slot = &g_AkaoBgmHandle;
    if (*slot != 0) {
        Akao_SendTableCommand(*slot, 0x44C, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayCancelSound(void) {
    void * volatile *slot;

    slot = &g_AkaoBgmHandle;
    if (*slot != 0) {
        Akao_SendTableCommand(*slot, 0x44D, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayMoveSound(void) {
    void * volatile *slot;

    slot = &g_AkaoBgmHandle;
    if (*slot != 0) {
        Akao_SendTableCommand(*slot, 0x44E, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayErrorSound(void) {
    void * volatile *slot;

    slot = &g_AkaoBgmHandle;
    if (*slot != 0) {
        Akao_SendTableCommand(*slot, 0x44F, 0x100, 0x80, 0x7F);
    }
}
