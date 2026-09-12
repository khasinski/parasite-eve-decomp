# StopRCnt in the counter translation unit

Production matches all **52/52 linked bytes** of StopRCnt at `0x80085918`,
and all **364/364 bytes** of the complete counter TU. The function clears the
selected bits in the interrupt mask register and returns one. Its index is
the low 16 bits of the argument; it does not introduce a range check.

Native GCC 2.7.2 and unmodified GNU as reproduce the indexed load using v0
for both the address scratch and result. The old instruction-asm block and
its a1 pin are removed. All five functions retain their original source order.

`tools/tests/test_counter_spu_address.py` checks every linked byte, including
relocations and unchanged neighbors, against the retail executable's SHA-256.
