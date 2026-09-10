# LIBSPU SPU.OBJ initialization

`src/main/psyq/libspu/spu_init.c` reconstructs all 640 bytes of `_spu_init`,
retail address 0x8007D1D4 (file offset 0x6D9D4). It uses stock native GCC 2.7.2
and GNU assembly, without register pins, compiler barriers or instruction ASM.

Both hot and cold initialization enable SPU DMA priority, clear transfer state,
mute the master output and reset SPUCNT. A bounded status poll reports a timeout
when its incremented counter exceeds 0xF00, then continues initialization. The
function sets the SPU address-unit constants, transfer control, reverb volumes,
key-off and reverb-enable masks, and clears the ten-halfword request buffer.

Cold initialization additionally clears pitch modulation, noise and external
input volumes; uploads the initial 16-byte sample; resets all 24 voice register
records; then keys voices on and off with four delay calls after each step.
Both paths finish by enabling SPUCNT (0xC000), setting the transfer-ready state,
and clearing the transfer/IRQ callbacks.

The code reuses the shared SpuRegs and SpuVoiceRegs layouts. The request-buffer
clear uses volatile halfword stores to preserve the retail ascending write
sequence; the compiler otherwise reverses that loop. Initializing its counter
before the address-unit setup preserves the retail scheduling without a pin.

## SDK evidence and scope

PsyQ 4.6 LIBSPU.LIB exports `_spu_init` from SPU.OBJ at text offset 0 and names
the associated transfer/address state, address-unit constants and callback
pointers in its data section. Relative to `_spu_RXX`, offsets 0x18/0x1C/0x20
identify the transfer address/mode and address mode; 0x24..0x30 are address-unit
constants; 0x34 is `_spu_inTransfer`; 0x38/0x3C are callback pointers. These agree
with the project's existing retail aliases rooted at 0x8009B3FC. The SDK also
declares `_spu_RQ` as 0x14 bytes, consistent with the ten retail halfword stores.

That reference SDK version is not asserted to have identical initialization
instruction bytes. The acceptance target is the game's retail executable.
This TU covers only the initialization fragment of the larger SPU object; the
adjacent transfer functions remain in their existing units.

## Verification

`make -j4 verify-clean` runs the repository tests and checks the complete main
executable against retail SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
An additional standalone compile/link comparison at 0x8007D1D4 verified all
640 function bytes, including resolved relocations. Shared aliases in the
relocatable comparison were paired by their existing retail addresses; the
linked comparison uses unmodified retail bytes.
