#ifndef PE1_GTE_H
#define PE1_GTE_H

int rcos(int angle);

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

/* Load the three translation-vector words into TRX, TRY, and TRZ. */
#define gte_ldtransmatrix(matrix) \
    asm volatile("lw $12,20(%0)\n\t" \
                 "lw $13,24(%0)\n\t" \
                 "ctc2 $12,$5\n\t" \
                 "lw $14,28(%0)\n\t" \
                 "ctc2 $13,$6\n\t" \
                 "ctc2 $14,$7" \
                 : : "r"(matrix) : "$12", "$13", "$14")

/* Preserve and replace the first column of the rotation matrix. */
#define gte_getrotcol0(x, y, z) \
    asm volatile("cfc2 %0,$0\n\t" \
                 "cfc2 %1,$2\n\t" \
                 "cfc2 %2,$4" \
                 : "=r"(x), "=r"(y), "=r"(z))

/* OP temporarily replaces R11, R22, and R33, so preserve their COP2 values. */
#define gte_pushrotcol0() \
    asm volatile("cfc2 $13,$0\n\t" \
                 "cfc2 $14,$2\n\t" \
                 "cfc2 $15,$4" \
                 : : : "$13", "$14", "$15")

#define gte_poprotcol0() \
    asm volatile("ctc2 $13,$0\n\t" \
                 "ctc2 $14,$2\n\t" \
                 "ctc2 $15,$4" \
                 : : : "$13", "$14", "$15")

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

/* MAC stores followed by a pointer return leave v0 available to the caller. */
#define gte_stmac_return(out) \
    asm volatile("swc2 $25,0(%0)\n\t" \
                 "swc2 $26,4(%0)\n\t" \
                 "swc2 $27,8(%0)" \
                 : : "r"(out) : "$2", "memory")

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

#define gte_nclip_now() \
    asm volatile(".word 0x4B400006")

#define gte_getmac0(out) \
    asm volatile("mfc2 %0,$24" : "=r"(out))

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

/* The GPL wrappers consume v1 before transferring the final IR vector. */
#define gte_stir123_gpl(out) \
    asm volatile("swc2 $9,0(%0)\n\t" \
                 "swc2 $10,4(%0)\n\t" \
                 "swc2 $11,8(%0)" \
                 : : "r"(out) : "$3", "memory")

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

/* OP consumes IR3 before IR1 and IR2 in the original PSY-Q wrapper. */
#define gte_ldopv(vec) \
    asm volatile("lwc2 $11,8(%0)\n\t" \
                 "lwc2 $9,0(%0)\n\t" \
                 "lwc2 $10,4(%0)" \
                 : : "r"(vec) : "memory")

/* OP uses R11, R22, and R33 as its temporary diagonal matrix entries. */
#define gte_ldopv1(vec) \
    asm volatile("lw $8,0(%0)\n\t" \
                 "lw $9,4(%0)\n\t" \
                 "lw $10,8(%0)\n\t" \
                 "ctc2 $8,$0\n\t" \
                 "ctc2 $9,$2\n\t" \
                 "ctc2 $10,$4" \
                 : : "r"(vec) : "$8", "$9", "$10", "memory")

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

#define gte_declare_three_outputs()                                         \
    register void *gte_out0 asm("$8");                                      \
    register void *gte_out1 asm("$9");                                      \
    register void *gte_out2 asm("$10")

#define gte_bind_three_outputs(out0, out1, out2)                            \
    do {                                                                     \
        gte_out0 = (out0);                                                   \
        gte_out1 = (out1);                                                   \
        gte_out2 = (out2);                                                   \
    } while (0)

#define gte_dpct_separate()                                                 \
    asm volatile("nop\n\t"                                                \
                 ".word 0x4AF8002A"                                         \
                 : : : "$8", "$9", "$10")

#define gte_rtpt_three_outputs()                                            \
    asm volatile("nop\n\t"                                                \
                 ".word 0x4A280030"                                         \
                 : : : "$8", "$9", "$10")

#define gte_rtps_three_outputs()                                            \
    asm volatile("nop\n\t"                                                \
                 ".word 0x4A180001"                                         \
                 : : : "$8", "$9", "$10")

#define gte_strgb0_bound()                                                  \
    asm volatile("swc2 $20,0($8)" : : "r"(gte_out0) : "memory")
#define gte_strgb1_bound()                                                  \
    asm volatile("swc2 $21,0($9)" : : "r"(gte_out1) : "memory")
