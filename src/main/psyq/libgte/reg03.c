#include "pe1/gte.h"

void SetVertex0(void *v) {
    gte_ldv0(v);
}

void SetVertex1(void *v) {
    gte_ldv1(v);
}

void SetVertex2(void *v) {
    gte_ldv2(v);
}

void SetVertexTri(void *v0, void *v1, void *v2) {
    gte_ldv0(v0);
    gte_ldv1(v1);
    gte_ldv2(v2);
}

void SetRGBfifo(void *c0, void *c1, void *c2) {
    gte_ldrgb0(c0);
    gte_ldrgb1(c1);
    gte_ldrgb2(c2);
}

void SetIR123(int ir1, int ir2, int ir3) {
    gte_ldir123(ir1, ir2, ir3);
}

void SetIR0(int ir0) {
    gte_ldir0(ir0);
}

void SetSZfifo3(int sz1, int sz2, int sz3) {
    gte_ldsz3(sz1, sz2, sz3);
}

void SetSZfifo4(int sz0, int sz1, int sz2, int sz3) {
    gte_ldsz4(sz0, sz1, sz2, sz3);
}

void SetSXSYfifo(int sxy0, int sxy1, int sxy2) {
    gte_ldsxy0(sxy0);
    gte_ldsxy1(sxy1);
    gte_ldsxy2(sxy2);
}

void SetRii(int r11r12, int r22r23, int r33) {
    gte_setrotcol0(r11r12, r22r23, r33);
}

void SetMAC123(int mac1, int mac2, int mac3) {
    gte_ldmac123(mac1, mac2, mac3);
}

void SetData32(int data) {
    gte_ldlzcs(data);
}

void SetDQA(int dqa) {
    gte_lddqa(dqa);
}

void SetDQB(int dqb) {
    gte_lddqb(dqb);
}
