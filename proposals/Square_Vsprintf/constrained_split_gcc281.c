/* Unmodified stock GCC 2.8.1 and MASPSX; experimental, not an exact match. */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -fno-strength-reduce -fno-force-mem */
/* Experimental scheduling constraints; not an exact match. See README.md. */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

/* The 12-byte template at D_80094528 is all zero.  Only the low-byte
 * flag field and the following prefix byte are used by this formatter. */
typedef union {
    u32 flags;
    struct {
        u8 flags;
        s8 prefix;
        u16 reserved;
    } bytes;
} FormatHeader;

typedef struct {
    FormatHeader header;
    s32 width;
    s32 precision;
} FormatSpec;

typedef struct {
    u8 number[0x200];
    FormatSpec spec;
} FormatWork;

extern FormatSpec D_80094528;
extern u8 D_8001161C[];
extern u8 D_80011630[];

extern s32 strlen(u8 *);
extern u8 *memchr(u8 *, s32, s32);
extern void *memmove(void *, const void *, int);

s32 Square_Vsprintf(char *dest, s8 *format, ...)
{
    FormatWork work;
    u8 *argState[2];
#define args argState[0]
    u8 *src;
    u8 *end;
    register u8 *digits asm("$7");
    u32 value;
    s32 length;
    s32 count;
    s32 c;
    s32 hash;
    s32 zero;
    s32 space;
    s32 plus;
    s32 minus;
    s8 *flagBase;
    s8 *precisionBase;

    args = (u8 *)&format + 4;
    c = *format;
    length = 0;
    if (c == 0) {
        goto finished;
    }
    minus = '-';
    plus = '+';
    space = ' ';
    zero = '0';

    do {
        if (c != '%') {
            dest[length++] = c;
            continue;
        }

        {
            register FormatSpec *initial asm("$5") = &D_80094528;
            asm("" : "+r"(initial));
            {
            u32 flags = initial->header.flags;
            s32 width = initial->width;
            s32 precision = initial->precision;

            work.spec.header.flags = flags;
            work.spec.width = width;
            work.spec.precision = precision;
            }
        }
        /* Keep the template stores before initializing the flag parser. */
        asm volatile("" : : "m"(work.spec));
        hash = '#';

        for (;;) {
            /* Keep the base for the optional width argument below. */
            flagBase = format;
            format = flagBase + 1;

            c = flagBase[1];
            if (c == minus) {
                work.spec.header.flags |= 1;
                continue;
            }
            if (c == plus) {
                work.spec.header.flags |= 2;
                continue;
            }
            if (c == space) {
                work.spec.header.bytes.prefix = c;
                continue;
            }
            if (c == hash) {
                work.spec.header.flags |= 4;
                continue;
            }
            if (c == zero) {
                work.spec.header.flags |= 8;
                continue;
            }
            break;
        }

        if (c == '*') {
            s32 w = *(s32 *)args;
            args += 4;
            work.spec.width = w;
            if (w < 0) {
                u32 nf = work.spec.header.flags;
                work.spec.width = -w;
                work.spec.header.flags = nf | 1;
            }
            format = flagBase + 2;
            c = flagBase[2];
        } else {
            while ((u32)(c - '0') < 10) {
                work.spec.width = work.spec.width * 10 - ('0' - c);
                c = *++format;
            }
        }

        if (c == '.') {
            /* The star arm advances by two from this same base. */
            precisionBase = format;
            format = precisionBase + 1;

            c = precisionBase[1];
            if (c == '*') {
                work.spec.precision = *(s32 *)args;
                args += 4;
                format = precisionBase + 2;
                c = precisionBase[2];
            } else {
                while ((u32)(c - '0') < 10) {
                    work.spec.precision = work.spec.precision * 10 - ('0' - c);
                    c = *++format;
                }
            }
            if (work.spec.precision >= 0) {
                work.spec.header.flags |= 0x10;
            }
        }

        src = (u8 *)&work.spec;
        if (work.spec.header.flags & 1) {
            work.spec.header.flags &= ~8;
        }

conversion:
        switch (c) {
        case 'h':
            work.spec.header.flags |= 0x20;
            c = *++format;
            goto conversion;

        case 'l':
            work.spec.header.flags |= 0x40;
            c = *++format;
            goto conversion;

        case 'L':
            work.spec.header.flags |= 0x80;
            c = *++format;
            goto conversion;

        case 'd':
        case 'i':
            value = *(s32 *)args;
            args += 4;
            asm volatile("" : : "m"(args));
            if ((work.spec.header.flags >> 5) & 1) {
                value = (s16)value;
            }
            if ((s32)value < 0) {
                value = -value;
                work.spec.header.bytes.prefix = minus;
            } else if ((work.spec.header.flags >> 1) & 1) {
                work.spec.header.bytes.prefix = plus;
            }
            goto decimal;

        case 'u':
            value = *(u32 *)args;
            args += 4;
            asm volatile("" : : "m"(args));
            if ((work.spec.header.flags >> 5) & 1) {
                value &= 0xFFFF;
            }
            work.spec.header.bytes.prefix = 0;
decimal:
            if (!((work.spec.header.flags >> 4) & 1)) {
                if ((work.spec.header.flags >> 3) & 1) {
                    work.spec.precision = work.spec.width;
                    if (work.spec.header.bytes.prefix != 0) {
                        work.spec.precision = work.spec.width - 1;
                    }
                }
                if (work.spec.precision <= 0) {
                    work.spec.precision = 1;
                }
            }
            count = 0;
            while (value != 0) {
                u32 q = value / 10;
                *--src = value - q * 10 + '0';
                value = q;
                count++;
            }
            do {
                while (count < work.spec.precision) {
                    *--src = zero;
                    count++;
                }
            } while (0);
            {
                s32 psgn = work.spec.header.bytes.prefix;
                s32 pch = *(u8 *)&work.spec.header.bytes.prefix;
                if (psgn != 0) {
                    *--src = pch;
                    count++;
                }
            }
            break;

        case 'o':
            value = *(u32 *)args;
            args += 4;
            asm volatile("" : : "m"(args));
            if ((work.spec.header.flags >> 5) & 1) {
                value &= 0xFFFF;
            }
            if (!((work.spec.header.flags >> 4) & 1)) {
                if ((work.spec.header.flags >> 3) & 1) {
                    work.spec.precision = work.spec.width;
                }
                if (work.spec.precision <= 0) {
                    work.spec.precision = 1;
                }
            }
            count = 0;
            while (value != 0) {
                *--src = (value % 8) + '0';
                value >>= 3;
                count++;
            }
            if (((work.spec.header.flags >> 2) & 1) && count != 0 && *(s8 *)src != zero) {
                *--src = zero;
                count++;
            }
            do {
                while (count < work.spec.precision) {
                    *--src = zero;
                    count++;
                }
            } while (0);
            break;

        case 'p':
            {
                u32 pf = work.spec.header.flags;
                work.spec.precision = 8;
                work.spec.header.flags = pf | 0x50;
            }

        case 'X':
            digits = D_8001161C;
            goto hexadecimal;

        case 'x':
            digits = D_80011630;
hexadecimal:
            value = *(u32 *)args;
            args += 4;
            asm volatile("" : : "m"(args));
            if ((work.spec.header.flags >> 5) & 1) {
                value &= 0xFFFF;
            }
            if (!((work.spec.header.flags >> 4) & 1)) {
                if ((work.spec.header.flags >> 3) & 1) {
                    work.spec.precision = work.spec.width;
                    if ((work.spec.header.flags >> 2) & 1) {
                        work.spec.precision = work.spec.width - 2;
                    }
                }
                if (work.spec.precision <= 0) {
                    work.spec.precision = 1;
                }
            }
            count = 0;
            while (value != 0) {
                *--src = digits[value % 16];
                value >>= 4;
                count++;
            }
            do {
                while (count < work.spec.precision) {
                    *--src = zero;
                    count++;
                }
            } while (0);
            if ((work.spec.header.flags >> 2) & 1) {
                --src;
                *src = c;
                /* Keep the two prefix-byte pointer updates separate. */
                asm volatile("" : "+r"(src));
                --src;
                *src = zero;
                count += 2;
            }
            break;

        case 'c':
            src--;
            {
                u8 *p = args;
                s32 ch = *(u8 *)p;
                count = 1;
                *src = ch;
                args = p + 4;
            }
            break;

        case 's':
            src = *(u8 **)args;
            args += 4;
            if ((work.spec.header.flags >> 2) & 1) {
                count = *src++;
                if (((work.spec.header.flags >> 4) & 1) && work.spec.precision < count) {
                    count = work.spec.precision;
                }
            } else if (!((work.spec.header.flags >> 4) & 1)) {
                count = strlen(src);
            } else {
                end = memchr(src, 0, work.spec.precision);
                count = end - src;
                if (end == 0) {
                    count = work.spec.precision;
                }
            }
            break;

        case 'n': {
            register u32 isShort asm("$2");
            register u32 nf asm("$3") = work.spec.header.flags;
            src = *(u8 **)args;
            args += 4;
            asm volatile("" : : "r"(nf));
            isShort = (nf >> 5) & 1;
            if (isShort) {
                *(s16 *)src = length;
            } else {
                *(s32 *)src = length;
            }
            continue;
        }

        default:
            if (c != '%') {
                goto finished;
            }
            dest[length++] = c;
            continue;
        }

        if (count < work.spec.width && !(work.spec.header.flags & 1)) {
            while (count < work.spec.width) {
                dest[length++] = space;
                work.spec.width--;
            }
        }
        memmove(dest + length, src, count);
        length += count;
        while (count < work.spec.width) {
            dest[length++] = space;
            count++;
        }
    } while ((c = *++format) != 0);

finished:
    dest[length] = 0;
    return length;
#undef args
}
