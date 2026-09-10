# getintr matching state

This candidate reconstructs all five LIBCD interrupt cases and the unknown
interrupt diagnostic. Its `DataReady` error suppression follows retail: an
error response containing exactly one FIFO byte is accepted, so the condition
uses the received-byte count `i == 1`.

Stock GCC 2.8.1 emits `0x564` bytes against retail's `0x55C`. The remaining
eight bytes arise from register allocation and scheduling; all retail switch
paths and result-buffer copies are represented.

Reproduce the comparison by temporarily placing `getintr.c` under
`src/main/psyq/libcd/` and running:

    python3 tools/scripts/probe_match.py psyq/libcd/getintr
