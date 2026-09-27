#include "common.h"

u32 Task_GpuFlushPrimQueue(void) {
    register u32 *base asm("$8");
    register u32 *head_slot asm("$15");
    register u32 *tail_slot asm("$24");
    register s32 head asm("$9");
    register s32 tail asm("$10");
    register u32 *head_ptr asm("$11");
    register u32 *tail_ptr asm("$12");
    register u32 head_value asm("$13");
    register u32 tail_value asm("$14");
    u32 ret;
    /* Match debt: read architectural zero without emitting an instruction;
     * OR/ORI preserve the original encodings of the return and wrap value. */
    register u32 zero asm("$0");
    asm volatile("" : "=r"(zero));
    base = (u32 *)0x80070E0C;
    head_slot = (u32 *)0x80070E04;
    tail_slot = (u32 *)0x80070E08;
    head = *head_slot;
    tail = *tail_slot;
    /* Keep the base as a register operand rather than a folded constant. */
    asm volatile("" : "=r"(base) : "0"(base));

    /* Match note: preserve target operand order in the address adds. */
    head_ptr = (u32 *)((u32)base + (u32)head);
    tail_ptr = (u32 *)((u32)base + (u32)tail);
    asm volatile("" : "=r"(head_ptr), "=r"(tail_ptr) : "0"(head_ptr), "1"(tail_ptr));

    head_value = *head_ptr;
    tail_value = *tail_ptr;

    head_value += tail_value;
    *head_ptr = head_value;
    /* Finish the entry write and result copy before updating queue offsets. */
    asm volatile("" : "=r"(head_value) : "0"(head_value) : "memory");
    ret = zero | head_value;
    asm volatile("" : "=r"(ret), "=r"(head), "=r"(tail) : "0"(ret), "1"(head), "2"(tail));

    head = (u32)head - 4;
    tail = (u32)tail - 4;
    if (head < 0) {
        head = zero | 0x40;
    }
    *head_slot = head;
    if (tail < 0) {
        tail |= 0x40;
    }
    *tail_slot = tail;
    return ret;
}
