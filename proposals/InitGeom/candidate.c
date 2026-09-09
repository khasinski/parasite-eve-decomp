#include "common.h"

extern void *D_800960AC;
void St_InstallDmaHandler(void);

#define initgeom_control(reg, value) \
    asm volatile("ctc2 %0,$" #reg "\n\tnop" : : "r"(value))

void InitGeom(void) {
    register void *return_address asm("$31");
    u32 status;
    asm volatile("" : "=r"(return_address));
    D_800960AC = return_address;
    St_InstallDmaHandler();
    asm volatile("mfc0 %0,$12\n\tnop" : "=r"(status));
    status |= 0x40000000;
    asm volatile("mtc0 %0,$12\n\tnop" : : "r"(status));
    initgeom_control(29, 0x155);
    initgeom_control(30, 0x100);
    initgeom_control(26, 1000);
    initgeom_control(27, -0x1062);
    initgeom_control(28, 0x01400000);
    asm volatile("ctc2 $zero,$24\n\tctc2 $zero,$25\n\tnop");
}
