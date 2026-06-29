void AddPrim(unsigned int *ot, unsigned int *prim) {
    unsigned int low_mask = 0xFFFFFF;
    register unsigned int high_mask asm("$7") = 0xFF000000;

    *prim = (*prim & high_mask) | (*ot & low_mask);
    *ot = (*ot & high_mask) | ((unsigned int)prim & low_mask);
}
