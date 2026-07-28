#ifndef PE1_GTE_H
#define PE1_GTE_H

/*
 * PSY-Q-style GTE operations.  Keep these macros limited to COP2 state and
 * commands; the calling function owns all normal CPU-side algorithmic work.
 */

/* Load the five packed words which hold R11..R33. */
#define gte_ldrotmatrix(matrix) \
    asm volatile("lw $12,0(%0)\n\t" \
                 "lw $13,4(%0)\n\t" \
                 "ctc2 $12,$0\n\t" \
                 "ctc2 $13,$1\n\t" \
                 "lw $12,8(%0)\n\t" \
                 "lw $13,12(%0)\n\t" \
                 "lw $14,16(%0)\n\t" \
                 "ctc2 $12,$2\n\t" \
                 "ctc2 $13,$3\n\t" \
                 "ctc2 $14,$4" \
                 : : "r"(matrix) : "$12", "$13", "$14")

/* Preserve and replace the first column of the rotation matrix. */
#define gte_getrotcol0(x, y, z) \
    asm volatile("cfc2 %0,$0\n\t" \
                 "cfc2 %1,$2\n\t" \
                 "cfc2 %2,$4" \
                 : "=r"(x), "=r"(y), "=r"(z))

#define gte_setrotcol0(x, y, z) \
    asm volatile("ctc2 %0,$0\n\t" \
                 "ctc2 %1,$2\n\t" \
                 "ctc2 %2,$4" \
                 : : "r"(x), "r"(y), "r"(z))

#define gte_ldv0(vec) \
    asm volatile("lwc2 $0,0(%0)\n\t" \
                 "lwc2 $1,4(%0)" \
                 : : "r"(vec) : "memory")

#define gte_ldv1(vec) \
    asm volatile("lwc2 $2,0(%0)\n\t" \
                 "lwc2 $3,4(%0)" \
                 : : "r"(vec) : "memory")

#define gte_ldv2(vec) \
    asm volatile("lwc2 $4,0(%0)\n\t" \
                 "lwc2 $5,4(%0)" \
                 : : "r"(vec) : "memory")

/* MVMVA: rotation matrix, V0, no translation, SF=0, LM=0. */
#define gte_mvmva() \
    asm volatile("nop\n\t" \
                 "nop\n\t" \
                 ".word 0x4A486012")

/* MVMVA: light matrix, IR vector, no translation, SF=1, LM=1. */
#define gte_lightcolor() \
    asm volatile("nop\n\t" \
                 ".word 0x4A4DA412")

#define gte_stmac(out) \
    asm volatile("swc2 $25,0(%0)\n\t" \
                 "swc2 $26,4(%0)\n\t" \
                 "swc2 $27,8(%0)" \
                 : : "r"(out) : "memory")

/* LZCS/LZCR: count leading zeroes in a 32-bit value. */
#define gte_ldlzcs(value) \
    asm volatile("mtc2 %0,$30" : : "r"(value))

#define gte_stlzcr(out) \
    asm volatile("swc2 $31,0(%0)" : : "r"(out) : "memory")

/* The LZCR-to-GPR transfer needs two COP2 hazard slots. */
#define gte_getlzcr(out) \
    asm volatile("nop\n\t" \
                 "nop\n\t" \
                 "mfc2 %0,$31" : "=r"(out))

/* NCLIP: signed screen-space triangle area from SXY0, SXY1, and SXY2. */
#define gte_ldsxy0(value) \
    asm volatile("mtc2 %0,$12" : : "r"(value))

#define gte_ldsxy1(value) \
    asm volatile("mtc2 %0,$13" : : "r"(value))

#define gte_ldsxy2(value) \
    asm volatile("mtc2 %0,$14" : : "r"(value))

#define gte_nclip() \
    asm volatile("nop\n\t" \
                 "nop\n\t" \
                 ".word 0x4B400006")

#define gte_stmac0(out) \
    asm volatile("swc2 $24,0(%0)" : : "r"(out) : "memory")

/* RTV0TR: rotate V0 and add the translation vector. */
#define gte_rtv0tr() \
    asm volatile("nop\n\t" \
                 ".word 0x4A480012")

#define gte_getir1(out) \
    asm volatile("mfc2 %0,$9" : "=r"(out))

#define gte_getir2(out) \
    asm volatile("mfc2 %0,$10" : "=r"(out))

#define gte_getir3(out) \
    asm volatile("mfc2 %0,$11" : "=r"(out))

#define gte_rtv0tr_mac() \
    asm volatile("nop\n\t" \
                 "nop\n\t" \
                 ".word 0x4A480012")

#define gte_stir123(out) \
    asm volatile("swc2 $9,0(%0)\n\t" \
                 "swc2 $10,4(%0)\n\t" \
                 "swc2 $11,8(%0)" \
                 : : "r"(out) : "memory")

/* RTIR12: rotate the current IR vector with no translation. */
#define gte_ldir123(x, y, z) \
    asm volatile("mtc2 %0,$9\n\t" \
                 "mtc2 %1,$10\n\t" \
                 "mtc2 %2,$11" \
                 : : "r"(x), "r"(y), "r"(z))

/* Load the three scalar IR inputs from consecutive words. */
#define gte_ldir123v(vec) \
    asm volatile("lwc2 $9,0(%0)\n\t" \
                 "lwc2 $10,4(%0)\n\t" \
                 "lwc2 $11,8(%0)" \
                 : : "r"(vec) : "memory")

