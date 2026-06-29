void SetGeomOffset(int x, int y) {
    asm("sll $4,$4,16");
    asm("sll $5,$5,16");
    asm("ctc2 $4,$24");
    asm("ctc2 $5,$25");
}
