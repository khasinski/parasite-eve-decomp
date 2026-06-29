#ifndef PE1_ACTOR_MODEL_H
#define PE1_ACTOR_MODEL_H

/* Actor 3D model / render object and MODEL SHARING.
 *
 * Each FieldActor embeds a "render object" at +0x1B4 (see field_actor.h). The
 * render object holds pointers to the actor's 3D model data (geometry + skeleton
 * + animation) and the per-instance transformed/projected vertex data.
 *
 * MODEL SHARING (confirmed live via DuckStation GDB, first field screen):
 *   The model is loaded ONCE; multiple actors of the same character reference
 *   the same model data. Two actors share a model iff their render objects hold
 *   the SAME geometry pointers at robj +0x00..+0x20.
 *   Observed: actors id3 and id4 shared model 0x80194xxx (the two duplicate
 *   NPCs); the player and actor id5 shared model 0x800f9xxx. An inactive actor
 *   has an all-zero render object.
 *   Shared (per-model, identical across instances): robj +0x00..+0x20 (geometry
 *   section pointers) and +0x54/+0x58/+0x80/+0x84 (bone/skeleton data) and +0xB0
 *   (animation data). Per-instance (differ per actor): +0x48/+0x50 (position),
 *   +0x5C..+0x78 (GTE-transformed vertices), the current anim frame, and the
 *   color/fade bytes (+0x88..+0x96).
 *
 * Render object (FieldActor + 0x1B4), offsets relative to the render object:
 *   +0x00..+0x20  9 model section pointers (verts/faces/bones/etc.) [SHARED per model]
 *   +0x24         linked source render object (Render_SetObjectAnim; attachment/
 *                 shared-animation, set by Entity_CopyAttachmentData) -- 0 if none
 *   +0x28         anim step/state (1 or 3)
 *   +0x2A         current anim id
 *   +0x30..+0x44  scale / small state (e.g. 0x00010000, 0x00000ccc)
 *   +0x48,+0x50   instance position (world/screen) [per-instance]
 *   +0x54,+0x58   bone/skeleton pointers [SHARED per model]
 *   +0x5C..+0x78  transformed/projected vertex coords [per-instance]
 *   +0x80,+0x84   bone-matrix source pointers [SHARED per model]
 *   +0x88..+0x96  color / fade state [per-instance]
 *   +0xB0         current animation data pointer [SHARED per model]
 *
 * MODEL ID / TYPE on the actor: bytes at FieldActor +0x0C and +0x0D form the
 * actor's model/type id (used by Entity_CopyAttachmentData to find another actor
 * of the same type to share animation from). flags +0x98 bit 0x10 = skip when
 * searching; the actor's parent link is +0x18C.
 *
 * LOAD / RENDER functions:
 *   Render_InitObjectFromTable(robj, owner, animIdx)  -- init robj from a model table
 *   Render_SetObjectAnim(robj, srcRobj, animId)       -- link/share animation
 *   Render_SetupEntityPrims(), Render_DrawWithAnim()  -- build + draw prims
 *   Entity_CopyAttachmentData()                        -- find same-type actor, share
 *   Scene_LoadEntityTexture()/Scene_LoadEntityTextures()/Asset_LoadTimTextures()
 *                                                      -- stream TIM textures from PE.IMG
 *   Gpu_LoadTimAsset()/Gpu_LoadTimTable()             -- upload TIMs to VRAM (LoadImage)
 *   Anim_BuildRotationMatrices(), Anim_DecodeBoneRotationsByte/Short(),
 *   Entity_AdvanceAnim(), Entity_TickAnimSequences()  -- skeletal animation
 *     (bone rotations are delta/abs-encoded; decoded to 3x3 matrices in scratch
 *      RAM 0x1F800000 for the GTE).
 *
 * Quick live probe: two actors A,B share a model iff
 *   mem(A+0x1B4, 0x24) == mem(B+0x1B4, 0x24)  (the geometry pointer block).
 *
 * MODEL DATA BLOCK FORMAT (TMD-like, PE-custom; read live from a shared model):
 *   The 9 pointers at robj +0x00..+0x20 point to consecutive sections of one
 *   model block loaded in RAM (streamed from PE.IMG as part of the scene/room
 *   load: Task_LoadSceneData / Scene_LoadMap / Render_InitDisplayLists).
 *   Render_SetupEntityPrims(robj, block) is the PARSER: it stores block -> robj
 *   +0x00, skips the 0x1C header (addiu a1,a1,0x1C), then walks the per-section
 *   element counts to compute and store each following section pointer into robj
 *   +0x04..+0x20. (Render_InitObjectFromTable separately binds counts/TPAGE from
 *   a per-id table at ownerTable+0x18, 0x10 bytes/entry: {field2C, field2E,
 *   field30, field70=TPAGE}.)
 *   Observed section roles (sizes vary per model):
 *     [0] +0x00  header (~0x1C): section counts (header shorts include the
 *                per-section element counts).
 *     [1] +0x04  node/bone table (~0x114).
 *     [2] +0x08  VERTICES: packed s16 (x,y,z) triples (e.g. (-6,-140,115)).
 *     [3] +0x0C  vertex COLORS: 0xRRGGGBBcc words (0x00808080 = neutral gray).
 *     [4] +0x10  PRIMITIVE/POLYGON list (largest): per-poly {color, type byte
 *                (e.g. 0x0A = gouraud), vertex indices} -> the GPU face list.
 *     [5] +0x14  small table (~0x10).
 *     [6] +0x18  skeleton / bone data (~0x170).
 *     [7] +0x1C  (~8 bytes) and [8] +0x20 = bone index / tail tables.
 *   Animation overlays bone rotations onto this shared geometry per frame
 *   (Anim_DecodeBoneRotations* -> 3x3 matrices in scratch RAM 0x1F800000 for the
 *   GTE), and Render_TransformSkinnedVertices projects per-instance verts.
 *   Instances of the same character reuse [0]..[8] verbatim (shared pointers);
 *   only the projected vertices / anim frame / colour are per-actor.
 */

#endif /* PE1_ACTOR_MODEL_H */
