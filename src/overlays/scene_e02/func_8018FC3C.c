#include "common.h"
typedef struct {
    u8 field_0;
    u8 field_1;
    s16 field_2;
} SceneE02State;

void func_8018FC3C(void *arg0, SceneE02State *arg1) {
    if (arg1->field_2 == 0x3A) {
        arg1->field_1 = 2;
    }
}
