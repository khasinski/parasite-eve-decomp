#include "common.h"

extern int func_8007F418(int, int);
extern void func_80080DC4(int, void *, void *);
extern int D_80172CE8;
extern volatile int D_80172CF0;
extern volatile int D_80172CF4;
extern void func_80080B44(int, void *);
extern int func_8007F0C8(int, void *, int, int, int);

int func_80125794(int code) {
    int result[2];
    u8 command[2];
    command[0] = 1;
    command[1] = code;
    while (func_8007F418(0, 0) != 2) {}
    func_80080DC4(13, command, result);
    return 0;
}

int func_801257EC(int command, int parameter) {
    int result[2];
    int status;
    int *request = &D_80172CE8;
    func_80080B44(*request, result);
    status = func_8007F0C8((u8)command, result, (u8)parameter, 0, -1);
    D_80172CF0 = *request;
    D_80172CF4++;
    return (u8)status;
}
