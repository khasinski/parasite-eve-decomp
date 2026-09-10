# PsyQ LIBSN `SNMAIN` translation unit

PsyQ 4.6 `LIBSN.LIB` names the original module `SNMAIN`. Its `.text` is one
contiguous 384-byte unit containing, in order:

| Function | Offset | Size |
| --- | ---: | ---: |
| `__SN_ENTRY_POINT` | `0x000` | `0xA8` |
| `__main` | `0x0A8` | `0x70` |
| `__do_global_dtors` | `0x118` | `0x68` |

Against retail `0x80072534..0x800726B4`, the PsyQ 4.6 module has all 67
non-relocated instruction words identical. The other 29 words differ only in
the relocation fields filled by the linker. PsyQ 4.7 retains the `SNMAIN`
module but changes its startup code, including the initial stack adjustment,
so it is not the retail version used here.
This proves that the three current ASM entries are one SDK translation unit and
that their unusual 4/8/12 save layout and constructor/destructor call sequence
come directly from the distributed SN runtime object.

`__SN_ENTRY_POINT` is explicitly handwritten startup code. The adjacent
`__main` and `__do_global_dtors` C reconstructions live in their own proposal
directories while the shared stock-pipeline frame convention remains
unresolved. Production boundaries should eventually be replaced together as
this `SNMAIN` unit rather than treated as unrelated functions.

```sh
python proposals/libsn_runtime/verify_sdk.py \
  "/path/to/Psy-Q - 46/LIB/LIBSN.LIB"
```
