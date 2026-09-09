/* Host algorithm tests use native pointers. PSX ABI layout is checked by the
 * cross build; do not apply its pointer-sized layout assertions on a 64-bit host. */
#include <stddef.h>
#include "common.h"
#undef PE1_OFFSETOF
#define PE1_OFFSETOF(type, member) offsetof(type, member)
#undef PE1_STATIC_ASSERT
#define PE1_STATIC_ASSERT(expr, name) \
    _Static_assert(sizeof(void *) != 4 || (expr), #name)
