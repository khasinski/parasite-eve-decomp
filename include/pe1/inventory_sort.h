#ifndef PE1_INVENTORY_SORT_H
#define PE1_INVENTORY_SORT_H

#include "pe1/inventory.h"

extern int D_8009D0A0;
extern ItemDataRecord *(*D_8009D0B4)(int index);
extern unsigned char *D_8009D0B8;
extern unsigned char *D_8009D0BC;

#define g_InvSortStatIndex D_8009D0A0
#define g_InvSortLookup D_8009D0B4

int Inv_CompareItemsByStatForSort(const short *leftIndex,
                                  const short *rightIndex);
int Inv_CompareItemsForSort(const short *leftIndex, const short *rightIndex);

#endif