#define gte_strgb2_bound()                                                  \
    asm volatile("swc2 $22,0($10)" : : "r"(gte_out2) : "memory")
#define gte_stsxy_three_0()                                                 \
    asm volatile("swc2 $12,0($8)" : : "r"(gte_out0) : "memory")
#define gte_stsxy_three_1()                                                 \
    asm volatile("swc2 $13,0($9)" : : "r"(gte_out1) : "memory")
#define gte_stsxy_three_2()                                                 \
    asm volatile("swc2 $14,0($10)" : : "r"(gte_out2) : "memory")
#define gte_stsxy2_three_0()                                                \
    asm volatile("swc2 $14,0($8)" : : "r"(gte_out0) : "memory")
#define gte_stir0_three_1()                                                 \
    asm volatile("swc2 $8,0($9)" : : "r"(gte_out1) : "memory")

#define gte_getflag(out) asm volatile("cfc2 %0,$31" : "=r"(out))

#define gte_declare_rt4_results(first_flags, flags, depth)                   \
    register int first_flags asm("$3");                                     \
    register int flags asm("$8");                                           \
    register int depth asm("$2")

#define gte_store_third_output(value)                                       \
    asm volatile("sw %0,0($10)" : : "r"(value), "r"(gte_out2) : "memory")

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

#define gte_getflag(out) \
    asm volatile("cfc2 %0,$31" : "=r"(out))

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

#define gte_gpf0() \
    asm volatile("nop\n\t" \
                 ".word 0x4B90003D")

#define gte_gpf12() \
    asm volatile("nop\n\t" \
                 ".word 0x4B98003D")

#define gte_gpl0() \
    asm volatile("nop\n\t" \
                 ".word 0x4BA0003E")

#define gte_gpl12() \
    asm volatile("nop\n\t" \
                 ".word 0x4BA8003E")

/* Fixed-register staging used by the handwritten PSY-Q transfer wrappers. */
#define gte_declare_xy_staging()                                             \
    register int gte_x asm("$8");                                            \
    register int gte_y asm("$9")

#define gte_declare_xyz_staging()                                            \
    gte_declare_xy_staging();                                                \
    register int gte_z asm("$10")

#define gte_declare_shift12() register int gte_shift asm("$11")
#define gte_set_shift12() (gte_shift = 12)

#define gte_load_byte2(input)                                                \
    do {                                                                     \
        gte_x = ((unsigned char *)(input))[0];                               \
        gte_y = ((unsigned char *)(input))[1];                               \
    } while (0)

#define gte_load_byte3(input)                                                \
    do {                                                                     \
        gte_load_byte2(input);                                               \
        gte_z = ((unsigned char *)(input))[2];                               \
    } while (0)

#define gte_load_packed_short3(input)                                        \
    asm volatile("lw $8,0(%3)\n\t"                                         \
                 "lw $10,4(%3)\n\t"                                        \
                 "sra $9,$8,16\n\t"                                        \
                 "andi $8,$8,0xFFFF\n\t"                                   \
                 "andi $10,$10,0xFFFF"                                       \
                 : "=r"(gte_x), "=r"(gte_y), "=r"(gte_z)                    \
                 : "r"(input) : "memory")

#define gte_ldir0_ir12(scale)                                                \
    asm volatile("mtc2 %0,$8\n\t"                                          \
                 "mtc2 $8,$9\n\t"                                          \
                 "mtc2 $9,$10"                                               \
                 : : "r"(scale), "r"(gte_x), "r"(gte_y))

#define gte_ldir0_ir123(scale)                                               \
    asm volatile("mtc2 %0,$8\n\t"                                          \
                 "mtc2 $8,$9\n\t"                                          \
                 "mtc2 $9,$10\n\t"                                         \
                 "mtc2 $10,$11"                                              \
                 : : "r"(scale), "r"(gte_x), "r"(gte_y), "r"(gte_z))

/* GPL without an inserted hazard slot, for wrappers which already provide it. */
#define gte_gpl0_now() asm volatile(".word 0x4BA0003E")
#define gte_gpl12_now() asm volatile(".word 0x4BA8003E")

#define gte_store_mac12_byte2(output)                                        \
    do {                                                                     \
        register void *gte_out asm("$13");                                   \
        gte_out = (output);                                                  \
        asm volatile("mfc2 $8,$25\n\t"                                     \
                     "mfc2 $9,$26\n\t"                                     \
                     "srav $8,$8,$11\n\t"                                  \
                     "srav $9,$9,$11\n\t"                                  \
                     "sb $8,0($13)\n\t"                                    \
                     "sb $9,1($13)"                                          \
                     : "=r"(gte_x), "=r"(gte_y)                             \
                     : "r"(gte_out), "r"(gte_shift) : "memory");            \
    } while (0)

