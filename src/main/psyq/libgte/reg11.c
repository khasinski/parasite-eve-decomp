void SetFarColor(int r, int g, int b) {
    asm("sll $4,$4,4");
    asm("sll $5,$5,4");
    asm("sll $6,$6,4");
    asm("ctc2 $4,$21");
    asm("ctc2 $5,$22");
    asm("ctc2 $6,$23");
}
