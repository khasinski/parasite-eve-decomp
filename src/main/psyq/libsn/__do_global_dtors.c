/* CC1_FLAGS: -fcall-used-$16 -fcall-used-$17 */
#include "pe1/psyq_libsn.h"

void __do_global_dtors(void) {
    /* SNMAIN uses a 16-byte runtime frame without GCC's outgoing-call area. */
    typedef union RuntimeStackWord {
        u32 word;
        void (**functionPointer)(void);
    } RuntimeStackWord;
    typedef union RuntimeValue {
        s32 initialized;
        void (*function)(void);
    } RuntimeValue;
    register RuntimeStackWord *stackPointer asm("$29");
    register void (**destructor)(void) asm("$16");
    register s32 remaining asm("$17");
    register RuntimeValue value asm("$8");

    value.initialized = D_80094538;
    stackPointer -= 4;
    stackPointer[1].functionPointer = destructor;
    stackPointer[2].word = remaining;
    asm volatile("sw $31, 12($29)" : : : "memory");

    if (value.initialized != 0) {
        destructor = jtbl_80010000;
        asm volatile(
            "lui %0, 0\n"
            "addiu %0, %0, 0"
            : "=r"(remaining));
        if (remaining != 0) {
            do {
                value.function = *destructor++;
                asm volatile(
                    "jalr %1\n"
                    "addiu %0, %0, -1"
                    : "=r"(remaining)
                    : "r"(value.function), "0"(remaining));
            } while (remaining != 0);
        }
    }

    asm volatile("lw $31, 12($29)" : : : "memory");
    remaining = stackPointer[2].word;
    destructor = stackPointer[1].functionPointer;
    stackPointer += 4;
    asm volatile("" : : "r"(stackPointer), "r"(destructor), "r"(remaining));
}
