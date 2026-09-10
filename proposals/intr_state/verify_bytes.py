#!/usr/bin/env python3
"""Compile and link the INTR fragment, then compare every byte to retail."""
import hashlib
import os
from pathlib import Path
import re
import subprocess
import tempfile

from elftools.elf.elffile import ELFFile

ROOT = Path(__file__).resolve().parents[2]
FUNCTIONS = (
    ('Render_InitSceneGeom', 0x64700, 464),
    ('Sys_SetIntrCallback', 0x648D0, 328),
    ('Sys_SaveDisableIntr', 0x64A18, 160),
    ('Sys_RestoreIntr', 0x64AB8, 120),
)


def verify(work):
    obj = work / 'intr_state.o'
    subprocess.run([
        'tools/scripts/cc.sh', 'src/main/psyq/libapi/intr_state.c', str(obj)
    ], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    undefined = subprocess.check_output(
        ['mipsel-none-elf-nm', '-u', str(obj)], text=True).split()[1::2]
    symbols = {}
    for filename in ('configs/USA/sym.main.txt',
                     'linkers/USA/undefined_syms_manual.txt'):
        symbols.update(re.findall(r'^(\w+) = (0x[0-9A-Fa-f]+);',
                                 Path(filename).read_text(), re.M))
    definitions = [f'{name} = ' +
                   ('0x' + name[2:] if name.startswith('D_') else symbols[name]) + ';'
                   for name in undefined]
    script = work / 'intr_state.ld'
    linked = work / 'intr_state.elf'
    script.write_text('\n'.join(definitions) +
                      '\nSECTIONS { .text 0x80073F00 : { *(.text) } '
                      '/DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) '
                      '*(.comment) *(.gnu.attributes) } }')
    subprocess.run(['mipsel-none-elf-ld', '-T', str(script), str(obj),
                    '-o', str(linked)], check=True)
    retail = Path('assets/USA/main.exe').read_bytes()
    assert hashlib.sha1(retail).hexdigest() == '452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
    with linked.open('rb') as stream:
        elf = ELFFile(stream)
        section = elf.get_section_by_name('.text')
        data, base = section.data(), section['sh_addr']
        symbols = {s.name: s for s in elf.get_section_by_name('.symtab').iter_symbols()}
        for name, offset, size in FUNCTIONS:
            symbol = symbols[name]
            start = symbol['st_value'] - base
            assert symbol['st_size'] == size, name
            assert data[start:start + size] == retail[offset:offset + size], name
            print(f'{name}: {size}/{size} linked bytes match retail')


if __name__ == '__main__':
    os.chdir(ROOT)
    with tempfile.TemporaryDirectory(prefix='intr-state-bytes-') as work:
        verify(Path(work))
