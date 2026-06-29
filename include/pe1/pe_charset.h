#ifndef PE1_PE_CHARSET_H
#define PE1_PE_CHARSET_H

/* PE1 in-game text character set (glyph codes) and helpers.
 *
 * Recovered by writing every code to a live dialogue box and reading the
 * rendered glyphs (DuckStation GDB). Strings are arrays of these codes ending
 * in 0xFF. See tools/scripts/pe1_text.py for an encode/decode CLI + codegen.
 *
 *   0x00..0x09  '0'..'9'        0x0A..0x0E  + - = * %      0x0F  space
 *   0x10..0x29  'A'..'Z'        0x2A..0x2F  & ! ? " ' ,
 *   0x30..0x49  'a'..'z'        0x4A..0x4D  . : ; /
 *   0xFF        terminator      (>=0x50: control / extended glyphs:
 *                                0xF7 colour, 0xF8/0xF9, 0xFB escape, 0xFE<arg>)
 *
 * NOTE: a pure C preprocessor cannot turn a string literal "Hello!" into glyph
 * bytes (no compile-time iteration over a literal; PSYQ GCC 2.7.2 has no
 * constexpr). Use one of:
 *   - PE_STR(...) with the per-glyph PE_* tokens below (compile-time arrays);
 *   - pe_encode() at runtime;
 *   - tools/scripts/pe1_text.py encode "..." to paste a byte array.
 */

#define PE_TERM  0xFF
#define PE_SP    0x0F   /* space */

/* digits */
#define PE_0 0x00
#define PE_1 0x01
#define PE_2 0x02
#define PE_3 0x03
#define PE_4 0x04
#define PE_5 0x05
#define PE_6 0x06
#define PE_7 0x07
#define PE_8 0x08
#define PE_9 0x09
/* symbols */
#define PE_PLUS  0x0A
#define PE_MINUS 0x0B
#define PE_EQ    0x0C
#define PE_STAR  0x0D
#define PE_PCT   0x0E
#define PE_AMP   0x2A
#define PE_EXCL  0x2B   /* ! */
#define PE_QUES  0x2C   /* ? */
#define PE_DQUOT 0x2D   /* " */
#define PE_APOS  0x2E   /* ' */
#define PE_COMMA 0x2F
#define PE_DOT   0x4A
#define PE_COLON 0x4B
#define PE_SEMI  0x4C
#define PE_SLASH 0x4D

/* letters: PE_A..PE_Z = 0x10..0x29, PE_a..PE_z = 0x30..0x49 */
#define PE_A 0x10
#define PE_B 0x11
#define PE_C 0x12
#define PE_D 0x13
#define PE_E 0x14
#define PE_F 0x15
#define PE_G 0x16
#define PE_H 0x17
#define PE_I 0x18
#define PE_J 0x19
#define PE_K 0x1A
#define PE_L 0x1B
#define PE_M 0x1C
#define PE_N 0x1D
#define PE_O 0x1E
#define PE_P 0x1F
#define PE_Q 0x20
#define PE_R 0x21
#define PE_S 0x22
#define PE_T 0x23
#define PE_U 0x24
#define PE_V 0x25
#define PE_W 0x26
#define PE_X 0x27
#define PE_Y 0x28
#define PE_Z 0x29
#define PE_a 0x30
#define PE_b 0x31
#define PE_c 0x32
#define PE_d 0x33
#define PE_e 0x34
#define PE_f 0x35
#define PE_g 0x36
#define PE_h 0x37
#define PE_i 0x38
#define PE_j 0x39
#define PE_k 0x3A
#define PE_l 0x3B
#define PE_m 0x3C
#define PE_n 0x3D
#define PE_o 0x3E
#define PE_p 0x3F
#define PE_q 0x40
#define PE_r 0x41
#define PE_s 0x42
#define PE_t 0x43
#define PE_u 0x44
#define PE_v 0x45
#define PE_w 0x46
#define PE_x 0x47
#define PE_y 0x48
#define PE_z 0x49

/* Compile-time string array (pure preprocessor):
 *   static const unsigned char s[] = PE_STR(PE_H,PE_e,PE_l,PE_l,PE_o,PE_EXCL);
 * expands to { 0x17, 0x34, 0x3B, 0x3B, 0x3E, 0x2B, 0xFF }. */
#define PE_STR(...) { __VA_ARGS__, PE_TERM }

/* Runtime ASCII -> glyph-code encoder. Writes codes for `s` plus a 0xFF
 * terminator into `dst`; returns the number of bytes written (incl. terminator).
 * Unmapped characters are skipped. */
static int pe_encode(unsigned char *dst, const char *s)
{
    unsigned char *p = dst;
    unsigned char ch;

    while ((ch = (unsigned char)*s++) != 0) {
        if (ch >= '0' && ch <= '9')      *p++ = (unsigned char)(ch - '0');
        else if (ch >= 'A' && ch <= 'Z') *p++ = (unsigned char)(0x10 + (ch - 'A'));
        else if (ch >= 'a' && ch <= 'z') *p++ = (unsigned char)(0x30 + (ch - 'a'));
        else if (ch == ' ')              *p++ = PE_SP;
        else {
            switch (ch) {
            case '+': *p++ = PE_PLUS;  break;
            case '-': *p++ = PE_MINUS; break;
            case '=': *p++ = PE_EQ;    break;
            case '*': *p++ = PE_STAR;  break;
            case '%': *p++ = PE_PCT;   break;
            case '&': *p++ = PE_AMP;   break;
            case '!': *p++ = PE_EXCL;  break;
            case '?': *p++ = PE_QUES;  break;
            case '"': *p++ = PE_DQUOT; break;
            case '\'':*p++ = PE_APOS;  break;
            case ',': *p++ = PE_COMMA; break;
            case '.': *p++ = PE_DOT;   break;
            case ':': *p++ = PE_COLON; break;
            case ';': *p++ = PE_SEMI;  break;
            case '/': *p++ = PE_SLASH; break;
            default: /* unmapped (high/extended glyph) -> skip */ break;
            }
        }
    }
    *p++ = PE_TERM;
    return (int)(p - dst);
}

/* glyph code -> ASCII (0 if not a printable code). */
static char pe_code_to_ascii(unsigned char c)
{
    if (c <= 0x09) return (char)('0' + c);
    if (c >= 0x10 && c <= 0x29) return (char)('A' + (c - 0x10));
    if (c >= 0x30 && c <= 0x49) return (char)('a' + (c - 0x30));
    if (c == 0x0F) return ' ';
    switch (c) {
    case 0x0A: return '+'; case 0x0B: return '-'; case 0x0C: return '=';
    case 0x0D: return '*'; case 0x0E: return '%';
    case 0x2A: return '&'; case 0x2B: return '!'; case 0x2C: return '?';
    case 0x2D: return '"'; case 0x2E: return '\''; case 0x2F: return ',';
    case 0x4A: return '.'; case 0x4B: return ':'; case 0x4C: return ';';
    case 0x4D: return '/';
    default: return 0;
    }
}

#endif /* PE1_PE_CHARSET_H */
