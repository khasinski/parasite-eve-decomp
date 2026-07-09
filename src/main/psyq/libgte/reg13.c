void SetGeomScreen(int h) {
    asm volatile("ctc2 %0,$26" : : "r"(h));
}
