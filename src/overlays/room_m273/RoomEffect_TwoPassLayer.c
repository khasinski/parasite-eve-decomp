typedef struct { short x,y,z,w; } Vector;
typedef struct { Vector position; unsigned char parameter[4]; } Effect;
#include "room_m273.h"

int func_8019320C(int mode, Effect *effect) {
    Vector position, rotation;
    if (mode == 1) {
        if (D_800E27EC >= 8) return 1;
    } else if (mode == 2) {
        short frame = D_800E27EC - 1;
        int offset = ((unsigned int)frame << 9) & 0x3e00;
        int product = ((short *)D_800966EC)[offset / 2] * 2 + 4096;
        int sample = ((short *)D_800966EC)[offset / 2 + 1];
        short size = product;
        short shade = (unsigned int)sample >> 5;
        short i;
        rotation.x = 1024;
        rotation.y = 0;
        rotation.z = frame * 170;
        rotation.w = -1;
        position = effect->position;
        i = 0;
        do {
            func_800D004C(&position, 128, 128, 12, &rotation,
                          (short)size, (short)size, D_8019AB70,
                          effect->parameter, (short)shade, 1);
            shade = (short)shade >> 1;
            size = (short)size * 3 / 2;
            i++;
            position.y = D_800942EC;
        } while (i < 2);
    }
    return 0;
}
