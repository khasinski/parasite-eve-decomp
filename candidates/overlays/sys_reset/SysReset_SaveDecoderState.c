#include "common.h"
#include "pe1/sys_reset.h"


int *func_8010BEFC(int *source) {
    int *output;
    int *input;
    int i;

    output = D_8010DA10;
    input = source;
    i = 15;
    do {
        *output++ = *input++;
        i--;
    } while (i != -1);

    output = D_8010DA50;
    input = source + 16;
    i = 15;
    do {
        *output++ = *input++;
        i--;
    } while (i != -1);

    func_8010C1EC(D_8010DA0C, 0x20);
    func_8010C1EC(D_8010DA90, 0x20);
    return source;
}
