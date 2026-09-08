# Translation-unit audit

`tools/scripts/audit_tu_candidates.py` finds contiguous manifest entries where
a controller directly registers a callback from the immediately preceding
entry. It is evidence for review, not evidence of an original source boundary.

## Verified main units

| Range | Unit | Evidence |
| --- | --- | --- |
| `0x64F74..0x65238` | `gpu/libgpu_sys` | Seven contiguous functions match the known Psy-Q `libgpu/sys.c` tail in SDK source order. Their combined object retains every function's retail size; the sole changed relocation is the now object-local `SetDefDrawEnv` → `GetVideoMode` call. `make check` is retail-identical. |
| `0x76014..0x76180` | `psyq/libapi/counter` | Five contiguous functions match public Psy-Q `libapi/counter.c` in SDK source order. Their combined object retains every function's retail size; only jumps to each function's own epilogue become object-local relocations. `make check` is retail-identical. |
| `0x64494..0x64628` | `psyq/libapi/intr_callbacks` | Ten contiguous callback facade functions are grouped in public Psy-Q `etc/intr.c` in the same order. Reconciling their conflicting recovered callback prototypes to an unprototyped function pointer preserves all 404 `.text` bytes exactly; `make check` is retail-identical. |
| `0x64EA0..0x64F70` | `psyq/libapi/intr_dma` | The adjacent DMA callback registration and clear helpers are in public Psy-Q `etc/intr_dma.c` in the same order. Their combined object preserves all 208 `.text` bytes exactly; `make check` is retail-identical. |
| `0x68B44..0x68DD0` | `psyq/libgte/average` | Six adjacent vector-average API functions form a complete Psy-Q GTE family in API order. Their combined object preserves all 652 `.text` bytes exactly; `make check` is retail-identical. |
| `0x69834..0x69A40` | `psyq/libgte/pipeline` | Eleven adjacent pipeline API functions occur in the same order in public Psy-Q library implementations. Their combined object preserves all 524 `.text` bytes exactly; `make check` is retail-identical. |
| `0x6C2C0..0x6C3FC` | `psyq/libcd/bios` | The volume and interrupt initializers occur consecutively in public Psy-Q `libcd/bios.c`. Their shared register declarations and compiler flags produce an object whose 316 `.text` bytes are identical to the two prior objects; `make check` is retail-identical. |

## Deferred main pairs

| Range | Pair | Reason to defer |
| --- | --- | --- |
| `0x6D874..0x6D9CC` | `_SpuInit` / `SpuStart` | Public Psy-Q `s_ini.c` groups them, and the ranges are contiguous, but `_SpuInit` matches only with GCC 2.8.1 plus `-mno-split-addresses` while `SpuStart` matches with GCC 2.7.2. A combined trial changes both code streams, beyond the expected object-local call relocation. |

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
