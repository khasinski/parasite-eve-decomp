#include "fx_common.h"

/* Pins and empty scheduling barriers preserve the retail argument setup,
 * interleaved vector reads, and snapshot stores; tracked in crutch debt. */
void func_80195994(s32 index, s32 first, s32 second, s32 source) {
    FxCommonMotionVec point;
    s32 extra[2];
    register s32 rawIndex asm("$16") = index;
    s32 offset;
    register s32 firstValue asm("$18");
    register s32 secondValue asm("$19");
    u8 *table = &D_801D0260;
    s32 *extraPtr;
    register s32 sourceValue asm("$22");
    register void *lookupTable asm("$4");
    register s32 lookupCount asm("$5");
    s32 *base;
    register s32 bx asm("$9");
    register s32 by asm("$6");
    register s32 bz asm("$3");
    register s32 ax asm("$7");
    register s32 ay asm("$4");
    register s32 az asm("$3");
    register s32 px asm("$10");
    register s32 py asm("$8");
    register s32 pz asm("$7");
    register s32 qx asm("$8");
    register s32 qy asm("$6");
    register s32 qz asm("$5");
    register void *nextTable asm("$4");

    lookupTable = table;
    firstValue = first;
    lookupCount = 2;
    sourceValue = source;
    /* The tied operand copies second unchanged after the other call arguments. */
    asm("" : "=r"(secondValue) : "0"(second), "r"(lookupTable), "r"(lookupCount), "r"(sourceValue));
    base = func_8006EC6C(lookupTable, lookupCount);
    asm volatile("" : "=r"(rawIndex) : "0"(rawIndex), "r"(base));
    offset = (s16)rawIndex * 52;
    extraPtr = extra;
    func_8018F55C(sourceValue, D_801EA378[offset], base, &point, extraPtr);

    nextTable = table;
    asm volatile("" : : "r"(nextTable));
    firstValue &= 0xff;
    px = point.x;
    asm volatile("" : : "r"(px) : "memory");
    bx = D_8019C810;
    asm volatile("" : : "r"(bx) : "memory");
    py = point.y;
    asm volatile("" : : "r"(py) : "memory");
    by = D_8019C814;
    asm volatile("" : : "r"(by) : "memory");
    pz = point.z;
    asm volatile("" : : "r"(pz) : "memory");
    bz = D_8019C818;
    asm volatile("" : : "r"(bz) : "memory");
    rawIndex = 1;
    D_8019C056 = firstValue;
    firstValue = rawIndex << firstValue;
    D_8019C09C = 0;
    D_8019C0A0 = 0;
    D_8019C0A4 = 0;
    D_8019C054 = firstValue;
    asm volatile("" : "=r"(px) : "0"(px));
    D_8019C08C = px - bx;
    D_8019C090 = py - by;
    D_8019C094 = pz - bz;
    D_8019C0AC = bx;
    D_8019C0AE = by;
    D_8019C0B0 = bz;
    D_8019C0B4 = px;
    D_8019C0B6 = py;
    D_8019C0B8 = pz;

    asm volatile("" : : : "5");
    base = func_8006EC6C(nextTable, 2);
    func_8018F55C(sourceValue, D_801EA378[offset + 1], base, &point, extraPtr);

    asm volatile("" : "=r"(secondValue) : "0"(secondValue));
    secondValue &= 0xff;
    asm volatile("" : : "r"(secondValue) : "memory");
    qx = point.x;
    asm volatile("" : : "r"(qx) : "memory");
    ax = D_8019C330;
    asm volatile("" : : "r"(ax) : "memory");
    qy = point.y;
    asm volatile("" : : "r"(qy) : "memory");
    ay = D_8019C334;
    asm volatile("" : : "r"(ay) : "memory");
    qz = point.z;
    asm volatile("" : : "r"(qz) : "memory");
    az = D_8019C338;
    asm volatile("" : : "r"(az) : "memory");
    rawIndex = 1;
    rawIndex <<= secondValue;
    D_8019C06C = 0;
    D_8019C070 = 0;
    D_8019C074 = 0;
    D_8019C052 = secondValue;
    D_8019C050 = rawIndex;
    asm volatile("" : "=r"(qx) : "0"(qx));
    D_8019C05C = qx - ax;
    D_8019C060 = qy - ay;
    D_8019C064 = qz - az;
    D_8019C07C = ax;
    D_8019C07E = ay;
    D_8019C080 = az;
    D_8019C084 = qx;
    D_8019C086 = qy;
    D_8019C088 = qz;
}
