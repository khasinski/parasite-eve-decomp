void SetBackColor(int r, int g, int b) {
    asm("sll $4,$4,4");
    asm("sll $5,$5,4");
    asm("sll $6,$6,4");
    asm("ctc2 $4,$13");
    asm("ctc2 $5,$14");
    asm("ctc2 $6,$15");
}