/* RGB is the colour input used by the colour interpolation commands. */
#define gte_ldrgb(rgb) \
    asm volatile("lwc2 $6,0(%0)" : : "r"(rgb) : "memory")

#define gte_strgb(out) \
    asm volatile("swc2 $22,0(%0)" : : "r"(out) : "memory")

#define gte_ldrgb0(rgb) \
    asm volatile("lwc2 $20,0(%0)" : : "r"(rgb) : "memory")

#define gte_ldrgb1(rgb) \
    asm volatile("lwc2 $21,0(%0)" : : "r"(rgb) : "memory")

#define gte_ldrgb2(rgb) \
    asm volatile("lwc2 $22,0(%0)" : : "r"(rgb) : "memory")

#define gte_strgb0(out) \
    asm volatile("swc2 $20,0(%0)" : : "r"(out) : "memory")

#define gte_strgb1(out) \
    asm volatile("swc2 $21,0(%0)" : : "r"(out) : "memory")

#define gte_strgb2(out) \
    asm volatile("swc2 $22,0(%0)" : : "r"(out) : "memory")

/* IR0 is the scalar input used by GPF. */
#define gte_ldir0(value) \
    asm volatile("mtc2 %0,$8" : : "r"(value))

#define gte_ldmac123(x, y, z) \
    asm volatile("mtc2 %0,$25\n\t" \
                 "mtc2 %1,$26\n\t" \
                 "mtc2 %2,$27" \
                 : : "r"(x), "r"(y), "r"(z))

#define gte_lddqa(value) \
    asm volatile("ctc2 %0,$27" : : "r"(value))

#define gte_lddqb(value) \
    asm volatile("ctc2 %0,$28" : : "r"(value))

/* SQR squares IR1..IR3 into MAC1..MAC3. */
#define gte_sqr() \
    asm volatile("nop\n\t" \
                 ".word 0x4AA00428")

/* SQR with the PSY-Q SF flag set. */
#define gte_sqr12() \
    asm volatile("nop\n\t" \
                 ".word 0x4AA80428")

/* AVSZ uses the four depth FIFO registers and writes OTZ. */
#define gte_ldsz3(z1, z2, z3) \
    asm volatile("mtc2 %0,$17\n\t" \
                 "mtc2 %1,$18\n\t" \
                 "mtc2 %2,$19" \
                 : : "r"(z1), "r"(z2), "r"(z3))

#define gte_ldsz4(z0, z1, z2, z3) \
    asm volatile("mtc2 %0,$16\n\t" \
                 "mtc2 %1,$17\n\t" \
                 "mtc2 %2,$18\n\t" \
                 "mtc2 %3,$19" \
                 : : "r"(z0), "r"(z1), "r"(z2), "r"(z3))

#define gte_avsz3() \
    asm volatile("nop\n\t" \
                 ".word 0x4B58002D")

#define gte_avsz4() \
    asm volatile("nop\n\t" \
                 ".word 0x4B68002E")

#define gte_getotz(out) \
    asm volatile("mfc2 %0,$7" : "=r"(out))

/* Colour interpolation commands with the current IR vector and IR0. */
#define gte_dpcl() \
    asm volatile("nop\n\t" \
                 ".word 0x4A680029")


#define gte_intpl() \
    asm volatile("nop\n\t" \
                 ".word 0x4A980011")

/* GPF multiplies IR1..IR3 by IR0 into MAC1..MAC3. */
#define gte_gpf() \
    asm volatile("nop\n\t" \
                 "nop\n\t" \
                 ".word 0x4B90003D")

#define gte_getmac123(x, y, z) \
    asm volatile("mfc2 %0,$25\n\t" \
                 "mfc2 %1,$26\n\t" \
                 "mfc2 %2,$27" \
                 : "=r"(x), "=r"(y), "=r"(z))

#define gte_rtir12() \
    asm volatile("nop\n\t" \
                 ".word 0x4A49E012")

/* RTPS: project V0 and update SXY2/SZ3. */
#define gte_rtps() \
    asm volatile("nop\n\t" \
                 "nop\n\t" \
                 ".word 0x4A180001")

#define gte_stsxy2(out) \
    asm volatile("swc2 $14,0(%0)" : : "r"(out) : "memory")

#define gte_getsz3(out) \
    asm volatile("mfc2 %0,$19" : "=r"(out))

/* RTPT: project V0, V1, and V2. */
#define gte_ldv012(vec) \
    asm volatile("lwc2 $0,0(%0)\n\t" \
                 "lwc2 $1,4(%0)\n\t" \
                 "lwc2 $2,8(%0)\n\t" \
                 "lwc2 $3,12(%0)\n\t" \
                 "lwc2 $4,16(%0)\n\t" \
                 "lwc2 $5,20(%0)" \
                 : : "r"(vec) : "memory")

#define gte_rtpt() \
    asm volatile("nop\n\t" \
                 ".word 0x4A280030")

#define gte_stsxy012(out) \
    asm volatile("swc2 $12,0(%0)\n\t" \
                 "swc2 $13,4(%0)\n\t" \
                 "swc2 $14,8(%0)" \
                 : : "r"(out) : "memory")

#define gte_stsz123(out) \
    asm volatile("swc2 $17,0(%0)\n\t" \
                 "swc2 $18,4(%0)\n\t" \
                 "swc2 $19,8(%0)" \
                 : : "r"(out) : "memory")

#endif
