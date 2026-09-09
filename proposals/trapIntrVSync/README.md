# trapIntrVSync: matched and promoted

The 108-byte function at 0x8007440C is now built from
src/main/psyq/libapi/trapIntrVSync.c. The candidate is the same source.
Stock GCC281 with unsplit addresses reproduces all **108/108 linked retail
bytes**, without pins, barriers, instruction ASM or postpasses.

The source increments the volatile VSync counter with unsigned arithmetic
before conversion back to its signed storage type, preserving the retail
32-bit wrap. It then visits eight callback slots in order, loading each slot
when reached. Callback changes to later slots therefore take effect during
the same dispatch. A callback's modifications to the counter are retained.

Initializing the loop index before incrementing the counter, and assigning
the table pointer afterward, produces the retail register allocation and
prologue. The equivalent earlier pointer-loop layout scored 75.51852%; an
indexed loop scored 79.03704% but emitted an unnecessary per-iteration shift.
The final object initially scored 98.888885% against the old expected object
only because g_VSyncCount and g_IntrVSyncCallbackTable had different symbol
names from D_800956AC and D_8009568C. Linking both names to the existing shared
addresses removes all differences.

Shared callback-table/counter declarations now live in psyq_api_internal.h;
redundant declarations in the initializer and setter unit were removed.
The manifest selects the new C function.

```sh
tools/scripts/cc.sh proposals/trapIntrVSync/candidate.c /tmp/trap-vsync.o
python proposals/trapIntrVSync/verify_behavior.py /tmp/trap-vsync.o
make check
```

The candidate and production object pass **3,072 cases** against the
SHA-1-checked retail body and a callback-mutation model: all 256 callback
masks, four counter boundary values and three callback mutation modes.
The verifier checks ordered callback-entry snapshots, removal/replacement of
future slots, changes to an already visited slot and the counter, final table
and count, all callee-saved registers, SP and canaries. Callback bodies are
modeled; no hardware is emulated. Mutants visiting only seven slots or
incrementing the counter twice fail assertions.

After regenerating expected objects directly from retail, the production
object scores **100.0%** in objdiff. The full main executable retains SHA-1
452fb033f2eaa4b18aa20a5bca60b8125af3a37b. All 191 overlay hashes, 290 repository
tests, source mapping/policy and debt/organization gates pass.
