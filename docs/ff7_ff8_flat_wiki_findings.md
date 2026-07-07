# FF7/FF8 Flat Wiki Findings for PE1

Working notes from the local clone of
<https://github.com/ff7-mods/ff7-flat-wiki> at `reference/ff7-flat-wiki`.

This is not evidence that PE1 uses identical formats. Treat it as a source of
candidate structure names, field semantics, and high-value comparison points.
Only move anything into headers after PE1 code/data confirms it.

## High-Confidence Reuse Areas

### AKAO sound driver

Useful sources:

- `reference/ff7-flat-wiki/docs/FF7/PSX/Sound/AKAO_sequence.md`
- `reference/ff7-flat-wiki/docs/FF7/PSX/Sound/INSTRx.DAT.md`
- `reference/ff7-flat-wiki/docs/FF7/PSX/Sound/INSTRx.ALL.md`
- `reference/ff7-flat-wiki/docs/FF7/PSX/Sound/Code_Map.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_DAT.md`
- `reference/ff7-flat-wiki/docs/FF8/WorldMap_music.md`

Local PE1 cross-checks:

- `docs/assets/formats-and-consumers.md`
- `docs/assets/function-asset-map.md`
- `src/main/main/Scene_LoadFieldBg.c`

What already mapped well in PE1:

- `AkaoInstrument` is a 0x40-byte instrument record: SPU start address, loop
  address, ADSR bytes, then 12 pitch words.
- PE1 sequence opcodes share many FF7 names: load instrument, volume/pan,
  ADSR edits, loops, note flags, pitch/fine-tune/portamento-ish commands.
- AKAO data can be embedded in multiple module/resource types, not only a
  global music archive. FF8 `DAT` section 9 and world-map notes reinforce that
  sound blobs may be carried by field/battle/world resources.
- PE1 room loading already points in this direction: `Scene_LoadFieldBg.c`
  documents resource key `0x5EAF6804` as room AKAO/audio resolved through
  `Asset_FindTable08ByU32Key`. A raw text search for `"AKAO"` is therefore not
  enough; room resources may be keyed and parsed without carrying a visible
  ASCII marker in nearby code.

Actionable PE1 work:

- Keep replacing raw voice update masks with `AKAO_VOICE_PARAM_*`.
- Add sequence header/drum-map structs only after finding PE1 data or parser
  code that reads the header offsets/masks.
- Search scene/battle resource loaders for embedded `"AKAO"` magic, not only
  sound-loader paths.
- Trace consumers of room resource key `0x5EAF6804` and the loader that turns
  it into AKAO commands or SPU sample uploads.
- The FF7 field script AKAO command list is useful for public command wrapper
  names, but do not blindly name PE1 lower command IDs from it. PE1 wrappers
  can carry extra queue/slot arguments around the same underlying sequence
  opcodes.

Open PE1 checks:

- Whether PE1 has FF7-style relative channel offsets in sequence headers.
- Whether PE1 uses drum maps / overlay voices / alternate voices in the same
  form.
- Whether PE1 battle/room overlays embed AKAO frames in resource sections.

### TIM / BGR555 texture handling

Useful sources:

- `reference/ff7-flat-wiki/docs/PSX/TIM_format.md`
- `reference/ff7-flat-wiki/docs/FF7/Field/MIMfile.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_MIM.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_MAP.md`

What maps conceptually:

- Paletted PSX image data and BGR555 CLUT handling line up with PE1's texture
  and background decode notes.
- FF8 MIM/MAP describes field backgrounds as 16x16 tiles sourced from packed
  texture pages, with per-tile palette/page/layer/blend metadata. PE1's room
  backgrounds use a different container and packed record layout, but the same
  rendering idea.
- FF7 field backgrounds also draw first-layer tiles behind second-layer
  occluder sprites, matching PE1's current `background.h` model at a conceptual
  level.

Actionable PE1 work:

