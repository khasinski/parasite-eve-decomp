#!/usr/bin/env python3
"""Compare this formatter's ASPSX/psyk dumps with a stock MASPSX ELF object.

Diagnostic only: supports the relocation forms present in Square_Vsprintf,
not arbitrary PSY-Q objects. Generate dumps using `psyk list -d file.obj`.
"""
import argparse
from pathlib import Path
import re, struct, runpy, subprocess, tempfile
from elftools.elf.elffile import ELFFile

externs={'D_80094528':0x80094528,'D_8001161C':0x8001161C,'D_80011630':0x80011630,'strlen':0x80072314,'memchr':0x80072324,'memmove':0x80072334}

def decode(path, bases):
    lines=Path(path).read_text().splitlines()
    symbols={int(m[1],16):externs[m[2]] for line in lines if (m:=re.fullmatch(r"14 : XREF symbol number ([0-9a-f]+) '([^']+)'",line))}
    for line in lines:
        if m:=re.fullmatch(r"12 : XDEF symbol number ([0-9a-f]+) '([^']+)' at offset ([0-9a-f]+) in section (\d+)", line):
            symbols[int(m[1],16)] = bases[int(m[4])] + int(m[3],16)
    data={1:bytearray(),2:bytearray(),3:bytearray()}; patches=[]; section=None; remaining=0; fragment=0
    for line in lines:
        if m:=re.fullmatch(r'6 : Switch to section (\d+)',line):
            assert remaining==0; section=int(m[1])
        elif m:=re.fullmatch(r'2 : Code (\d+) bytes',line):
            assert remaining==0; fragment=len(data[section]); remaining=int(m[1])
        elif m:=re.match(r'    /\* ([0-9a-f]{8}) \*/',line):
            assert remaining>=4;data[section]+=struct.pack('<I',int(m[1],16));remaining-=4
        elif m:=re.match(r'    /\* ((?:00){1,3}) \*/ ; invalid',line):
            # Psyk prints trailing zero bytes separately from whole words.
            size=len(m[1])//2
            assert remaining==size
            data[section]+=bytes(size);remaining=0
        elif m:=re.fullmatch(r'10 : Patch type (\d+) at offset ([0-9a-f]+) with (.+)',line):
            assert remaining==0; patches.append((section,fragment+int(m[2],16),m[1],m[3]))
    assert remaining==0
    for section,offset,kind,expr in patches:
        if m:=re.fullmatch(r'\[([0-9a-f]+)\]',expr):value=symbols[int(m[1],16)]
        elif m:=re.fullmatch(r'\(sectbase\((\d+)\)\+\$([0-9a-f]+)\)',expr):value=bases[int(m[1])]+int(m[2],16)
        else:raise ValueError(expr)
        old=struct.unpack_from('<I',data[section],offset)[0]
        if kind=='82':new=(old&0xffff0000)|(((value+0x8000)>>16)&0xffff)
        elif kind=='84':new=(old&0xffff0000)|(value&0xffff)
        elif kind=='74':new=(old&0xfc000000)|((value>>2)&0x3ffffff)
        elif kind=='16':new=value
        else:raise ValueError(kind)
        struct.pack_into('<I',data[section],offset,new)
    return data

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('reference_object', type=Path)
parser.add_argument('dumps', type=Path, nargs='+')
args=parser.parse_args()
ns=runpy.run_path(str(Path(__file__).resolve().parents[1] / 'verify_behavior.py'))
with tempfile.TemporaryDirectory() as d:
    p=Path(d);(p/'link.ld').write_text(ns['candidate_layout'](args.reference_object)[0])
    subprocess.run(['mipsel-none-elf-ld','-T',str(p/'link.ld'),str(args.reference_object),'-o',str(p/'candidate.elf')],check=True)
    with (p/'candidate.elf').open('rb') as f:
        e=ELFFile(f)
        sections={1:e.get_section_by_name('.rodata'),2:e.get_section_by_name('.text'),3:e.get_section_by_name('.data')}
        reference={i:section.data() if section else b'' for i,section in sections.items()}
        bases={i:section['sh_addr'] if section else 0x80094528 for i,section in sections.items()}
    for dump in args.dumps:
        got=decode(dump, bases)
        assert got==reference,[(section,[hex(i) for i in range(0,len(got[section]),4) if got[section][i:i+4]!=reference[section][i:i+4]][:20]) for section in got]
        print(dump.name,'equals stock MASPSX after relocations:',len(got[2]),'text bytes,',len(got[1]),'rodata bytes,',len(got[3]),'data bytes')
