# CD sector DMA read

Production `CD_getsector2` at `0x8007BF44` matches all **256/256 linked bytes**.
The complete `sector_read` TU matches **492/492 bytes**, including its already-C
`CD_getsector` neighbor. Native GCC 2.8.1 with unsplit addresses and unmodified
GNU as compile the status polling loop as ordinary C. GNU as places the
backward branch label after the load-delay nop, as in retail.

The function waits for the sector-ready bit, starts DMA, waits for DMA to
finish, and restores the CD request register. No instruction asm remains.
The three existing pins are unchanged; no empty barriers were added.
`tools/tests/test_sector_file_search.py` checks all linked TU bytes against
the retail SHA-256, including relocations. No compiler or assembler patches,
postpasses, or binary rewriting are used.
