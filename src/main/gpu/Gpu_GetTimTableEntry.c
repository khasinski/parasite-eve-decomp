/* CC1_PROFILE: build0001 */

int Gpu_GetTimTableEntry(int base, int index) {
    int offset = (short)index << 2;
    int ptr = offset + base;
    return base + *(int *)ptr;
}
