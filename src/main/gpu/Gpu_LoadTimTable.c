/* CC1_PROFILE: build0001 */

void Gpu_LoadTimImage(int arg0);

void Gpu_LoadTimTable(int base, int count) {
    int i;

    for (i = 0; i < count; i++) {
        int ptr = ((short)i << 2) + base;
        int offset = *(int *)ptr;
        Gpu_LoadTimImage(base + offset);
    }
}
