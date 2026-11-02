SECTIONS
{
    HIDDEN(__romPos = 0);
    fx_common_sector17_ROM_START = __romPos;
    fx_common_sector17_VRAM = ADDR(.fx_common_sector17);
    .fx_common_sector17 (OVERLAY_WINDOW_FX_SECTOR17 + 0x0) : AT(fx_common_sector17_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        fx_common_sector17_TEXT_START = .;
        build/JP/overlays/fx_common_sector17/asm/JP/overlays/fx_common_sector17/data/fx_common_sector17_copy.data.s.o(.code_data);
        . = ALIGN(., 4);
        fx_common_sector17_TEXT_END = .;
        fx_common_sector17_TEXT_SIZE = ABSOLUTE(fx_common_sector17_TEXT_END - fx_common_sector17_TEXT_START);
    }
    __romPos += SIZEOF(.fx_common_sector17);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    fx_common_sector17_ROM_END = __romPos;
    fx_common_sector17_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
