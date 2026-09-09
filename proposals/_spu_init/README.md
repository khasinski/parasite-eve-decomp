# Low-level SPU initialization

`candidate.c` reconstructs all 640 retail bytes at 0x8007D1D4 as C, using
SpuRegs and SpuVoiceRegs. It has no pins, barriers or instruction ASM.
Stock GCC272 scores 88.8625%; production remains ASM. GCC281 unsplit scores
82.06875% with or without expensive optimizations, and split scores 69.93125%.
Remaining differences include register allocation, address scheduling and
symbol aliases. These are function scores, not new matched production bytes.

The routine enables SPU DMA priority bits, resets transfer software state,
zeros master volumes and SPUCNT, waits once, then polls the low eleven status
bits. A timeout after 3841 unsuccessful reads prints a diagnostic and CONTINUES
initialization. It establishes memory-mode values 2/3/8/7, transfer control 4,
clears reverb settings and a ten-halfword software buffer, and keys voices off.

Mode zero additionally clears modulation/noise and external-input volumes,
transfers the 16-byte payload at D_8009B43C, initializes six fields in every one
of the 24 voice records, keys voices on, waits four times, keys them off and
waits four more times. Envelope and repeat-address fields are not cleared.
Nonzero modes skip that cold-start block. Both paths enable SPUCNT with 0xC000,
set the transfer-active flag and clear transfer/IRQ callback pointers.

The shared SpuRegs map now names key_on, key_off, pitch_modulation,
noise_enable and reverb_enable as pairs of halfwords at 0x188..0x19B, and
transfer_control at 0x1AC. These replace padding without changing layout.
The access widths and offsets come from retail instructions; register meanings
are corroborated by the [psx-spx SPU register map](https://psx-spx.consoledev.net/soundprocessingunitspu/).
The unused range 0x19C..0x1A5 retains its previous padding representation.

```sh
tools/scripts/cc.sh proposals/_spu_init/candidate.c /tmp/spu-init.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/main/_spu_init.s.o \
  -2 /tmp/spu-init.o -o /tmp/spu-init.json
python proposals/_spu_init/verify_behavior.py /tmp/spu-init.o
```

The finite Unicorn comparison checks 48 cases: hot values 0/1/-1, pending
status reads 0/1/3840/3841, initial fill 0/A5, and optional replacement of the
SPU pointer by the first modeled wait. It compares every register access,
diagnostic/payload/wait call, final register and software state, buffer bounds
and stack restoration. Original and candidate text use separate RAM regions.
The external wait, transfer and printf bodies are modeled, not executed;
their entry words are replaced by NOPs and hooks stop on the second word before
servicing the model. Hooking the unmodified wait entry allowed its stack
prologue to affect the emulation around a call with an MMIO delay slot; the
NOP stubs ensure the modeled functions do not partially execute.

Negative controls for 23 voices, early timeout, aborting initialization after
the diagnostic, and reversing the hot/cold condition are all rejected.
These tests are not physical SPU timing verification or a whole-library proof.

Shared-header validation: main retail SHA-1, all 191 overlay SHA-1 checks,
290 repository tests and source/organization/debt gates pass.

The original routine explicitly returns zero on both hot and cold paths.
The shared declaration is now int _spu_init(int hot), replacing the incorrect
local void declaration in _SpuInit.c. The behavior suite asserts v0 == 0 for
all 48 cases; returning one is rejected. Two independent local reconstructions
(psyz libspu/spu.c and sotn-decomp psxsdk/libspu/spu.c) corroborate the int/s32
return type; retail instructions remain the primary evidence.

Separating each reloaded register pointer into a local block raised the old
83.20625% source to 86.925%; correcting the return raised it to 88.8625%.
The change expresses independent pointer lifetimes without register pins.
Disabling both GCC272 schedulers scores 73.1% with the corrected source.

The combined SPU object additionally supports `--real-io`: 192 cases execute
both reconstructed functions with independent reset and transfer delays.
The default 48-case mode still models transfer; both modes pass after the
transfer's single control-store refinement. See ../spu_init_io/README.md.
