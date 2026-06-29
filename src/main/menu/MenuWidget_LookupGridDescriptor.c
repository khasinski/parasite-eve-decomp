extern char D_80092888[];

void *MenuWidget_LookupGridDescriptor(unsigned int index) {
    if (index >= 0x41) {
        return 0;
    }
    return D_80092888 + index * 0x20;
}
