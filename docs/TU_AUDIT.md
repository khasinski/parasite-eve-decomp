# Translation-unit audit

`tools/scripts/audit_tu_candidates.py` finds contiguous manifest entries where
a controller directly registers a callback from the immediately preceding
entry. It is evidence for review, not evidence of an original source boundary.

## Verified main units

| Range | Unit | Evidence |
| --- | --- | --- |
| `0x64F74..0x65238` | `gpu/libgpu_sys` | Seven contiguous functions match the known Psy-Q `libgpu/sys.c` tail in SDK source order. Their combined object retains every function's retail size; the sole changed relocation is the now object-local `SetDefDrawEnv` → `GetVideoMode` call. `make check` is retail-identical. |
| `0x6586C..0x659E4` | `psyq/libgpu/sys_transfer` | The adjacent `LoadImage`, `StoreImage`, and `MoveImage` VRAM transfer helpers occur in this order in public Psy-Q `libgpu/sys.c`. Their shared GCC 2.8.1 `-mno-split-addresses` object preserves all 376 `.text` bytes exactly; `make check` is retail-identical. |
| `0x664E8..0x66950` | `gpu/drawenv` | The contiguous front and back DRAWENV encoders share their compiler profile and packet construction helpers. They also confirm that GPU state offsets `0x04` and `0x06` are the display width and height. The combined object preserves all 1128 `.text` bytes exactly; `make check` is retail-identical. |
| `0x66970..0x66AA0` | `gpu/drawarea` | The paired top-left and bottom-right draw-area packet encoders share the same `-O1` profile, state dimensions, and clamp algorithm. Their combined object preserves all 304 `.text` bytes exactly; `make check` is retail-identical. |
| `0x67DE8..0x68200` | `gpu/dma_transfer` | Four contiguous DMA transfer operations share the retry loop, DMA state, callback installation, and GPU dispatch table. The combined object preserves all 1048 `.text` bytes exactly and removes duplicate controller declarations; `make check` is retail-identical. |
| `0x76014..0x76180` | `psyq/libapi/counter` | Five contiguous functions match public Psy-Q `libapi/counter.c` in SDK source order. Their combined object retains every function's retail size; only jumps to each function's own epilogue become object-local relocations. `make check` is retail-identical. |
| `0x64494..0x64628` | `psyq/libapi/intr_callbacks` | Ten contiguous callback facade functions are grouped in public Psy-Q `etc/intr.c` in the same order. Reconciling their conflicting recovered callback prototypes to an unprototyped function pointer preserves all 404 `.text` bytes exactly; `make check` is retail-identical. |
| `0x64EA0..0x64F70` | `psyq/libapi/intr_dma` | The adjacent DMA callback registration and clear helpers are in public Psy-Q `etc/intr_dma.c` in the same order. Their combined object preserves all 208 `.text` bytes exactly; `make check` is retail-identical. |
| `0x68B44..0x68DD0` | `psyq/libgte/average` | Six adjacent vector-average API functions form a complete Psy-Q GTE family in API order. Their combined object preserves all 652 `.text` bytes exactly; `make check` is retail-identical. |
| `0x69834..0x69A40` | `psyq/libgte/pipeline` | Eleven adjacent pipeline API functions occur in the same order in public Psy-Q library implementations. Their combined object preserves all 524 `.text` bytes exactly; `make check` is retail-identical. |
| `0x6C2C0..0x6C3FC` | `psyq/libcd/bios` | The volume and interrupt initializers occur consecutively in public Psy-Q `libcd/bios.c`. Their shared register declarations and compiler flags produce an object whose 316 `.text` bytes are identical to the two prior objects; `make check` is retail-identical. |
| `0x6ABCC..0x6ACD0` | `psyq/libcd/sys` | Seven contiguous CD API wrappers occur in public Psy-Q `libcd/sys.c` in this source order. The combined 260-byte object changes only the two relocations for its own `CdComstr` and `CdIntstr` epilogues; `make check` is retail-identical. |
| `0x6B0EC..0x6B2B4` | `psyq/libcd/sys_data` | The data callback, synchronization wrapper, and CD-position conversion helpers occur together in public Psy-Q `libcd/sys.c`. Their combined object preserves all 456 `.text` bytes exactly; `make check` is retail-identical. |
| `0x6B08C..0x6B0EC` | `psyq/libcd/sys_control` | Three contiguous control and sector-read wrappers form the preceding `libcd/sys.c` API fragment. Their combined object preserves all 96 `.text` bytes exactly; `make check` is retail-identical. |
| `0x6DED0..0x6E2E0` | `psyq/libspu/spu_transfer` | Four contiguous DMA transfer functions occur in public Psy-Q `libspu/spu.c` in this source order. The 1040-byte combined object changes only seven calls that become object-local relocations; `make check` is retail-identical. |
| `0x6E2E0..0x6E3C8` | `psyq/libspu/spu_register_write` | The consecutive register write helpers occur together in public Psy-Q `libspu/spu.c`. Their 232-byte combined object changes only two relocations for its own return path; `make check` is retail-identical. |
| `0x6E404..0x6E508` | `psyq/libspu/spu_control` | Controller-priority and delay helpers occur with the SPU hardware wait loop in public Psy-Q `libspu/spu.c`. The 260-byte combined object changes only the wait loop's object-local jump relocation; `make check` is retail-identical. |
| `0x72CB4..0x72D14` | `psyq/libds/dscb` | Four adjacent stream start, sync, ready, and DMA-data callback registration helpers form the complete local callback block; the latter three occur in public Psy-Q `libds/dscb.c` in this source order. The combined object preserves all 96 `.text` bytes exactly; `make check` is retail-identical. |
| `0x72574..0x7265C` | `psyq/libds/dsread_control` | The adjacent asynchronous-read initializer and break helper share the `libds` read-controller state and compiler profile. Their combined object preserves all 232 `.text` bytes exactly; `make check` is retail-identical. |
| `0x72C00..0x72CA8` | `psyq/libds/dstype` | The disk-type synchronization callback registers the adjacent `GD_disk_kind` function as an async-read callback; its prior `char[]` declaration masked that relationship. Giving it the true function prototype forms the public Psy-Q `libds/dstype.c` callback block and preserves all 168 `.text` bytes exactly; `make check` is retail-identical. |

