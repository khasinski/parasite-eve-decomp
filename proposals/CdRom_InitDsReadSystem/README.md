# DS read-system initialization

**Matched and integrated** in `src/main/cdrom/CdRom_InitDsReadSystem.c`.
All 324 linked bytes match retail using stock native GCC 2.8.1 with
`-mno-split-addresses` and the existing GNU assembler path. The source uses
the shared DS structures, no register pins, no empty barriers, and no
instruction assembly. The full EXE SHA-1 matches retail; all 18 behavioral
cases below pass with the production object. Linked bytes are guarded by
`tools/tests/test_gpu_submission_ds_init.py`.

## Historical proposal (before integration)

New ordinary-C reconstruction of `CdRom_InitDsReadSystem`, 324 retail bytes
at 0x8007EC14. The previous directory contained target/build artifacts but no
candidate C. This uses the already recovered DS queue and callback structures.
The current project categorizes the symbol as main-game; this proposal does
not independently prove its upstream SDK object or change category accounting.

If the system is already enabled, it returns zero without initialization.
Otherwise it resets the three callback-registry words, the state/result/eight
payload bytes of each of three command slots, eight queues through
CQ_clear_queue, queue indices and pending count, eight callback-slot value
words in descending order, and the callback cursor. Reserved bytes in the
slots are preserved. It initializes callback dispatch, installs four callback
addresses, invokes DS_read_cbready, clears DsReadCallback and returns one.
The callback setter casts follow the existing shared unsigned-integer ABI;
this candidate does not invent a replacement callback signature.

Stock GCC281 default addressing scores **72.69136%**. Unsplitting addresses
scores 58.024693% (unchanged with expensive optimizations disabled), and GCC272
scores 57.93827%. No pins, barriers, instruction asm or fabricated data-page
structures are used. This is a proposal, not a production match.

```sh
tools/scripts/cc.sh proposals/CdRom_InitDsReadSystem/candidate.c /tmp/ds-init.o
python proposals/CdRom_InitDsReadSystem/verify_behavior.py /tmp/ds-init.o
```

Eighteen SHA-1-checked retail comparisons cross three enabled-state results,
two memory fills and three external API return values. They compare complete
state snapshots at every external call, queue addresses/order, callback
identities, final memory (including preserved padding), return and SP.
CQ_clear_queue is modeled as clearing one 24-byte queue; other external APIs
are modeled with NOP entry stubs and return values. Their original bodies are
not executed, so this does not claim full subsystem integration. Mutants that
clear seven queues or register the wrong poll callback fail assertions.
