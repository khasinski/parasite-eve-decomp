extern int D_80094538;
extern void (*jtbl_80010000[])(void);

void __main(void) {
    register void (**ctor)() asm("$16");
    register int count asm("$17");
    register void (*fn)() asm("$8");
    register int initialized asm("$8");

    initialized = D_80094538;
    if (initialized != 0) {
        return;
    }

    initialized = 1;
    D_80094538 = initialized;
    ctor = jtbl_80010000;
    asm volatile(
        "lui %0, 0\n"
        "addiu %0, %0, 0"
        : "=r"(count));
    if (count != 0) {
        do {
            fn = *ctor++;
            asm volatile(
                "jalr %1\n"
                "addiu %0, %0, -1"
                : "=r"(count)
                : "r"(fn), "0"(count)
                : "$31");
        } while (count != 0);
    }
}
