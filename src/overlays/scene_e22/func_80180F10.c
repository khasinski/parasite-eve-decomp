typedef unsigned char u8;
typedef signed short s16;

void func_8018FF78(void *arg0);

int func_80180F10(char *arg0) {
    register u8 value asm("$5");
    register char *slot asm("$6");
    register char *child asm("$2");
    register char *event asm("$4");

    value = 4;
    slot = arg0 + 0xC;
    arg0[0] = value;
    arg0[3] = 0;

    if (arg0[0xB8] == 0) {
        if (*(s16 *)(arg0 + 0x14) == 0) {
            goto done;
        }
        child = *(char **)(arg0 + 8);
        child = *(char **)child;
        if (child == 0) {
            goto done;
        }
        child = *(char **)(child + 0x18);
        asm volatile(
            ".set\tnoreorder\n\t"
            "j\tfunc_8018FF78\n\t"
            "sb\t%1, 0(%0)\n\t"
            ".set\treorder"
            :
            : "r"(child), "r"(value));
    }

nonzero:
    {
        child = *(char **)(arg0 + 8);
        child = *(char **)child;
        if (child != 0) {
            event = *(char **)(child + 0x18);
            if (event[0] == 1) {
                event[0] = value;
            }
        }
    }

done:
    child = *(char **)(slot + 4);
    if (child != 0) {
        *(void **)child = 0;
    }
    return 0;
}
