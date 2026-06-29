extern char D_80092478[];

void *MenuWidget_LookupSimpleDescriptor(unsigned int index) {
    if (index >= 0x41) {
        return 0;
    }
    return D_80092478 + index * 0x10;
}
