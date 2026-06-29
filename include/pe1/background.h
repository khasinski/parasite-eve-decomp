#ifndef PE1_BACKGROUND_H
#define PE1_BACKGROUND_H

/* Parasite Eve field background subsystem -- CONFIRMED & VALIDATED decode.
 *
 * A field room background is 2D (flat). It is composited from TWO layers:
 *   LAYER 1  a grid of 16x16 textured tiles (the bulk: walls/floor/ceiling).
 *   LAYER 2  free-form sprite/poly pieces that fill the non-rectangular holes
 *            (door frames, pillars) AND occlude the live actors (so Aya is
 *            hidden when she walks "behind" them). Not constrained to the grid.
 * Both layers draw flat textured polys through the GTE/OT path, but the bg polys
 * are coplanar -- the perspective you see is baked into the pre-rendered 8bpp
 * texture pixels, not computed in 3D. There is NO per-cell tilemap array; the
 * "tile order" is the per-poly (screen-rect <-> texture-UV) list in the room
 * layout resource.
 *
 * Layer 1 has been decoded and validated: rendering it straight from PE.IMG
 * reproduces the Carnegie Hall corridor at mean abs err 2.53 / 99.3% within
 * tol48 vs the clean reference rip. See tools/scripts/render_bg_from_peimg.py.
 *
 * == PIXEL FORMAT ==
 *   8bpp paletted textures in VRAM texture pages; color via per-tile CLUT
 *   (BGR555). A page is 64 wide (16bpp VRAM units) x 256 tall = 128 x 256 in
 *   8bpp pixels, stored row-major (128-byte rows), 32768 bytes per page.
 *
 * == ROOM LAYOUT RESOURCE (the poly list) ==
 *   obj header: u16 count @+6, u32 mesh-array rel-off @+0x14, s16 display
 *               offset @+0x38 (x) / +0x3a (y).
 *   mesh entry (0x38 bytes each):
 *               u8  flags @+0     (bit1 0x2 = active/first-of-LOD-run)
 *               s16 scrX  @+0xC,  s16 scrY @+0xE   (base screen pos)
 *               u8  group @+0x24  (drawn when == g_GeomGroupSel @0x800BCFFD)
 *               u16 count @+0x26  (number of tile records)
 *               u32 POS-array rel-off @+0x28  (stride 4)
 *               u32 UV-array  rel-off @+0x2C  (stride 8)
 *   POS record (u32 a):
 *               X = scrX + dispX + ((a >> 22) & 0x3ff)
 *               Y = scrY + dispY + ((a >> 12) & 0x3ff)
 *   UV record  (u32 r0 ; byte+4 = U, byte+3 = V) -- a 16x16 texel block at (U,V):
 *               page tx  = (r0 >> 16) & 0xf      (page VRAM x16 = tx*64)
 *               page band: tx >= 12 -> VRAM y0, else VRAM y256  (atlas layout)
 *               clut x   = ((r0 >> 9) & 0x3f) * 16
 *               clut y   =  r0 & 0x1ff           (-> CLUT bank row)
 *   The LAYER-1 bg-tile entry is the mesh entry whose tiles all sample the bg
 *   texture pages (in the corridor: entry index 19, 289 tiles).
 *
 * == LAYER 2 (SOLVED -- same record format, just more entries) ==
 *   LAYER 2 turned out to use the IDENTICAL mesh-entry / tile-record format as
 *   LAYER 1 (POS array stride 4, UV array stride 8, tx=(uvword>>16)&0xf, etc.).
 *   There is no separate sprite sub-format: at runtime EVERY mesh entry is drawn
 *   through one path.  Confirmed by disassembly:
 *     Geo_BuildMeshList 0x80067A78 iterates the mesh-entry array and, for each
 *       entry with flag bit 0x2 set AND group(+0x24)==g_GeomGroupSel(0x800BCFFD),
 *       calls Render_DrawSpriteEntry 0x80067294.
 *     Geo_LoadMeshEntry 0x80066F60 is the BUILD step that decodes an entry's
 *       POS/UV tiles into GPU prims (this is the format Layer 1 documents; the
 *       UV word's bit 0x1000 -> sets the entry's drawable flag 0x2; it builds
 *       TWO LOD prim buffers at entry +0x30 and +0x34).
 *     Render_DrawSpriteEntry 0x80067294 only RE-POSITIONS/clips the already-built
 *       prims each frame (clips vs 320/224); texture+CLUT were baked at build.
 *   So a room's layout is one obj with N mesh entries, partitioned into GROUPS
 *   (entry +0x24).  Only ONE group is active per displayed view (g_GeomGroupSel).
 *   Within the active group, the drawable (flag 0x2) entries are:
 *     - one big full-screen entry      = LAYER 1 (the 16x16 grid),
 *     - several smaller flag-0x2 entries = LAYER 2 (door frames / pillars /
 *       actor-occluders that fill the non-grid holes).
 *   Entries with flag bit 0x2 CLEAR are LOD-0 / animation-frame duplicates that
 *   are not drawn.  Corridor (obj @ byte 22619344): group 0 drawable entries are
 *   e16,e17,e18 (Layer 2) + e19 (Layer 1, 289 tiles); group 1 is a 2nd camera
 *   view (e44..e49 Layer 2 + e50 Layer 1, 621 tiles).
 *   COMPOSITE ORDER: draw the big grid first, then the smaller flag-0x2 pieces
 *   ON TOP (PE's hole-fill/occlusion order).  The extractor's group_variants()
 *   'L1+L2' variant implements exactly this.
 *   TRANSPARENCY (essential): a texel whose 16-bit CLUT value is 0x0000 is
 *   TRANSPARENT (not drawn) -- standard PSX. The L2 pieces are non-rectangular,
 *   so their 16x16 tiles are mostly 0x0000 around the shape; you MUST skip those
 *   texels and keep the layer underneath. Drawing them opaque leaves black
 *   artifacts around door frames. With per-texel alpha the corridor reaches
 *   MAE 0.41 / tol48 100% / tol16 100% vs 9.png (vs 1.35 without).
 *
 * == DRAW / LOAD CODE ==
 *   loader  Scene_LoadFieldBg 0x8006AD40  src/main/main/Scene_LoadFieldBg.c
 *                                 (renamed from the mis-named Scene_Nop; room
 *                                 bg/asset loader, gated by g_GameState[0]&1)
 *                                 upload loop 0x8006AF18 (LoadImage records),
 *                                 tpage/clut pack loop 0x8006AFB4.
 *   Geo_LoadMeshEntry 0x80066F60  builds the tile primitives (the format above).
 *   Render_DrawSpriteEntry 0x80067294  flat screen-space path (clips vs 320/224).
 *   Render_DrawRoom   0x80068014  src/main/render/Render_DrawRoom.c (GTE/OT).
 *   Geo_BuildMeshList 0x80067A78 ; Gpu_LoadGeomState 0x80066800 ; called from
 *                     Entity_FrameUpdate 0x80035558 (@0x80035B0C).
 *   GetClut 0x80077AA4 ; GetTPage 0x80077A64 (libgpu).
 *   Asset_FindTable08ByU32Key 0x8006E498  (container resource lookup by u32 id).
 *
 * == RELEVANT GLOBALS ==
 *   g_GameState       0x800B0CD8   (loader uses s5):
 *                       +0x11C -> object/sprite-placement resource (id 0xC4B5BA04)
 *                       +0x120/+0x124 -> room geometry resource    (id 0xCAAD0704)
 *                       +0x14C -> loaded asset container (0xC-record directory)
 *                       +0x160/+0x174/+0x180/+0x188 -> streamed data buffers
 *   g_PeImageBaseLba  0x800B0DD8   PE.IMG base sector (loader s6). See pe_image.h.
 *   bg tpage/clut descriptor table 0x80091640 (2 x 0x10; computed tpage @+0x10,
 *                       clut @+0x12). Note: 0x80091640..0x8009166x is SHARED GPU
 *                       state (also referenced by menu/textbox code via .reloc),
 *                       so renaming needs the full byte-safe workflow.
 *   g_GeomGroupSel        0x800BCFFD   active group/view selector (u8). Written by
 *                       Gpu_LoadGeomState; read by Geo_BuildMeshList/
 *                       Geo_RenderMeshList/Render_* as an index of stride-52
 *                       entries in the State ctx g_GeomState (selects which
 *                       mesh-entry group / camera view is drawn).
 *   g_GeomState        field GPU/draw "State" context (typedef State); +0x38/+0x3A
 *                       = display/scroll offset added to bg tile screen X/Y.
 *   g_GeomOtZ        OT depth/Z accumulator advanced per mesh entry.
 *   PE.IMG room-data descriptor table (u16 offset pairs) 0x800930EA.
 *
 * == PE.IMG LAYOUT (Carnegie Hall corridor; verified byte-identical to a dump) ==
 *   Texture pages (32768 B each): contiguous from byte 22052872, BUT a 2048-byte
 *   alignment gap follows the 2nd page; disk order = VRAM (768,0)(832,0)(896,0)
 *   (960,0)(320,256)(384,256)..(768,256). CLUT bank @ byte 22448136 (VRAM y480,
 *   x0, 512-B row stride). Room layout obj @ byte 22619344. Asset container @
 *   byte 409600; geometry resource @ byte 414608. (Per-room offsets differ; the
 *   loader resolves them via 0x800930EA + g_PeImageBaseLba.)
 *
 * == CLUT-BANK SELECTION (per-room nuance) ==
 *   The CLUT bank is found as a high-STP-bit 32768-slot AFTER the texture pages.
 *   Noisy texture art can spike an individual page's STP fraction toward ~0.4,
 *   so a first-above-threshold scan can wrongly land on a texture page (this
 *   left sec16804 drawing only 80/562 tiles -- too dark).  Fix: when the obj's
 *   page demand is known (band0 prefix of tx 12..15 + band256 from tx=5), rank
 *   the slots AT/AFTER that demand by STP fraction and try the STRONGEST first;
 *   then let the edge-correlation-weighted reference matcher confirm.  sec16804
 *   -> CLEAN (clut slot 13, 13 pages, ref 36.png, edge 0.41).
 *
 * == OPEN ==
 *   - 6 rooms stay WEAK for honest (non-decode) reasons, NOT a format gap:
 *       * sec 8945 / 16411 / 17596: no structurally-matching reference exists in
 *         the rip set (best edge corr < 0.1 vs ALL refs though real content is
 *         drawn).  16411/17596 are a dim duplicate-layout pair (clut bank is a
 *         genuinely dark palette; a brighter alt clut exists at the next slot but
 *         no ref to disambiguate).
 *       * sec 11434 / 13408 / 13597: geometry+palette CORRECT (edge 0.49/0.51/
 *         0.26 vs 14/24/25.png) but the rip captured them under a runtime
 *         brightness/fade the static PE.IMG palette does not contain: a single
 *         per-channel scale (~0.30 / ~0.21 / ~0.05) collapses the drawn-pixel
 *         MAE to 11.9 / 6.6 / 3.6.  Not "fixed" in-tool (that would be faking the
 *         metric); flagged here as a runtime-lighting effect, not a decode bug.
 *   - Generic per-room locator in PE.IMG (room directory) for batch extraction.
 */

#endif /* PE1_BACKGROUND_H */
