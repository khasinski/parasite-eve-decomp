#ifndef PE1_STAT_MODIFIERS_H
#define PE1_STAT_MODIFIERS_H

/* Base scaling entry followed by the per-category stat modifiers. */
extern int g_StatScaleBase[];
int *Battle_GetModifierTable(void);

#endif
