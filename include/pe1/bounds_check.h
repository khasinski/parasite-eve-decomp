#ifndef PE1_BOUNDS_CHECK_H
#define PE1_BOUNDS_CHECK_H

/* Retail leaves this diagnostic hook empty. Callers pass a failure code
 * and sometimes one or two addresses. Its original parameter list cannot
 * be recovered from the empty body; keep the C89 unspecified argument list. */
void BoundsCheck_AssertStub();

#endif
