#include "common.h"

extern int func_8007F418(int, int);
extern void func_80080DC4(int, void *, void *);

int func_80125794(int code) {
    int result[2];
    u8 command[2];
    command[0] = 1;
    command[1] = code;
    while (func_8007F418(0, 0) != 2) {}
    func_80080DC4(13, command, result);
    return 0;
}
