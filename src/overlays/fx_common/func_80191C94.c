#include "common.h"

typedef struct FxAssetSource {
    u8 reserved[0x100];
    u8 *data;
} FxAssetSource;

extern FxAssetSource D_800B0CD8;
extern u16 D_80093170[];
extern u16 D_80093174[];
extern u16 D_80093172[];
extern char D_801D0260[];
extern char D_8019CE10[];
extern u8 D_8019C1F0;

int func_8006E6A8(void *, void *, int);
int func_8006E7E8(void);

void func_80191C94(void) {
    FxAssetSource *resource = &D_800B0CD8;
    u16 *range;
    int result;

first:
    range = D_80093170;
    do {
        result = func_8006E6A8(resource->data + range[0],
                               D_801D0260, range[1] - range[0]);
    } while (result == -1);
    for (;;) {
        result = func_8006E7E8();
        if (result == 0) break;
        if (result == -1) goto first;
    }

    if (D_8019C1F0) goto third;

second:
    range = D_80093174;
    do {
        result = func_8006E6A8(resource->data + range[0],
                               D_8019CE10, range[1] - range[0]);
    } while (result == -1);
    for (;;) {
        result = func_8006E7E8();
        if (result == 0) return;
        if (result == -1) goto second;
    }

third:
    range = D_80093172;
    do {
        result = func_8006E6A8(resource->data + range[0],
                               D_8019CE10, range[1] - range[0]);
    } while (result == -1);
    for (;;) {
        result = func_8006E7E8();
        if (result == 0) break;
        if (result == -1) goto third;
    }
}
