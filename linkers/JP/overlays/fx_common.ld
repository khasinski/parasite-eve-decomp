SECTIONS
{
    HIDDEN(__romPos = 0);
    fx_common_ROM_START = __romPos;
    fx_common_VRAM = ADDR(.fx_common);
    .fx_common (OVERLAY_WINDOW_ROOM + 0x8) : AT(fx_common_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        fx_common_RODATA_START = .;
        build/JP/overlays/fx_common/asm/JP/overlays/fx_common/data/fx_common/fx_common_header.s.o(.rodata);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/effects.c.o(.rodata);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/camera.c.o(.rodata);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/motion_endpoints.c.o(.rodata);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/camera_path.c.o(.rodata);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/effect_init.c.o(.rodata);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/model_buffers.c.o(.rodata);
        . = ALIGN(., 4);
        fx_common_RODATA_END = .;
        fx_common_RODATA_SIZE = ABSOLUTE(fx_common_RODATA_END - fx_common_RODATA_START);
        fx_common_TEXT_START = .;
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/camera.c.o(.text);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/effects.c.o(.text);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/motion_endpoints.c.o(.text);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/camera_path.c.o(.text);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/effect_init.c.o(.text);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/model_buffers.c.o(.text);
        . = ALIGN(., 4);
        fx_common_TEXT_END = .;
        fx_common_TEXT_SIZE = ABSOLUTE(fx_common_TEXT_END - fx_common_TEXT_START);
        fx_common_DATA_START = .;
        build/JP/overlays/fx_common/asm/JP/overlays/fx_common/data/fx_common/fx_common_header.s.o(.data);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/effects.c.o(.data);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/camera.c.o(.data);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/motion_endpoints.c.o(.data);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/camera_path.c.o(.data);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/effect_init.c.o(.data);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/model_buffers.c.o(.data);
        build/JP/overlays/fx_common/asm/JP/overlays/fx_common/data/g_FxCommonCameraAngles.data.s.o(.data);
        . = ALIGN(., 4);
        fx_common_DATA_END = .;
        fx_common_DATA_SIZE = ABSOLUTE(fx_common_DATA_END - fx_common_DATA_START);
        fx_common_BSS_START = .;
        build/JP/overlays/fx_common/asm/JP/overlays/fx_common/data/fx_common/fx_common_header.s.o(.bss);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/effects.c.o(.bss);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/camera.c.o(.bss);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/motion_endpoints.c.o(.bss);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/camera_path.c.o(.bss);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/effect_init.c.o(.bss);
        build/JP/overlays/fx_common/src-jp/overlays/fx_common/model_buffers.c.o(.bss);
        . = ALIGN(., 4);
        fx_common_BSS_END = .;
        fx_common_BSS_SIZE = ABSOLUTE(fx_common_BSS_END - fx_common_BSS_START);
    }
    __romPos += SIZEOF(.fx_common);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    fx_common_ROM_END = __romPos;
    fx_common_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
