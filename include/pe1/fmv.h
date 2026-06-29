#ifndef PE1_FMV_H
#define PE1_FMV_H

/* FMV / streamed-video subsystem (opening movie, cutscenes).
 *
 * Characterised live on real BIOS (DuckStation GDB) by stepping through the
 * first FMV sequence: captured the game-state transitions in/out of playback.
 * The decoder itself is not in src/ yet (MDEC/DecDCT pipeline, still in asm/ or
 * libpress); what is documented here is the OBSERVABLE main-RAM footprint, which
 * is what a PC port must reproduce at the engine level.
 *
 * == DISC SOURCE ==
 *   FMV streams live on the disc as \FMV1\PEDISC01.IDF;1 (CD1) and
 *   \FMV2\PEDISC02.IDF;1 (CD2) -- the same files OpenPeImage searches for disc
 *   detection (see pe_image.h). They are streamed via the CD/DsRead path, not
 *   read out of PE.IMG.
 *
 * == STATE-MACHINE BRACKETING (verified live) ==
 *   FMV is dispatched as its own SCENE through the Boot_MainLoop dispatcher on
 *   D_8009D280 (0x8009D280, the scene-dispatch token; see Boot_MainLoop). For the
 *   opening movie:
 *       before FMV : D_8009D280 = 0xA80001C8   flags = 0x40000000
 *       FMV playing: D_8009D280 = 0xA8067148   flags = 0x4000A240
 *       after  FMV : D_8009D280 = 0xA8000248   flags = 0x40002000
 *   (token values are per-scene; the point is FMV gets a distinct token and
 *    returns to a gameplay token on completion.)
 *
 *   FMV-ACTIVE FLAGS in D_800B0CD8[0] (0x800B0CD8): the bits set only while a
 *   movie plays are 0x8000 | 0x200 | 0x40 = 0x8240. They clear on exit. (0x2000
 *   is a general "game running" bit that persists into gameplay; 0x40000000 is
 *   always set here.)
 *
 * == PER-FRAME ACTIVITY (verified by snapshot-diff during playback) ==
 *   Main RAM churn during playback is small (~53 dwords in 0x80090000..0x800B2000),
 *   i.e. the VIDEO does NOT pass through observable main RAM (see below). What
 *   advances each frame:
 *     - a synchronised frame/stream counter, mirrored at 0x8009D250,
 *       0x800A34xx and the low half of 0x800B0DBC (all step together).
 *     - SPU voice state at 0x800B0028..0x800B00C0 (per-voice volumes 0x7FFF /
 *       0x6D3E / ...) -> this is the FMV AUDIO (XA / streamed-to-SPU).
 *
 * == VIDEO PATH ==
 *   MDEC-decoded frames are DMA'd straight to VRAM; the pixel data never lands
 *   in CPU-visible main RAM (a 0x801D0000 buffer that the scene-load pointers
 *   reference stays 0% changed during playback -- it is static loaded data, not
 *   the video). Consequence: the frames are invisible to the GDB main-RAM path
 *   (DuckStation's RSP does not expose VRAM). Port implication: the decode is
 *   CD-STR -> MDEC -> DMA -> VRAM; reimplement as a stream demux + video decode
 *   to a texture, driven by the same frame counter, with audio to the SPU mixer.
 *
 * Quick live probe (tools at /tmp/fmv_probe.py): scene token = *(u32*)0x8009D280;
 *   FMV-active iff (*(u32*)0x800B0CD8 & 0x8240) != 0; frame counter at 0x8009D250.
 */

#endif /* PE1_FMV_H */
