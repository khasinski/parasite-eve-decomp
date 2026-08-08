#!/usr/bin/env python3
"""Compile one translation unit and say whether an edit left its code identical.

Shared by the cleanup scripts. The rule everywhere is the same: take a
baseline object before the edit, delete the object, rebuild, compare. Deleting
first matters - a stale .o has twice made a failed edit look like a pass.

Compare the *sections*, not the whole file. A whole-object diff also sees the
symbol table and the gcoff line numbers, so deleting a single unused
declaration - which cannot change one instruction - shows up as a difference
and the edit gets reverted for nothing.
"""

import pathlib
import shutil
import subprocess

ROOT = pathlib.Path(__file__).resolve().parents[2]
BUILD = ROOT / 'build' / 'USA'
CC = ROOT / 'tools' / 'scripts' / 'cc.sh'
OBJCOPY = 'mipsel-none-elf-objcopy'
SECTIONS = ('.text', '.data', '.rodata', '.sdata', '.sbss', '.bss')


def object_for(src):
    return BUILD / (str(pathlib.Path(src).resolve().relative_to(ROOT)) + '.o')


def compile_one(src):
    src = pathlib.Path(src).resolve()
    obj = object_for(src)
    obj.parent.mkdir(parents=True, exist_ok=True)
    if obj.exists():
        obj.unlink()
    done = subprocess.run([str(CC), str(src.relative_to(ROOT)), str(obj.relative_to(ROOT))],
                          cwd=ROOT, capture_output=True, text=True, errors='replace')
    return done.returncode == 0 and obj.exists()


def sections(obj):
    """The bytes of every section that ends up in the image, keyed by name."""
    out = {}
    for name in SECTIONS:
        dump = obj.with_suffix('.o.%s.bin' % name.strip('.'))
        done = subprocess.run([OBJCOPY, '-O', 'binary', '--only-section', name,
                               str(obj), str(dump)],
                              capture_output=True, text=True, errors='replace')
        if done.returncode == 0 and dump.exists():
            out[name] = dump.read_bytes()
            dump.unlink()
    # Relocations move code just as surely as instructions do. Drop objdump's
    # banner: it names the file, which differs between object and baseline.
    done = subprocess.run(['mipsel-none-elf-objdump', '-r', str(obj)],
                          capture_output=True, text=True, errors='replace')
    body = [line for line in done.stdout.splitlines() if 'file format' not in line]
    out['<relocs>'] = '\n'.join(body).encode()
    return out


def snapshot(src):
    """Build `src` and stash its section bytes; returns the stash or None."""
    if not compile_one(src):
        return None
    obj = object_for(src)
    keep = obj.with_suffix('.o.baseline')
    shutil.copy(obj, keep)
    return keep


def matches(src, baseline):
    """True when `src` still compiles to the same code and data."""
    if not compile_one(src):
        return False
    return sections(object_for(src)) == sections(baseline)


def try_edit(src, original, candidate, baseline):
    """Write `candidate`; keep it if the object is unchanged, else restore."""
    src = pathlib.Path(src)
    src.write_text(candidate)
    if matches(src, baseline):
        return True
    src.write_text(original)
    compile_one(src)
    return False
