#include "include_asm.h"

typedef unsigned int u32;
typedef int s32;

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
    register u32 ret asm("$2");

    base = (u32 *)0x80070E0C;
    head_slot = (u32 *)0x80070E04;
    tail_slot = (u32 *)0x80070E08;
    head = *head_slot;
    tail = *tail_slot;

    /* Match note: preserve target operand order in the address adds. */
    asm volatile("addu %0,%1,%2" : "=r"(head_ptr) : "r"(base), "r"(head));
    asm volatile("addu %0,%1,%2" : "=r"(tail_ptr) : "r"(base), "r"(tail));

    head_value = *head_ptr;
    tail_value = *tail_ptr;
    asm volatile("" : "=r"(head_value), "=r"(tail_value) : "0"(head_value), "1"(tail_value));
    head_value += tail_value;
    asm volatile("sw %1,0(%0)" : : "r"(head_ptr), "r"(head_value));
    asm volatile("or %0,$0,%1" : "=r"(ret) : "r"(head_value));

    head -= 4;
    tail -= 4;
    if (head < 0) {
        asm volatile("ori %0,$0,0x40" : "=r"(head));
    }
    *head_slot = head;
    if (tail < 0) {
        tail |= 0x40;
    }
    *tail_slot = tail;
    return ret;
}
