# DsRead2 flag reconstruction

Production now registers the data callback with an ordinary C call. This
proposal also reconstructs the remaining stream-flag assembly as C:
bit 0x20 clears g_DsStreamNoLocFlag, otherwise the flag becomes 1.

Stock GCC 2.8.1 with default address splitting scores 99.0566% against the
expected object. An empty input barrier on the value 1 after its store
retains the target branch layout and placement of its constant load.
Using the original mode and position parameters directly removes both old
register pins without changing this result. The candidate has one empty
input barrier and no register pins; it is not an exact match.

The remaining instruction-operand differences are in the flag window:

- Target constant 1 uses v0; candidate uses v1.
- Both target global addresses use AT; candidate uses v0.
- The nonzero store consequently reads v1 instead of v0.

The canonical flag symbol also differs from the expected object's D_ alias.
The ordinary C callback call and the remaining function body otherwise have
the expected instruction sequence. This is not a linked-byte match claim.

Enabling `-fcall-used-$1` changes the masks testing 0x100 and 0x20 and both
stack arguments containing -1 to AT as well. Trials pinning those values
and constraining their lifetimes did not recover the complete match.
Disabling address splitting loses the callback-address call delay slot.
No compiler, assembler or postpass modification is used or proposed.

```sh
tools/scripts/cc.sh proposals/DsRead2/candidate.c /tmp/dsread2-candidate.o
tools/objdiff/objdiff-cli diff -1 expected/build/USA/src/main/psyq/libds/DsRead2.c.o -2 /tmp/dsread2-candidate.o -o /tmp/dsread2-candidate.json
```

Further AT-allocation trials constrained both mode tests to v0 with tied
outputs and the two -1 stack arguments to their target registers. This
corrected those operands but changed the prologue schedule and prevented
the constant 1 from occupying the original branch delay slot. Neither an
AT clobber in the nonzero arm nor a memory clobber recovered the match.
Those variants are not retained as the best candidate.
