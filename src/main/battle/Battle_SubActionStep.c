typedef unsigned char u8;
typedef unsigned short u16;

extern volatile char *g_ActiveActor;
extern volatile char *g_PlayerEntity;

void Battle_SubActionStep(void) {
    volatile char *state;

    state = g_ActiveActor;
    if (*(volatile u8 *)(state + 0x3A) >= *(volatile u8 *)(state + 0x3B)) {
        register volatile char *actor asm("$4");
        register volatile char *state_v1 asm("$3");
        u16 value;
        register u16 step asm("$3");
        u16 copy;

        value = *(volatile u16 *)(state + 0xC);
        step = *(volatile u16 *)(state + 0x38);
        copy = *(volatile u16 *)(state + 0x38);
        value -= step;
        *(volatile u16 *)(state + 0xC) = value;
        value = *(volatile u16 *)(state + 0xE);
        step = *(volatile u16 *)(state + 0x38);
        *(volatile u8 *)(state + 0x3A) = 0;
        *(volatile u16 *)(state + 0x60) = copy;
        actor = g_PlayerEntity;
        value -= step;
        state_v1 = g_ActiveActor;
        *(volatile u16 *)(state + 0xE) = value;
        value = *(volatile u16 *)(actor + 0x210);
        *(volatile u16 *)(state_v1 + 0x62) = value;
        copy = *(volatile u16 *)(actor + 0x212);
        *(volatile u8 *)(state_v1 + 0x66) = 0x1E;
        *(volatile u16 *)(state_v1 + 0x64) = copy;
        state_v1 = g_ActiveActor;
        *(volatile u8 *)(state_v1 + 0x67) = 3;
    }

    {
        char *state_v1;
        u8 value;

        state_v1 = (char *)g_ActiveActor;
        value = *(u8 *)(state_v1 + 0x3A);
        value++;
        *(u8 *)(state_v1 + 0x3A) = value;
    }
}
