#include "../room_lib/room_lib.h"

typedef struct RoomRenderWords8 RoomRenderWords8;
struct RoomRenderWords8 {
    int words[8];
};

void func_80192F34(void *unused0, void *unused1, void *output_arg) {
    register char *base asm("$17") = output_arg;
    register int index asm("$19");
    int red;
    register int blue asm("$23");
    register int mid asm("$22");
    register char *color asm("$20");
    register char *scale asm("$21");
    register char *motion asm("$18");
    char *clock;
    unsigned int speed;
    register int value asm("$2");

    clock = (char *)func_800C2B50();
    PE1_COMPILER_LAUNDER(base);
    *(RoomRenderWords8 *)base = *(RoomRenderWords8 *)(clock + 4);

    index = 0;
    red = 159;
    blue = 137;
    mid = 64;
    PE1_COMPILER_USE(index);
    PE1_COMPILER_USE(red);
    PE1_COMPILER_USE(blue);
    PE1_COMPILER_USE(mid);
    scale = base;
    PE1_COMPILER_LAUNDER(scale);
    color = scale;
    motion = scale;

    do {
        speed = (int)rand() % 40 + 10;
        rand();

        value = -200;
        RW16(motion, 0x32) = value;
        RW16(motion, 0x34) = value;
        value = -(speed >> 1);
        speed <<= 6;
        RW16(motion, 0x50) = value;
        RW16(motion, 0x52) = value;
        value = 160;
        RW16(motion, 0x30) = 0;
        RW16(scale, 0x78) = value;
        RW16(scale, 0x70) = speed;

        if (index == 1) goto color1;
        if (index == 0) goto color0;
        if (index == 2) goto color2;
        if (index == 3) goto color3;
        color += 4;
        goto advance_scale;

color0:
        color[0x20] = red;
        color[0x21] = 138;
        color[0x22] = blue;
        goto advance_color;
color1:
        base[0x24] = mid;
        base[0x25] = 76;
        base[0x26] = 6;
        goto advance_color;
color2:
        base[0x28] = 18;
        base[0x29] = 32;
        base[0x2A] = mid;
        goto advance_color;
color3:
        base[0x2C] = red;
        base[0x2D] = 98;
        base[0x2E] = blue;
advance_color:
        color += 4;
advance_scale:
        scale += 2;
        motion += 8;
        index++;
    } while ((unsigned int)index < 4);
}