- Use wiki terminology (`tile`, `source page`, `CLUT`, `layer`, `blend mode`)
  when naming PE1 background fields, but keep PE1-specific record layouts.
- The FF8 MAP blend/layer bytes are worth comparing against PE1 UV/control
  bits, especially any remaining unknowns in the mesh-entry or UV record.
- PE1's layer-2 occluders are probably best documented as "background
  occluder tiles/pieces", not as generic sprites.

Open PE1 checks:

- Whether any unknown PE1 UV bits correspond to FF7/FF8 blend modes or
  animation state.
- Whether field background animation in PE1 has an FF7/FF8-like per-tile state
  table or a separate PE-specific controller.

### Field / battle character models

Useful sources:

- `reference/ff7-flat-wiki/docs/FF8/FileFormat_MCH.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_DAT.md`
- `reference/ff7-flat-wiki/docs/FF7/Field/Models.md`
- `reference/ff7-flat-wiki/docs/PSX/TIM_format.md`

What maps conceptually:

- FF8 MCH field models carry TIM textures, mesh geometry, bones, skeleton
  hierarchy, skinning data, and texture animation. PE1 `actor_model.h` already
  observes model sections for vertices, colors, polygon lists, skeleton/bone
  data, and animation pointers.
- FF8 battle DAT is closer as a decomposition model: separate skeleton,
  geometry, animation, texture animation, AI/scripts, sounds, and TIM sections.
  PE1's resources are packed differently, but the section responsibilities are
  familiar.
- FF8 texture animation entries mention copied UV regions for eye/blink-like
  effects. This is a good lead for unexplained PE1 small model tables or
  texture-update routines.

Actionable PE1 work:

- Keep PE1 model headers named around confirmed sections:
  `vertex`, `vertex_color`, `primitive/polygon`, `skeleton/bone`,
  `bone_index/tail`, `animation`.
- Compare PE1 model section counts and strides against FF8 MCH/DAT only as a
  sanity check. The PE1 live observations suggest a custom TMD-like block, not
  an exact MCH file.
- Look for texture-animation code around model render/update functions before
  assigning names to the small unknown model sections.

Open PE1 checks:

- Which PE1 model section owns bone parent indices versus bone lengths.
- Whether the small tables at model sections `[5]`, `[7]`, and `[8]` correspond
  to skin objects, bone remaps, texture animation, or attachment metadata.
- Whether battle models and field models share the same PE1 block format.

### FF8 field-engine parallels

Useful sources:

- `reference/ff7-flat-wiki/docs/FF8.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_MCH.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_ONE.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_MIM.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_MAP.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_JSM.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_MSD.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_INF.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_CA.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_MSK.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_SFX.md`

What this suggests for PE1:

- FF8 splits a field into named responsibilities: textured character model
  container, background image pages, background tile map, field script, dialog
  text, gateways/exits, camera data, masks, particles, and field SFX IDs.
  PE1's containers are different, but its engine code already has matching
  responsibilities spread across `pe_image.h`, `background.h`,
  `actor_model.h`, field collision headers, text/menu code, and AKAO wrappers.
- The strongest comparison target is not one FF8 file format, but the loader
  graph: a room should resolve background pages/layout, actor model/texture
  resources, script/text data, collision/walkmesh, camera/bounds, and local
  sound references as one field package.
- FF8 `MCH` is a good vocabulary source for PE1 field actors: TIM textures,
  mesh geometry, skeleton hierarchy, skin objects/ranges, and texture
  animation. PE1 model data is not MCH, but the unknown PE1 model sections
  should be tested against those roles before inventing neutral names.
- FF8 `MIM/MAP` reinforces the background model already proven in PE1:
  paletted texture pages plus compact tile records with source coordinates,
  palette/CLUT selection, layer/grouping, blend/effect bits, and animation
  state. PE1 uses a custom POS/UV record pair, but unknown UV/control bits
  should be interpreted through this lens.

Actionable PE1 work:

- Build a PE1 "field package" map from loader call sites:
  `Scene_LoadRoom`, `Scene_LoadFieldBg`, `Scene_LoadRoomAssets`,
  `Task_LoadSceneData`, `Render_InitDisplayLists`, `Scene_LoadEntityTextures`,
  `Asset_FindTable*`, and room overlay init functions.
- Rename or document resources by responsibility first: `field_background`,
  `field_bg_layout`, `field_actor_model`, `field_actor_texture`,
  `field_script`, `field_text`, `field_collision`, `field_camera_bounds`,
  `field_sfx_table`, `field_akao`.
- For model decompilation, prioritize parser/setup routines that compute the
  nine render-object section pointers. Cross-check section sizes/strides
  against FF8's skeleton, vertex, face, skin-object, and texture-animation
  roles.
- For background decompilation, search for code that reads PE1 UV bits beyond
  page/CLUT/source coordinates. Compare those paths against FF8 MAP layer,
  blend mode, background animation id, and animation state fields.
- For overlays, classify repeated room-local functions by which part of the
  field package they touch: actor/model controller, camera/scroll controller,
  background/occluder animation, local sound trigger, or field script task.

Open PE1 checks:

- Whether PE1 has a field-local SFX table analogous to FF8 `SFX`, or whether
  it only stores AKAO/resource keys directly.
- Whether PE1 camera/bounds data maps more closely to FF8 `CA/INF` concepts or
  is fully baked into scripts/state tables.
- Whether PE1 masks/occluders are only background mesh entries, or whether
  there is also an FF8-like explicit mask resource.
- Whether PE1 actor texture updates include FF8-style copied UV-region texture
  animation for eyes/faces, damage states, or costume variants.

### FF8 field package vs PE1 room package

The useful comparison is the package boundary, not the byte layout. FF8 names
the field package pieces directly, while PE1 hides similar responsibilities
behind `PE.IMG` resource lookup and room-local loaders.

| Responsibility | FF8 wiki source | PE1 evidence | How to use it |
|---|---|---|---|
| Field actor model container | `FileFormat_ONE`, `FileFormat_MCH` | `actor_model.h`, `Scene_LoadEntityTexture(s)`, `Render_InitObjectFromTable`, `Render_SetupEntityPrims` | Treat PE1 actor data as a custom MCH-like block: textures + geometry + skeleton + animation. Do not copy MCH offsets. |
| Actor textures | `MCH` embedded TIM list, `ONE` model texture/data offsets | `Gpu_LoadTimAsset`, `Asset_LoadTimTextures`, `Scene_LoadEntityTexture(s)` | Search texture load call sites before naming unknown actor model sections as texture animation. |
| Skeleton and skinning | `MCH` bones/skin objects, battle `DAT` section 1/2 | `actor_model.h` sections `[1]`, `[6]`, `[7]`, `[8]`, animation matrix builders | Test PE1 small sections against bone parent, bone tail/index, and skin-object range roles. |
| Texture animation | `MCH` texture-animation records, battle `DAT` section 4 | PE1 unknown small model sections and render/update code | Good lead for eye/blink/equipment variants, but only after finding code that copies or changes UV regions. |
| Background image pages | `FileFormat_MIM` | `background.h` texture pages and CLUT bank | Use the same vocabulary: page, CLUT, 8bpp texel, transparent color 0. |
| Background tile/layout map | `FileFormat_MAP` | PE1 room layout mesh entries, POS arrays, UV arrays, `Geo_LoadMeshEntry` | Compare unknown PE1 UV bits against FF8 page, palette, layer, blend, animation id/state fields. |
| Camera/bounds/gateways | `FileFormat_CA`, `FileFormat_INF` | `scene_transition.h`, `field_collision.h`, camera functions in `function_names.md` | Look for PE1 transition/camera structs as room package metadata, not generic task state. |
| Field script | `FileFormat_JSM`, FF8 opcode pages | `Task_*`, `Scene_LoadMap`, room overlays, `function_names.md` task names | Use FF8 script categories to classify PE1 task commands, not opcode numbers. |
| Dialog/text | `FileFormat_MSD` | `text.h`, menu/text draw code, message task functions | Useful as a category in the package map; no direct PE1 format match yet. |
| Field SFX/music | `FileFormat_SFX`, FF7/FF8 music/effect opcodes, AKAO docs | `Task_QueueFieldSfx`, `Task_PlayFieldSfx`, AKAO command wrappers, room audio key `0x5EAF6804` | Strong lead for naming field sound queues and room-local audio resources. |