#define gte_store_mac123_byte3(output)                                       \
    do {                                                                     \
        register void *gte_out asm("$13");                                   \
        gte_out = (output);                                                  \
        asm volatile("mfc2 $8,$25\n\t"                                     \
                     "mfc2 $9,$26\n\t"                                     \
                     "mfc2 $10,$27\n\t"                                    \
                     "srav $8,$8,$11\n\t"                                  \
                     "srav $9,$9,$11\n\t"                                  \
                     "srav $10,$10,$11\n\t"                                \
                     "sb $8,0($13)\n\t"                                    \
                     "sb $9,1($13)\n\t"                                    \
                     "sb $10,2($13)"                                         \
                     : "=r"(gte_x), "=r"(gte_y), "=r"(gte_z)                \
                     : "r"(gte_out), "r"(gte_shift) : "memory");            \
    } while (0)

#define gte_store_ir123_packed_short3(output)                                \
    do {                                                                     \
        register void *gte_out asm("$13");                                   \
        asm volatile("mfc2 $8,$9\n\t"                                      \
                     "mfc2 $9,$10\n\t"                                     \
                     "andi $8,$8,0xFFFF\n\t"                               \
                     "sll $9,$9,16\n\t"                                    \
                     "or $8,$8,$9"                                           \
                     : "=r"(gte_x), "=r"(gte_y));                           \
        gte_out = (output);                                                  \
        asm volatile("mfc2 $10,$11\n\t"                                    \
                     "sw $8,0($13)\n\t"                                    \
                     "sw $10,4($13)"                                         \
                     : "=r"(gte_z) : "r"(gte_x), "r"(gte_out) : "memory"); \
    } while (0)

#define gte_op0() \
    asm volatile("nop\n\t" \
                 ".word 0x4B70000C")

#define gte_op12() \
    asm volatile("nop\n\t" \
                 ".word 0x4B78000C")

#define gte_getlzcr_now(out) \
    asm volatile("mfc2 %0,$31" : "=r"(out))

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

#define gte_declare_separate_outputs()                                      \
    register void *gte_out1 asm("$8");                                      \
    register void *gte_out2 asm("$9");                                      \
    register void *gte_depth_out asm("$10");                                \
    register void *gte_flags_out asm("$11");                                \
    register int gte_flags asm("$3");                                       \
    register int gte_depth_value asm("$2")

/* Keep the ABI-bound output registers free until RTPT has completed. */
#define gte_rtpt_separate()                                                 \
    asm volatile("nop\n\t"                                                \
                 ".word 0x4A280030"                                         \
                 : : : "$8", "$9", "$10", "$11")

#define gte_bind_separate_outputs(out1, out2, depth, flags)                  \
    do {                                                                     \
        gte_out1 = (out1);                                                   \
        gte_out2 = (out2);                                                   \
        gte_depth_out = (depth);                                             \
        gte_flags_out = (flags);                                             \
    } while (0)

#define gte_stsxy0(out)                                                     \
    asm volatile("swc2 $12,0(%0)" : : "r"(out) : "memory")
#define gte_stsxy1(out)                                                     \
    asm volatile("swc2 $13,0(%0)" : : "r"(out) : "memory")
#define gte_stsxy1_bound()                                                  \
    asm volatile("swc2 $13,0($8)" : : "r"(gte_out1) : "memory")
#define gte_stsxy2_bound()                                                  \
    asm volatile("swc2 $14,0($9)" : : "r"(gte_out2) : "memory")
#define gte_stir0(out)                                                      \
    asm volatile("swc2 $8,0(%0)" : : "r"(out) : "memory")
#define gte_stir0_bound()                                                   \
    asm volatile("swc2 $8,0($10)" : : "r"(gte_depth_out) : "memory")
#define gte_getflag_bound()                                                 \
    asm volatile("cfc2 $3,$31" : "=r"(gte_flags))
#define gte_getsz3_bound()                                                  \
    asm volatile("mfc2 $2,$19" : "=r"(gte_depth_value))
#define gte_store_flag_bound()                                              \
    asm volatile("sw $3,0($11)" : : "r"(gte_flags), "r"(gte_flags_out)     \
                 : "memory")

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
