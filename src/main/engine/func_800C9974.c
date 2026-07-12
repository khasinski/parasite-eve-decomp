typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

int func_800C9974(void *arg0, u8 *state, u8 *anim) {
    register u8 *state_a3 asm("$7") = state;
    register u8 *anim_a2 asm("$6") = anim;
    register int temp_v0 asm("$2");
    register int temp_v1 asm("$3");
    register int temp_a0 asm("$4");
    register int temp_a1 asm("$5");
    u8 count;

    asm volatile("" : "=r"(state_a3) : "0"(state_a3));
    temp_v0 = *(u16 *)(anim_a2 + 0x8);
    temp_v1 = *(u16 *)(anim_a2 + 0x10);
    temp_a0 = *(u16 *)(anim_a2 + 0x12);
    temp_a1 = *(u16 *)(anim_a2 + 0x14);
    asm volatile("" : "=r"(temp_a1) : "0"(temp_a1));
    temp_v0 += temp_v1;
    *(u16 *)(anim_a2 + 0x8) = temp_v0;
    temp_v0 = *(u16 *)(anim_a2 + 0xA);
    temp_v1 = *(u16 *)(anim_a2 + 0xC);
    temp_v0 += temp_a0;
    temp_v1 += temp_a1;
    *(u16 *)(anim_a2 + 0xA) = temp_v0;
    temp_v0 = *(u16 *)(anim_a2 + 0x12);
    temp_a0 = (unsigned int)anim_a2;
    *(u16 *)(anim_a2 + 0xC) = temp_v1;
    temp_v1 = anim_a2[1];
    temp_v0 += 3;
    *(u16 *)(anim_a2 + 0x12) = temp_v0;
    temp_v0 = *(s16 *)(anim_a2 + 0xA);
    temp_v1++;
    anim_a2[1] = temp_v1;
    if (temp_v0 > 0) {
        *(u16 *)(anim_a2 + 0x12) = -*(u16 *)(anim_a2 + 0x12);
    }
    count = ((u8 *)temp_a0)[2];
    ((u8 *)temp_a0)[2] = count - 1;
    if (count == 0) {
        state_a3[1] = 2;
    }
}