Concrete PE1 implication: when we see an unknown room resource or overlay-local
state, first ask which package role it serves. A room overlay touching actor
position plus animation IDs is probably field-script/actor-control state. A
resource consumed by `Geo_LoadMeshEntry` is background layout. A short table
feeding `Gpu_LoadTimAsset` is texture metadata. This should reduce ambiguous
names like "data table" and "sprite" in favor of package-role names.

### Model-section hypotheses from FF8 MCH/DAT

PE1's model format is not FF8 MCH, but the section roles are close enough to
give us a test plan for the unknown PE1 model sections:

- PE1 section `[1]` should be checked for MCH-like bone parent/length data or
  DAT-like per-bone transform defaults. If code walks it once per bone and then
  feeds scratchpad matrices, name it as a skeleton/node table rather than an
  unknown header.
- PE1 section `[5]` is small and sits after primitive data. FF8 MCH has an
  "unknown data" table that partitions skin objects, triangles, and quads. A PE1
  equivalent might be a primitive-range/object-range table.
- PE1 section `[6]` is already likely skeleton/bone data. Compare access widths:
  parent/length fields are usually `s16`, while matrix/rotation tables may be
  larger fixed-point fields.
- PE1 sections `[7]` and `[8]` are good candidates for bone index tails,
  attachment metadata, or section terminators. If any code uses them to select a
  subset of vertices, they may be skin-object/bone-remap tables.
- If a PE1 routine copies rectangular UV areas or swaps texture coordinates
  frame-by-frame, compare it with FF8 `texAnim` records before naming the data
  as generic animation.

Good local functions to inspect next:

- `Render_SetupEntityPrims`: parser that computes model section pointers.
- `Render_InitObjectFromTable`: binds per-actor model counts/TPAGE data.
- `Anim_BuildRotationMatrices`, `Anim_DecodeBoneRotationsByte`,
  `Anim_DecodeBoneRotationsShort`: confirms bone/animation encodings.
- `Render_TransformSkinnedVertices`: confirms skin-object and bone-index roles.
- `Entity_CopyAttachmentData`: confirms model sharing and attachment semantics.

### Background-control hypotheses from FF8 MIM/MAP

FF8 MAP has explicit fields for page, palette, layer, blend mode, background
animation id, and animation state. PE1 background records are different, but
`background.h` still has unknown/control bits around the UV word and mesh entry
flags.

PE1 checks worth doing:

- Audit every read of PE1 UV record words in `Geo_LoadMeshEntry`,
  `Render_DrawSpriteEntry`, and adjacent geometry builders. Separate proven
  page/CLUT/source-coordinate bits from still-unknown control bits.
- Search for writes to `g_GeomGroupSel` and mesh-entry `group @+0x24`; this is
  the closest PE1 equivalent to FF8's layer/view selection.
- Search for per-frame changes to mesh-entry flags or alternate prim buffers at
  entry `+0x30/+0x34`; these may be PE1's equivalent of background animation
  state.
- Treat layer 2 as "background occluder pieces" unless code proves a separate
  mask resource. FF8 has an explicit `MSK` category, but PE1 currently proves
  occlusion through the same mesh-entry/tile format as the main background.

### Camera, collision, and exits

FF8 `INF` and `CA` are useful because they split field metadata into camera
ranges, screen ranges, gateways, triggers, and camera matrices. PE1 already has
matching concepts, but they are scattered across runtime globals and task code.

