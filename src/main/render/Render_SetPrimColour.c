#include "pe1/render_prim.h"

void Render_SetPrimColour(PrimObj *obj, unsigned char a, unsigned char b, unsigned char c) {
    int i;
    int offset;
    unsigned char *base;

    for (i = 0; i < obj->count; i++) {
        offset = i << 4;
        base = (unsigned char *) obj->entries;
        base[offset + 4] = a;
        base[offset + 5] = b;
        base[offset + 6] = c;

        base = (unsigned char *) obj->entries + (obj->count << 4);
        base[offset + 4] = a;
        base[offset + 5] = b;
        base[offset + 6] = c;
    }
}
