#include "common.h"
#include "../room_lib/RoomLib_Overlay024.h"
s32 *func_800C2B10(s32 slot);
s32 func_80071A54(void);

void func_80191C94(void *arg0, void *arg1, RoomOverlay024VariantDSetupRecord *obj) {
    s32 random;

    obj->half2E = 0xD;
    obj->resource1 = *func_800C2B10(1);
    obj->resource2 = *func_800C2B10(2);
    obj->half26 = 1;
    obj->half2A = 0x80;
    obj->resource1_again = *func_800C2B10(1);
    obj->half2C = 0;
    obj->word30 = 0;
    obj->half22 = 0;
    obj->half14 = 0;
    random = func_80071A54();
    obj->random_mod = random % 40;
}
