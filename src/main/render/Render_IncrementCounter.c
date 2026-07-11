/* MASPSX_FLAGS: --store-return-delay */

extern int func_8003E60C;

void Render_IncrementCounter(void) {
    register int *ptr asm("$4");
    register int value asm("$5");

    ptr = &func_8003E60C;
    value = *ptr;
    value++;
    *ptr = value;
}