## Deferred main pairs

| Range | Pair | Reason to defer |
| --- | --- | --- |
| `0x653B8..0x654B8` | `SetGraphDebug` / `SetGraphQueue` | Public Psy-Q `libgpu/sys.c` puts the adjacent functions together, but the recovered queue helper requires `-mno-split-addresses` and its callback has a narrower prototype. Reconciling the callback to the shared ABI and compiling with the required flags changes `SetGraphDebug` and produces 260 rather than 256 bytes; defer until the shared GPU callback model matches. |
| `0x6D874..0x6D9CC` | `_SpuInit` / `SpuStart` | Public Psy-Q `s_ini.c` groups them, and the ranges are contiguous, but `_SpuInit` matches only with GCC 2.8.1 plus `-mno-split-addresses` while `SpuStart` matches with GCC 2.7.2. A combined trial changes both code streams, beyond the expected object-local call relocation. |
| `0x6C744..0x6C930` | `CD_getsector2` / `CD_getsector` | Public Psy-Q `libcd/bios.c` groups the adjacent low-level sector readers, but the recovered `CD_getsector` uses global `$v0` and `$sp` register variables. Declaring them for the shared object changes the earlier `CD_getsector2` register allocation across its code stream; defer until that recovered register model is reconciled. |

## Deferred room_m350 pairs

The following pairs are contiguous and directly connected, but are not merged
until their competing record or instance views are reconciled without changing
the matching object code:

| Range | Callback and controller | Reason to defer |
| --- | --- | --- |
| `0x57D4..0x5F1C` | `HomingTrailController` / `SequenceSpawner` | Two incompatible semantic views of a 28-byte pool record. |
| `0x6B7C..0x70DC` | `ActorOffsetPaletteCallback` / `FadingPairEmitter` | Callback and controller reconstruct different `Instance` transform views. |
| `0x91E8..0x9878` | `TransformedFlare` / `SweptAreaEmitter` | Competing `Instance` and `Actor` layouts require a common verified view. |
| `0x9878..0x9D10` | `AttachedCloud` / `AttachedCloudSpawner` | The callback and controller use different names and meanings for fields after the common prefix. |
| `0xA6D4..0xAB24` | `SineFadeCallback` / `ModelTransformEmitter` | Common 24-byte particle is established; merge must preserve the callback's `worldY` view at offset `0x200`. |

Every proposed merge must retain manifest order, pass its behavioral tests,
`make overlay-check OVERLAY=room_m350`, and a clean verification before it is
accepted.