PE1 naming guidance:

- Use `gateway`, `exit_line`, or `transition_trigger` only for data that actually
  causes `Scene_LoadRoom`/`Scene_LoadMap` or writes the current room id.
- Use `camera_range`, `camera_target`, `scroll`, or `screen_range` only where
  code clamps or interpolates the view, not for general actor position data.
- Keep `collision`/`walkmesh` names for `field_collision.h` structures: triangle
  entries, vertex tables, plane tables, region-height tables.
- For overlays, a function that combines actor movement, camera tracking, and a
  room transition is likely field-script control logic, not model/render logic.

## Medium-Confidence Leads

### Field script sound commands

Useful sources:

- `reference/ff7-flat-wiki/docs/FF7/Field/Script/Opcodes/F2_AKAO.md`
- `reference/ff7-flat-wiki/docs/FF7/Field/Script/Opcodes/DA_AKAO2.md`
- `reference/ff7-flat-wiki/docs/FF8/Field/Script/Opcodes/*MUSIC*.md`
- `reference/ff7-flat-wiki/docs/FF8/Field/Script/Opcodes/*EFFECT*.md`

The command families line up with what PE1 likely needs: play music, stop
music, play sound effects with pan/channel, pause/resume music or SFX, volume
transitions, and preserve sounds across field transitions.

Use this for naming PE1 script commands only after call-site confirmation. The
same numeric IDs are not guaranteed across games.

### Resource packaging patterns

Useful sources:

- `reference/ff7-flat-wiki/docs/FF8/FileFormat_DAT.md`
- `reference/ff7-flat-wiki/docs/FF8/WorldMap_wmsetxx.md`
- `reference/ff7-flat-wiki/docs/FF8/PlaystationMedia.md`

Square resources often group code/data-like sections by responsibility:
geometry, animation, scripts, text, textures, sounds. PE1's PE.IMG container
has its own directory structure, but the same mental model helps classify
unknown room/battle resources.

Actionable PE1 work:

- When an unknown resource contains TIM magic, AKAO magic, high-density text, or
  short relative offsets, classify it by section behavior before naming it.
- Use "resource section" names until a PE1 loader confirms a game-specific file
  format name.

## Things That Do Not Transfer Directly

- FF8 MCH file headers are not PE1 model headers. PE1 appears to stream a
  custom model block and builds section pointers at runtime.
- FF8 MIM/MAP files are not PE1 room background files. PE1 has a different
  room layout resource with mesh entries, POS arrays, UV arrays, and group
  selection.
- FF7/FF8 field script opcode numbers should not be copied into PE1 without
  matching PE1 script decoding code.
- FF7 AKAO code-map addresses are only naming/reference material. PE1 addresses
  and player structs differ.

## Best Next Targets

1. Continue AKAO structure cleanup in functions still using `track + offset`,
   especially sequence loop/call-stack handlers and voice allocation paths.
2. Search PE1 resource loaders for `"AKAO"` to find embedded sequence blobs in
   room/battle data.
3. Compare PE1 unknown model sections against FF8 MCH/DAT concepts:
   skin-object ranges, bone parent tables, texture animation, and per-object
   triangle/quad ranges.
4. Compare unknown PE1 background UV/control bits against FF8 MAP layer/blend
   fields.
5. Add provisional structs only in headers where at least two PE1 functions or
   one parser plus one runtime observation agree.

## Wider Field-Engine Leads

The extra value from a broader pass over `ff7-flat-wiki` is mostly in FF7/FF8
field data, not in sound. These formats are useful for classifying PE1 overlays
because room overlays often mix actor control, background animation, camera
state, local SFX, and transition triggers.

### Field script entity categories

Useful sources:

- `reference/ff7-flat-wiki/docs/FF8/FileFormat_JSM.md`
- `reference/ff7-flat-wiki/docs/FF8/Field/Script/Opcodes.md`
- `reference/ff7-flat-wiki/docs/FF7/Field/Script/Opcodes.md`

