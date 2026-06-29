#!/usr/bin/env python3
"""PE1 in-game text codec (encode/decode) with the full printable charmap.

The charmap was recovered by writing every code to a live dialogue box on a real
BIOS (DuckStation GDB) and reading the rendered glyphs. It supersedes the older
"ASCII = byte + 0x31" approximation (which was right for letters by coincidence
but wrong for digits and punctuation).

Printable map (code -> glyph):
    0x00..0x09  '0'..'9'
    0x0A..0x0E  '+' '-' '=' '*' '%'
    0x0F        ' ' (space)
    0x10..0x29  'A'..'Z'
    0x2A..0x2F  '&' '!' '?' '"' '\'' ','
    0x30..0x49  'a'..'z'
    0x4A..0x4D  '.' ':' ';' '/'
    0xFF        end of string (terminator)
    other high bytes (>=0x50): control / extended glyphs
        seen: 0xF7 colour/switch, 0xF8/0xF9, 0xFB escape, 0xFE <arg> page/cond

Usage:
    pe1_text.py encode "Hello CDA!"      # -> hex bytes + C array
    pe1_text.py decode 17343b3b3e0f...   # hex -> text
"""
from __future__ import annotations
import sys

# code -> glyph
_CODE_TO_CH: dict[int, str] = {}
for _i in range(10):
    _CODE_TO_CH[_i] = chr(ord("0") + _i)            # 0x00..0x09
for _i, _c in enumerate("+-=*%"):
    _CODE_TO_CH[0x0A + _i] = _c                      # 0x0A..0x0E
_CODE_TO_CH[0x0F] = " "
for _i in range(26):
    _CODE_TO_CH[0x10 + _i] = chr(ord("A") + _i)      # 0x10..0x29
for _i, _c in enumerate("&!?\"',"):
    _CODE_TO_CH[0x2A + _i] = _c                       # 0x2A..0x2F
for _i in range(26):
    _CODE_TO_CH[0x30 + _i] = chr(ord("a") + _i)      # 0x30..0x49
for _i, _c in enumerate(".:;/"):
    _CODE_TO_CH[0x4A + _i] = _c                       # 0x4A..0x4D

CH_TO_CODE: dict[str, int] = {v: k for k, v in _CODE_TO_CH.items()}
TERMINATOR = 0xFF


def encode(text: str) -> bytes:
    out = bytearray()
    for ch in text:
        if ch not in CH_TO_CODE:
            raise ValueError(f"char {ch!r} not in PE1 printable charmap (high/extended glyph)")
        out.append(CH_TO_CODE[ch])
    out.append(TERMINATOR)
    return bytes(out)


def decode(raw: bytes) -> str:
    out = []
    for b in raw:
        if b == TERMINATOR:
            break
        out.append(_CODE_TO_CH.get(b, f"<{b:02X}>"))
    return "".join(out)


def main() -> int:
    if len(sys.argv) < 3:
        sys.stderr.write(__doc__)
        return 2
    mode, arg = sys.argv[1], sys.argv[2]
    if mode == "encode":
        b = encode(arg)
        print(b.hex())
        print("{" + ", ".join(f"0x{x:02X}" for x in b) + "}")
    elif mode == "decode":
        print(decode(bytes.fromhex(arg)))
    else:
        sys.stderr.write("mode must be encode|decode\n")
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
