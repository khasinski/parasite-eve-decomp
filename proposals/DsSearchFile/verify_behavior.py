#!/usr/bin/env python3
"""Finite retail-MIPS comparison; CD/cache callees are modeled, not hardware.

Usage: verify_behavior.py candidate.o
Requires Unicorn, pyelftools, and mipsel-none-elf-binutils.
"""
import re, struct, subprocess, hashlib, itertools, sys, tempfile
from pathlib import Path
from elftools.elf.elffile import ELFFile
from unicorn import *
from unicorn.mips_const import *
object_path = Path(sys.argv[1]).resolve()
work = tempfile.TemporaryDirectory(prefix='ds-search-behavior-')
link_script = str(Path(work.name) / 'candidate.ld')
linked_object = str(Path(work.name) / 'candidate.elf')
exe=Path('build/USA/main.exe').read_bytes()
assert hashlib.sha1(exe).hexdigest()=='452fb033f2eaa4b18aa20a5bca60b8125af3a37b'
syms={n:int(a,16) for n,a in re.findall(r'^(\w+) = (0x[0-9A-Fa-f]+);',Path('configs/USA/sym.main.txt').read_text(),re.M)}
undef=subprocess.check_output(['mipsel-none-elf-nm','-u',str(object_path)],text=True).split()
names=undef[1::2]
script='\n'.join(f'{n} = {syms.get(n,int(n[2:],16) if n.startswith("D_") else 0):#x};' for n in names)
script+='\nSECTIONS { .text 0x80081414 : { *(.text) } /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.comment) *(.gnu.attributes) } }'
Path(link_script).write_text(script)
subprocess.run(['mipsel-none-elf-ld','-T',link_script,str(object_path),'-o',linked_object],check=True)
with open(linked_object,'rb') as f:
 elf=ELFFile(f);entries={s.name: s['st_value'] for s in elf.get_section_by_name('.symtab').iter_symbols()};patches=[(s['sh_addr'],s.data()) for s in elf.iter_sections() if s['sh_flags']&2 and s['sh_size']]
def run(case,patch):
 path,hit,cache_ok,media_ok,cached,debug=case
 u=Uc(UC_ARCH_MIPS,UC_MODE_MIPS32|UC_MODE_LITTLE_ENDIAN);u.mem_map(0,0x200000)
 start,size=struct.unpack_from('<II',exe,0x18);u.mem_write(start&0x1fffffff,exe[0x800:0x800+size])
 for a,d in patch:u.mem_write(a&0x1fffffff,d)
 def wr(a,v):u.mem_write(a&0x1fffffff,struct.pack('<I',v))
 def rdstr(a):
  b=bytearray()
  while True:
   c=u.mem_read(a&0x1fffffff,1)[0]
   if not c:return bytes(b)
   b.append(c);a+=1
   assert len(b)<256
 wr(0x8009B6E0,cached);wr(0x8009AFC0,debug)
 u.mem_write(0x100000,path+b'\0');u.mem_write(0x110000,b'\xcc'*64)
 for i in range(64):
  name=(b'FILE.BIN;1' if i==hit else b'OTHER.BIN;1') if hit>=0 or i<3 else b''
  u.mem_write(0xa36b0+i*24,struct.pack('<II',0x01020300+i,100+i)+name.ljust(16,b'\0'))
 trace=[]
 def call(cpu,a,size,data):
  fn=data;args=[cpu.reg_read(r) for r in [UC_MIPS_REG_A0,UC_MIPS_REG_A1,UC_MIPS_REG_A2]]
  if fn=='CdRom_GetDiskType':v=2;trace.append((fn,))
  elif fn=='DS_newmedia':v=media_ok;trace.append((fn,))
  elif fn=='DS_searchdir':
   name=rdstr(args[1]);trace.append((fn,args[0],name));v=0xffffffff if name==b'BAD' else args[0]+bool(name)
  elif fn=='DS_cachefile':v=cache_ok;trace.append((fn,args[0]))
  elif fn=='_cmp':v=int(rdstr(args[0])[:12]==rdstr(args[1])[:12])
  elif fn=='puts':trace.append((fn,rdstr(args[0])));v=0
  else:
   fmt=rdstr(args[0]);trace.append((fn,fmt,rdstr(args[1]),args[2] if args[0]==0x80011e6c else 0));v=0
  cpu.reg_write(UC_MIPS_REG_V0,v);cpu.reg_write(UC_MIPS_REG_PC,cpu.reg_read(UC_MIPS_REG_RA))
 for n in ['CdRom_GetDiskType','DS_newmedia','DS_searchdir','DS_cachefile','_cmp','puts','printf']:
  address=entries.get(n,syms[n]) if patch else syms[n]
  u.hook_add(UC_HOOK_CODE,call,user_data=n,begin=address,end=address)
 u.reg_write(UC_MIPS_REG_A0,0x80110000);u.reg_write(UC_MIPS_REG_A1,0x80100000)
 u.reg_write(UC_MIPS_REG_SP,0x801ff000);u.reg_write(UC_MIPS_REG_RA,0x8000f000)
 u.emu_start(0x80081414,0x8000f000,count=100000)
 assert u.reg_read(UC_MIPS_REG_PC)==0x8000f000
 return u.reg_read(UC_MIPS_REG_V0),bytes(u.mem_read(0x110000,64)),bytes(u.mem_read(0x9b6e0,4)),trace
paths=[b'',b'FILE.BIN;1',b'\\FILE.BIN;1',b'\\DIR\\FILE.BIN;1',b'\\BAD\\FILE.BIN;1',b'\\',b'\\A\\B\\C\\D\\E\\F\\FILE.BIN;1',b'\\A\\B\\C\\D\\E\\F\\G\\FILE.BIN;1']
for n,case in enumerate(itertools.product(paths,[-1,0,2,63],[0,1,0xffffffff],[0,1],[0,2],[0,2]),1):
 a,b=run(case,[]),run(case,patches)
 if a!=b:print('FAIL',case,a,b);raise SystemExit(1)
print('PASS',n,'cases: return pointers, output records, media state and external-call traces')
