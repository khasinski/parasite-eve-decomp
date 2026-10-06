/* ASSEMBLER: GNU */
/* PSY-Q LIBMATH DIVDF3: __divdf3 and _comp_mant. */
#include "pe1/math64.h"
double __divdf3(double numerator, double denominator) {
    struct {
        MathU64 quotient, bit, round, n, d, temp;
    } w;
    register MathDoubleBits left, right;
    register MathDoubleBits result;
    register int exponent;
    register unsigned sign;
    register int shift asm("$16");
    register MathU64 *savedN;
    register unsigned absMask;
    register MathU64 *qp asm("$4");
    left.value = numerator;
    right.value = denominator;
    asm("" : : "r"(left), "r"(right));
    {
        register int e = (((int)left.bits.hi >> 20) & 0x7ff);
        e -= (((int)right.bits.hi >> 20) & 0x7ff);
        exponent = e + 1022;
    }
    absMask = 0x7fffffff;
    {
        unsigned mask = 0x80000000;
        register unsigned lsign = left.bits.hi & mask;
        register unsigned rsign = right.bits.hi & mask;
        sign = lsign ^ rsign;
    }
    if (!(right.bits.hi & absMask) && !right.bits.lo) {
        {
            register unsigned mask asm("$2") = 0x7fff0000;
            asm("" : : "r"(mask));
        }
        result.bits.hi = 0xffffffff;
        if (!sign)
            result.bits.hi = 0x7fffffff;
        result.bits.lo = 0xffffffff;
    } else if (!(left.bits.hi & 0x7fffffff) && !left.bits.lo) {
        result.bits.hi = sign;
        result.bits.lo = 0;
    } else {
        w.n.hi = (left.bits.hi & 0xfffff) | 0x100000;
        w.n.lo = left.bits.lo;
        w.d.hi = (right.bits.hi & 0xfffff) | 0x100000;
        w.d.lo = right.bits.lo;
        if (_comp_mant(w.n, w.d) < 0) {
            _dbl_shift(&w.n, 0, w.n, 1);
            exponent--;
        }
        savedN = &w.n;
        shift = 1;
        w.bit.hi = 0x200000;
        w.bit.lo = 0;
        w.quotient.lo = 0;
        w.quotient.hi = 0;
        do {
            if (_comp_mant(w.n, w.d) >= 0) {
                w.quotient.hi |= w.bit.hi;
                w.quotient.lo |= w.bit.lo;
                _mainasu(&w.temp, w.d);
                _add_mant_d(savedN, w.n, w.temp);
            }
            _dbl_shift(&w.n, 0, w.n, shift);
            _dbl_shift_us(&w.bit, 1, w.bit, shift);
        } while (w.bit.hi || w.bit.lo);
        qp = &w.quotient;
        if (exponent >= 0) {
            shift = 1;
            w.round.hi = 0;
            w.round.lo = 1;
            _add_mant_d(qp, w.quotient, w.round);
            exponent++;
            qp = &w.quotient;
        } else {
            shift = -exponent;
            w.round.hi = 0;
            w.round.lo = 1U << shift;
            _add_mant_d(qp, w.quotient, w.round);
            exponent = 0;
            qp = &w.quotient;
            shift++;
        }
        _dbl_shift_us(qp, 1, w.quotient, shift);
        w.quotient.hi &= 0xffefffff;
        if (exponent >= 2047) {
            _err_math(34, 15);
            result.bits.hi = sign ? 0xfff00000 : 0x7ff00000;
            result.bits.lo = 0;
        } else {
            {
                register unsigned packed = sign | ((unsigned)exponent << 20);
                result.bits.hi = packed | w.quotient.hi;
            }
            result.bits.lo = w.quotient.lo;
        }
    }
    return result.value;
}



int _comp_mant(MathU64 left, MathU64 right)
{
    if (left.hi > right.hi) goto greater;
    if (left.hi < right.hi) return -1;
    if (left.lo > right.lo) return 1;
    {
        register int less asm("$3") = left.lo < right.lo;
        if (less) return -1;
    }
    asm("");
    return 0;
greater:
    {
        register int result asm("$2") = 1;
        asm("" : : "r"(result));
        return result;
    }
}