FF8 `JSM` separates field-script entities into doors, walkmesh lines,
background entities, and other entities. Even if PE1 does not use FF8 bytecode,
that split is a good naming lens:

- Player position checks plus room jump/camera writes are likely
  door/gateway/walkmesh-line triggers.
- Tile groups, palette state, scroll values, or occluder toggles are likely
  background entity controllers.
- Actor animation, facing, movement, or visibility without resource parsing is
  likely actor-script state.
- Room overlay calls into AKAO wrappers are often local SFX triggers, not
  global music commands.

Good provisional role names: `field_trigger`, `room_gateway`, `bg_entity`,
`actor_script`, `local_sfx`.

### Background tile records

Useful sources:

- `reference/ff7-flat-wiki/docs/FF8/FileFormat_MAP.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_MIM.md`
- `reference/ff7-flat-wiki/docs/FF7/Field/Background.md`
- `reference/ff7-flat-wiki/docs/FF7/Field/MIMfile.md`

FF8 `MAP` packs destination X/Y, source X/Y, texture page, palette, layer id,
blend mode, background animation id, and animation state into compact tile
records. PE1 has a custom format, but these are exactly the responsibilities
we should test before leaving PE1 UV/control bytes anonymous.

Concrete PE1 checks:

- Unknown background bits used near `setSemiTrans`, `setTPage`, CLUT setup, or
  ordering-table insertion should be tested as blend/layer/page fields.
- Small per-tile state arrays in room overlays may be background animation
  controllers. Keep animation id and animation state separate as hypotheses.
- PE1 occluder code should stay distinct from actor sprites. FF7/FF8 both use
  foreground/layer-2 background pieces that visually cover field actors.

### Field actor models and texture animation

Useful sources:

- `reference/ff7-flat-wiki/docs/FF8/FileFormat_MCH.md`
- `reference/ff7-flat-wiki/docs/FF8/FileFormat_ONE.md`
- `reference/ff7-flat-wiki/docs/FF7/Field/Models.md`
- `reference/ff7-flat-wiki/docs/FF7/Battle_model_format_(PSX).md`

FF8 `MCH` gives vocabulary for textured field characters: TIM texture list,
model data, bone count, vertex count, texture-animation count, face count,
skin-object count, skeleton data, vertex data, faces, and skin objects. PE1 is
not MCH, but unknown PE1 model sections should be tested against those roles.

Concrete PE1 checks:

- A model section indexed once per bone and feeding matrix/rotation code should
  get `skeleton`, `bone`, or `bone_hierarchy` names.
- A small section storing first vertex/count plus a bone id is probably a
  skin-object or bone-range table.
- Code that copies or swaps UV rectangles should be compared with FF8 MCH
  texture animation before being treated as normal animation data. This may
  explain blink/face/detail updates.
- FF7 field model loader records expose bone count, part count, animation
  count, and global model id. PE1 room actor loader tables should be checked
  for the same responsibility pattern.

### Overlay triage table

| Signals in code | Likely role | Better temporary name |
|---|---|---|
| Actor position, facing, animation id, visibility | Field actor script | `RoomActor*`, `FieldActorScript*` |
| Transition id, camera bounds, player region, map jump | Door/gateway trigger | `RoomGateway*`, `FieldTrigger*` |
| Tile page/CLUT/UV/layer/blend/OT writes | Background tile setup | `FieldBg*`, `BgTile*` |
| Small state machines over tile groups | Background entity animation | `BgEntity*`, `BgAnim*` |
| AKAO/SFX wrappers with room-local ids | Local sound trigger | `FieldSfx*`, `RoomAudio*` |
| Model section pointer math, bones, vertices, primitives | Actor model loader/render | `ActorModel*`, `ModelSection*` |

The stable name should come from the package role the overlay owns, not from
the first helper API it happens to call.
