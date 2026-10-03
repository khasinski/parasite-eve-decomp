#ifndef PE1_AKAO_PAN3D_H
#define PE1_AKAO_PAN3D_H

#include "common.h"
#include "pe1/akao/pos.h"

/* Projection distance (GTE H) shared with the field renderer. */
extern int *D_800BCFA8;

long RotTransPers(AkaoPackedRect3 *v, long *sxy, long *p, long *flag);

#endif /* PE1_AKAO_PAN3D_H */
