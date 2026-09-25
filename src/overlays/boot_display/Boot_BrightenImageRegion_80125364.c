#include "pe1/boot_display.h"

void func_80125364(void)
{
    s16 line;
    register u8 *image asm("$14") = (u8 *)D_80172C98 + 0x4000;
    register u8 *screen asm("$13") = *(u8 **)((u8 *)&D_80125B88 + g_BootDisplayBufferIndex * 16) + 0x19DAC;

    for (line = 110; line < 134; line++) {
        s16 row;
        register u16 *source asm("$11") = (u16 *)image;
        register u8 *pixel asm("$8") = screen;

        for (row = 0; row < 24; row++) {
            register u32 bits asm("$9") = *source;
            if (bits) {
                s16 column;
                register u8 *green_pixel asm("$7");
                column = 0;
                asm volatile("" : : "r"(column));
                green_pixel = pixel + 1;
                asm volatile("" : : "r"(green_pixel));
                for (; column < 4; column++) {
                    int index = bits & 15;
                    if (index) {
                        int brightness = (((u16 *)D_80172C9C)[index] & 31) << 3;
                        register u32 blue asm("$6") = green_pixel[1];
                        register u32 green asm("$5") = green_pixel[0];
                        u32 red = pixel[0];
                        blue += brightness;
                        if (blue >= 256)
                            blue = 255;
                        green += brightness;
                        if (green >= 256)
                            green = 255;
                        red += brightness;
                        if (red >= 256)
                            red = 255;
                        green_pixel[1] = blue;
                        green_pixel[0] = green;
                        pixel[0] = red;
                    }
                    bits >>= 4;
                    green_pixel += 3;
                    pixel += 3;
                }
            } else {
                pixel += 12;
            }
            source++;
        }
        image += 0x80;
        screen += 0x3C0;
    }
}
