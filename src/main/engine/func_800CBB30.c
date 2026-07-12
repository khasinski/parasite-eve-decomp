typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

int func_800CBB30(void *arg0, u8 *state, u8 *anim) {
    register u8 *state_a3 asm("$7") = state;
    register u8 *anim_a2 asm("$6") = anim;
    register u8 *cur_a0 asm("$4");
    u16 x_vel;
    u16 y_vel;
    u16 z_vel;
    u16 x_pos;
    u8 count;

    asm volatile("" : "=r"(state_a3) : "0"(state_a3));
    x_pos = *(u16 *)(anim_a2 + 8);
    x_vel = *(u16 *)(anim_a2 + 0x10);
    y_vel = *(u16 *)(anim_a2 + 0x12);
    z_vel = *(u16 *)(anim_a2 + 0x14);
    *(u16 *)(anim_a2 + 8) = x_pos + x_vel;
    *(u16 *)(anim_a2 + 0xA) = *(u16 *)(anim_a2 + 0xA) + y_vel;
    *(u16 *)(anim_a2 + 0xC) = *(u16 *)(anim_a2 + 0xC) + z_vel;
    cur_a0 = anim_a2;
    *(u16 *)(anim_a2 + 0x12) = *(u16 *)(anim_a2 + 0x12) + 3;
    anim_a2[1]++;
    if (*(s16 *)(anim_a2 + 0xA) > 0) {
        *(u16 *)(anim_a2 + 0x12) = -*(u16 *)(anim_a2 + 0x12);
    }
    count = cur_a0[2];
    cur_a0[2] = count - 1;
    if (count == 0) {
        state_a3[1] = 2;
    }
}
