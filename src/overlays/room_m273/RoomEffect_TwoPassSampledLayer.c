#include "room_m273.h"

int func_8019320C(int mode, RoomM273SampledLayerEffect *effect) {
    RoomM273Vector position, rotation;
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
        rotation.pad = -1;
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


extern RoomPlacementStateContext *D_800F32D0;

int func_801933A0(int mode) {
    switch(mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool,12,9,func_8019320C);
    case 1: {
        RoomM273SampledLayerEffect *effect;
        RoomPlacementMap *transform;
        if(D_800E27EC>=12) return 2;
        effect=func_800CE610(D_800F33E0->pool);
        if(!effect) break;
        transform=D_800F32D0->owner->map;
        effect->position.x=transform->x;
        asm("" : : : "memory");
        effect->position.y=transform->y;
        effect->position.z=transform->z;
        effect->parameter[0]=((short *)D_800966EC)[((D_800E27EC*1024/12)&0xFFF)*2]*40/4096;
        effect->parameter[1]=8;
        {
            unsigned long address=((D_800E27EC*1024/12)&0xFFF)*4;
            address+=(unsigned long)D_800966EC;
            effect->parameter[2]=((short *)address)[1]*40/4096;
        }
        break;
    }
    case 2:
        break;
    }
    return 0;
}
