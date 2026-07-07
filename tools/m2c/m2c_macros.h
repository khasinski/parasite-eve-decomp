#ifndef M2C_MACROS_H
#define M2C_MACROS_H

typedef int M2C_UNK;

#define M2C_FIELD(base, type, offset) (*(type)((char *)(base) + (offset)))

#endif
