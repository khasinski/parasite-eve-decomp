#include "common.h"
#include "pe1/asset_find.h"
#include "pe1/field_actor.h"
#include "pe1/field_effect_modules.h"
#include "pe1/field_effect_pool.h"
#include "pe1/field_flash_burst.h"
#include "pe1/field_glow_line.h"
#include "pe1/field_history_trail.h"
#include "pe1/field_orbit_point.h"
#include "pe1/field_owner_actor.h"
#include "pe1/field_pulled_sprite.h"
#include "pe1/field_ring_band.h"
#include "pe1/field_rotated_quad.h"
#include "pe1/field_rotated_triangle.h"
#include "pe1/field_shape_quads.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_star_fan.h"
#include "pe1/field_textured_ribbon.h"
#include "pe1/field_tile.h"
#include "pe1/field_tri_test.h"
#include "pe1/fieldfx_task.h"
#include "pe1/geom_state.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/pe_image.h"
#include "pe1/psyq_gpu.h"
#include "pe1/random.h"
#include "pe1/render_object.h"
#include "pe1/render_prim.h"
#include "pe1/room_fx.h"
#include "pe1/scene_assets.h"

/* Matching debt: three transfer-register pins per function, plus five empty
 * pointer/memory barriers across this TU. Matrix loads are C; each GTE
 * instruction and hazard nop uses its individual macro. */

void FieldFx_TransformMatrixPoint(RoomFxTransformOwner *owner, int index,
                                  const GteShortVector *input, GteShortVector *output) {
    GteVector result;
    GteMatrixWords *matrix = (GteMatrixWords *)&owner->transforms[index];
    GteMatrixWords untranslated;

    gte_ldrotmatrix(matrix);
    untranslated.tz = 0;
    untranslated.ty = 0;
    untranslated.tx = 0;
    gte_ldtransmatrix(&untranslated);

    gte_lwc2_0_0(input);
    gte_lwc2_1_4(input);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    gte_swc2_25_0(&result);
    gte_swc2_26_4(&result);
    gte_swc2_27_8(&result);

    /* Add translation modulo 32 bits before narrowing to halfwords. */
    output->x = result.x + owner->transforms[index].x;
    output->y = result.y + owner->transforms[index].y;
    output->z = result.z + owner->transforms[index].z;
}

void FieldFx_GetTransformLookAngles(RoomFxTransformOwner *owner, int index, GteShortVector *out)
{
    GteShortVector direction = D_800C2258;
    GteShortVector origin = g_FieldFxZeroVector;
    GteVector result;
    GteMatrixWords local;
    GteMatrixWords *matrix;
    GteShortVector *vector = &direction;
    GteShortVector *from = &origin;

    matrix = (GteMatrixWords *)&owner->transforms[index];
    gte_ldrotmatrix(matrix);
    local.tx = local.ty = local.tz = 0;
    gte_ldtransmatrix(&local);
    gte_lwc2_0_0(vector);
    gte_lwc2_1_4(vector);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    gte_swc2_25_0(&result);
    gte_swc2_26_4(&result);
    gte_swc2_27_8(&result);
    direction.x = result.x;
    direction.y = result.y;
    direction.z = result.z;
    FieldFx_CalculateLookAngles(from, vector, out);
}

void FieldFx_RotateVector(const GteMatrixWords *matrix,
                           const GteShortVector *input, GteShortVector *output) {
    GteVector result;
    GteMatrixWords untranslated;

    untranslated.tz = 0;
    untranslated.ty = 0;
    untranslated.tx = 0;
    gte_ldrotmatrix(matrix);
    gte_ldtransmatrix(&untranslated);

    gte_lwc2_0_0(input);
    gte_lwc2_1_4(input);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    gte_swc2_25_0(&result);
    gte_swc2_26_4(&result);
    gte_swc2_27_8(&result);

    output->x = result.x;
    output->y = result.y;
    output->z = result.z;
}

static inline int div4096(int value) {
    if (value < 0) {
        value += 0xFFF;
    }
    return value >> 12;
}

int FieldFx_IsPlayerInSegmentBand(s16 *a, s16 *b, int radius) {
    int angle;
    GteShortVector points[4];
    int dx;
    int dz;
    int x;
    int z;

    dx = b[0] - a[0];
    dz = b[2] - a[2];
    angle = -ratan2(dz, dx);
    x = div4096(rsin(angle) * radius);
    z = div4096(rcos(angle) * radius);

    /* Both trig calls are complete; reuse the angle scratch for the X offset. */
    angle = x;
    dz = z;
    points[0].x = a[0] - angle;
    points[0].z = a[2] - dz;
    points[1].x = a[0] + angle;
    points[1].z = a[2] + dz;
    points[2].x = b[0] - angle;
    points[2].z = b[2] - dz;
    points[3].x = b[0] + angle;
    points[3].z = b[2] + dz;

    return FieldFx_IsPlayerInQuadXZ(points);
}

void FieldFx_InitTexturePageTable(void) {
    int i;

    for (i = 0; i < 2; i++) {
        g_FieldFxTpageTable[i * 2] = GetTPage(i, 0, 0x380, 0x100);
    }

    for (i = 0; i < 2; i++) {
        g_FieldFxTpageTable[i * 2 + 1] = GetTPage(i, 0, 0x340, 0x100);
    }
}


void FieldFx_LoadEffectTexturePage(int index) {
    RECT rect;
    void *pixels;

    rect.x = 0x380;
    rect.y = 0x100;
    rect.w = 0x40;
    rect.h = 0x100;

    if (index == 0) {
        pixels = g_FieldEffectTexturePage0;
    } else {
        pixels = g_FieldEffectTexturePage1;
    }

    LoadImage(&rect, pixels);
    g_FieldFxLoadedTexturePage = index;
}

void FieldFx_EnsureEffectTexturePage(int index) {
    RECT rect;
    void *pixels;

    if (g_FieldFxLoadedTexturePage != index) {
        rect.x = 0x380;
        rect.y = 0x100;
        rect.w = 0x40;
        rect.h = 0x100;

        if (index == 0) {
            pixels = g_FieldEffectTexturePage0;
        } else {
            pixels = g_FieldEffectTexturePage1;
        }

        LoadImage(&rect, pixels);
        g_FieldFxLoadedTexturePage = index;
    }
}

/* Matched field-shape quads. CPU-side matrix loads, depth scaling and packet
 * writes are C; each GTE instruction and hazard nop has its own macro.
 * Matching debt: pinned transfer registers/pointers and empty constraints,
 * including the template-copy endpoint used to preserve preheader order. */
void FieldFx_DrawShapeQuads(GteShortVector *position, GteRotation *rotation,
                   int scale_x, int scale_y, int texture, int clut,
                   int page, int intensity, RenderColor *color)
{
    FieldStripPacket template;
    GteVector scale;
    GteRotation level;
    GteMatrix matrix;
    s32 depth;
    s32 *depthOut;
    FieldStripPacket *packet;
    GteMatrix *view;
    GteShortVector *vertex;
    int r;
    register int g asm("$3");
    int b;
    int u, v;
    int width, height;
    int bias;
    int i;

    level = g_FieldFxZeroRotation;
    if (rotation == 0) {
        rotation = &level;
    }
    template.tag.length = 9;
    template.code = 0x2C;
    if (page == 0xFF) {
        template.code &= ~2;
        template.tpage = g_FieldFxSpriteParams.tpage;
    } else {
        template.code |= 2;
        width = GetTPage(0, page, 0, 0);
        template.tpage = g_FieldFxSpriteParams.tpage | width;
    }
    view = D_800BCFA4.value;
    template.clut = clut;
    if (color == 0) {
        g = intensity;
        b = g;
        r = g;
    } else {
        r = intensity * color->r / 128;
        g = intensity * color->g / 128;
        b = intensity * color->b / 128;
    }
    asm("" : : : "$5");
    template.r0 = r;
    template.g0 = g;
    template.b0 = b;
    if (g_FieldFxSpriteParams.parameter06 != 0) {
        u = texture & 0xF;
        v = 0;
        if (u >= 8) {
            u -= 8;
            v = 0x20;
        }
        u <<= 4;
        v += texture / 16 * 16;
    } else {
        u = (texture & 0xF) << 4;
        v = texture / 16 * 16;
    }
    if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag != 0) {
        v += 0x60;
    }
    width = g_FieldFxSpriteParams.extent_x;
    height = g_FieldFxSpriteParams.extent_y;
    asm("" : : "r"(scale_x), "r"(scale_y));
    scale.x = scale_x * (width >> 4);
    template.u0 = template.u2 = u;
    template.v1 = template.v0 = v;
    scale.y = scale_y * (height >> 4);
    template.v2 = template.v3 = v + height - 1;
    scale.z = 0x1000;
    template.u1 = template.u3 = u + width - 1;
    /* view is already pinned to $t0, which is the retail matrix base. */
    gte_ldrotmatrix((const GteMatrixWords *)view);
    gte_ldtransmatrix((const GteMatrixWords *)view);
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    packet = (FieldStripPacket *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += g_FieldFxStripPacketCounts[g_FieldFxSpriteParams.parameter0A] * sizeof(FieldStripPacket);
    bias = (u16)g_FieldFxSpriteParams.depth;
    RotMatrixYXZ(rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    gte_swc2_25_0(matrix.t);
    gte_swc2_26_4(matrix.t);
    gte_swc2_27_8(matrix.t);
    if (rotation->flags == 1) {
        MulRotMatrix(&matrix);
    }
    gte_ldrotmatrix((const GteMatrixWords *)&matrix);
    gte_ldtransmatrix((const GteMatrixWords *)&matrix);
    vertex = g_FieldFxShapeVertexTable[g_FieldFxSpriteParams.parameter0A];
    i = 0;
    if (i < g_FieldFxStripPacketCounts[g_FieldFxSpriteParams.parameter0A]) {
        {
            register void *copyEnd asm("$14") = &template.x3;
            asm("" : : "r"(copyEnd));
        }
        depthOut = &depth;
        for (;;) {
            {
                register GteShortVector *v1 = &vertex[1];
                register GteShortVector *v2 asm("$2") = &vertex[2];
                asm volatile("" : "=r"(v1) : "0"(v1));
                gte_lwc2_0_0(vertex);
                gte_lwc2_1_4(vertex);
                gte_lwc2_2_0(v1);
                gte_lwc2_3_4(v1);
                gte_lwc2_4_0(v2);
                gte_lwc2_5_4(v2);
            }
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtpt_command();
            *packet = template;
            {
                s16 *p0 = &packet->x0;
                s16 *p1 = &packet->x1;
                s16 *p2 = &packet->x2;
                gte_stsxy3(p0, p1, p2);
            }
                        gte_stmac0(depthOut);
            if (depth == 0) {
                break;
            }
            {
                gte_stszotz(depthOut);
            }
            {
                register GteShortVector *v3 = &vertex[3];
                                gte_lwc2_0_0(v3);
                gte_lwc2_1_4(v3);
            }
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtps_command();
            depth -= bias;
            if ((unsigned)depth < 0x1000) {
                FieldStripLink link;

                {
                    register s16 *p3 = &packet->x3;
                                        gte_stsxy2(p3);
                }
                packet->tag.address = STRIP_OT(depth)->address;
                link.tag = &packet->tag;
                STRIP_OT(depth)->address = link.word;
            }
            i++;
            packet++;
            vertex += 4;
            if (i >= g_FieldFxStripPacketCounts[g_FieldFxSpriteParams.parameter0A])
                break;
        }
    }
}

/* Field colour animation: colour-key interpolation and the rotating
 * 16-entry CLUT row upload. */


int StoreImage(RECT *rect, void *pixels);

void FieldFx_SampleColorTrack(void *key, void *color, int time)
{
    RenderColorTrack *track = (RenderColorTrack *)key;
    /* Matching debt: retain the retail count register across initialization. */
    register int count asm("$8") = track->count;
    int i, total, length;
    key = (unsigned char *)key + 8;
    if (!track->duration) {
        count = 0;
        total = 0;
        /* Walk the timing halfwords; the preceding byte is the encoded
         * duration. Retaining this cursor reproduces the retail accesses. */
        key = (unsigned char *)key + 4;
        for (;;) {
            length = ((unsigned char *)key)[-1];
            if (length) {
                ((RenderColorKeyTiming *)key)->start = total;
                total += length;
                ++count;
                ((RenderColorKeyTiming *)key)->length = length;
                key = (unsigned char *)key + 8;
            } else {
                key = (unsigned short *)(track + 1);
                track->duration = total;
                track->count = count;
                break;
            }
        }
    }
    if (time > track->duration) time = track->duration;
    key = (unsigned char *)key + (count * 8 - 8);
    for (i = 0; i < count; ++i) {
        if (time >= ((RenderColorKey *)key)->timing.start) break;
        key = (unsigned char *)key - 8;
    }
    total = ((RenderColorKey *)key)->timing.length;
    length = time - ((RenderColorKey *)key)->timing.start;
    length = (length << 12) / total;
    LoadAverageCol(key, (RenderColorKey *)key + 1, 4096 - length, length, color);
}

void FieldFx_LoadRotatedClutRow(int arg0, int arg1, u16 *pixels) {
    RECT rect;
    int i;
    u16 *dst;
    int magic;
    int source_offset;

    rect.x = (arg0 & 0xF) << 4;
    rect.y = g_FieldFxPaletteTable[D_800F336C] + (arg0 / 16);
    rect.w = 0x10;
    rect.h = 1;

    if (arg1 == -1) {
        StoreImage(&rect, pixels);
        return;
    }

    i = 1;
    magic = 0x88888889;
    dst = D_800E21A8;
    dst[0] = pixels[0];
    do {
        source_offset = (u16)i * 2;
        i++;
        dst[(arg1 % 15) + 1] = *(u16 *)(source_offset + (int)pixels);
        arg1++;
    } while ((unsigned int)(u16)i < 0x10U);

    LoadImage(&rect, dst);
}

/* Transform input by the current view matrix, then load the result into
 * the GTE translation registers, optionally returning it in output.
 * Matching debt: seven pins and two empty pointer constraints. Matrix
 * reads are C; every COP2 transfer/command and hazard nop is separate. */
void FieldFx_TransformTranslation(const GteShortVector *input,
                                  GteMatrixWords *output) {
    GteMatrixWords local;
    /* The slot address stays in $v0 and the loaded matrix in $v1, with the
     * load-delay nop between them. Folding the load deletes that addiu. */
    register GteMatrixWords *matrix asm("$3");
    GteMatrix **slot = &D_800BCFA4.value;

    asm volatile("" : "=r"(slot) : "0"(slot));
    matrix = (GteMatrixWords *)*slot;
        gte_ldrotmatrix(matrix);
    gte_ldtransmatrix(matrix);

    gte_lwc2_0_0(input);
    gte_lwc2_1_4(input);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();

    if (!output)
        output = &local;
    gte_swc2_25_0(&output->tx);
    gte_swc2_26_4(&output->tx);
    gte_swc2_27_8(&output->tx);

    gte_ldtransmatrix(output);
}

/* Build, optionally scale/compose, and install a rotation matrix.
 * Matching debt: three pinned word-transfer registers. Matrix reads are C;
 * each GTE control-register transfer uses its own instruction macro. */
void FieldFx_LoadRotationMatrix(GteRotation *rotation, s32 *scale, GteMatrix *matrix) {
    GteMatrix local;

    if (matrix == 0) {
        matrix = &local;
    }
    RotMatrixYXZ((GteShortVector *)rotation, matrix);
    if (scale != 0) {
        ScaleMatrix(matrix, (const GteVector *)scale);
    }
    if (rotation->flags != 0) {
        MulRotMatrix(matrix);
    }
    gte_ldrotmatrix((const GteMatrixWords *)matrix);
}

static __inline__ void link_packet(void *ordering, void *packet)
{
    FieldStripLink link;

    link.tag = packet;
    *(u32 *)packet = (*(u32 *)packet & 0xff000000) | (*(u32 *)ordering & 0xffffff);
    *(u32 *)ordering = (*(u32 *)ordering & 0xff000000) | (link.word & 0xffffff);
}
void FieldFx_LinkBlendedPacket(void *ordering, void *packet, int mode)
{
    void *drawMode;
    if (mode != 255) {
        drawMode = D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset;
        g_DrawPacketOffset += 8;
        SetDrawTPage(drawMode, 0, 1, GetTPage(0, mode, 0, 0));
        if (packet) {
            ((u8 *)packet)[7] |= 2;
            link_packet(ordering, packet);
        }
        link_packet(ordering, drawMode);
    } else if (packet) {
        link_packet(ordering, packet);
    }
}

/* Offset a point by a radius along the rotated Z axis and a roll-rotated X
 * arm, then restore the field camera matrix.
 * Matching debt: six register pins and three empty pointer barriers.
 * Matrix loads are C; GTE instructions and hazard nops use individual macros. */
void FieldFx_OffsetPointByAngles(GteShortVector *origin, GteShortVector *out, int radius,
                   GteShortVector *angles, int arm, int roll)
{
    GteShortVector forward;
    GteShortVector side;
    GteVector point;
    GteMatrix rotation;
    GteMatrix rollMatrix;
    GteMatrixWords *matrix;
    GteMatrixWords *rolled;
    register GteMatrixWords *camera asm("$8");
    GteMatrix **slot;
    GteShortVector *rotationAngles;

    rollMatrix = g_FieldFxIdentityMatrix;
    forward.y = 0;
    forward.x = 0;
    forward.z = radius;
    rotationAngles = angles;
    matrix = (GteMatrixWords *)&rotation;
    RotMatrixYXZ(rotationAngles, (GteMatrix *)matrix);
        gte_ldrotmatrix(matrix);
    rotation.t[0] = 0;
    rotation.t[1] = 0;
    rotation.t[2] = 0;
    gte_ldtransmatrix(matrix);
    gte_lwc2_0_0(&forward);
    gte_lwc2_1_4(&forward);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtv0tr_sf0();
    side.z = 0;
    side.y = 0;
    side.x = arm;
    gte_swc2_25_0(rotation.t);
    gte_swc2_26_4(rotation.t);
    gte_swc2_27_8(rotation.t);
    rolled = (GteMatrixWords *)&rollMatrix;
    RotMatrixZ(roll, (GteMatrix *)rolled);
    MulRotMatrix((GteMatrix *)rolled);
        gte_ldrotmatrix(rolled);
    gte_ldtransmatrix(matrix);
    gte_lwc2_0_0(&side);
    gte_lwc2_1_4(&side);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtv0tr_sf0();
    gte_swc2_25_0(&point);
    gte_swc2_26_4(&point);
    gte_swc2_27_8(&point);
    slot = &D_800BCFA4.value;
    asm volatile("" : "=r"(slot) : "0"(slot));
    camera = (GteMatrixWords *)*slot;
    gte_ldrotmatrix(camera);
    gte_ldtransmatrix(camera);
    point.x += origin->x;
    point.y += origin->y;
    point.z += origin->z;
    out->x = point.x;
    out->y = point.y;
    out->z = point.z;
}

void FieldFx_CalculateLookAngles(GteShortVector *from, GteShortVector *to,
                                 GteShortVector *out) {
    int dx, dz, horizontal_distance;

    dz = to->z - from->z;
    dx = to->x - from->x;
    out->y = -ratan2(dz, dx) + 1024;
    horizontal_distance = SquareRoot0((unsigned)dx * dx + (unsigned)dz * dz);
    out->x = -ratan2(to->y - from->y, horizontal_distance);
    out->z = 0;
    out->x &= 4095;
    out->y &= 4095;
}

/* Rotates (0, 0, distance) by the YXZ angles in `input` (roll cleared) and
 * restores the shared camera matrix afterwards. The halfword distance
 * parameter makes the callee narrow it, which lets sched1 move the argument
 * copies below the constant vector copy as retail does.
 * Matching debt: four register pins and two empty constraints preserve
 * pointer allocation and ordering. All matrix loads are C; each GTE transfer,
 * command and hazard nop is an individual macro. */
void FieldFx_AnglesToVector(GteShortVector *input, s16 distance, GteShortVector *out)
{
    GteShortVector rotation;
    GteShortVector direction = g_FieldFxZeroVector;
    GteVector result;
    GteMatrixWords local;
    GteMatrixWords *matrix;
    const GteMatrixWords *saved = (const GteMatrixWords *)D_800BCFA4.value;

        matrix = &local;
    input->z = 0;
    rotation.x = input->x;
    rotation.y = input->y;
    rotation.z = input->z;
    RotMatrixYXZ(&rotation, (GteMatrix *)matrix);
        gte_ldrotmatrix(matrix);
    local.tx = 0;
    local.ty = 0;
    local.tz = 0;
    gte_ldtransmatrix(matrix);
    direction.z = distance;
    gte_lwc2_0_0(&direction);
    gte_lwc2_1_4(&direction);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    gte_swc2_25_0(&result);
    gte_swc2_26_4(&result);
    gte_swc2_27_8(&result);
    gte_ldrotmatrix(saved);
    gte_ldtransmatrix(saved);
    out->x = result.x;
    out->y = result.y;
    out->z = result.z;
}

/* Direction helpers for field effects: shortest turn between two angles,
 * eased turning, point distance and the alternating scatter offsets. */


int FieldFx_GetTurnDirection(int from, int to, s16 *distance)
{
  unsigned char new_var;
  register int direction;
  register int delta;
  register int direction16;
  int diff;
  from &= 0xFFF;
  to &= 0xFFF;
  direction = -1;
  if (((unsigned int) to) < ((unsigned int) from))
  {
    direction = 1;
  }
  diff = from - to;
  new_var = diff < 0;
  delta = diff;
  if (new_var)
  {
    delta = -delta;
  }
  if (delta >= 0x801)
  {
    direction16 = (s16) direction;
    direction = -direction16;
    delta = 0x1000 - delta;
  }
  if (distance != 0)
  {
    *distance = delta;
  }
  return (s16) direction;
}

/* Return the shortest turn direction and write its unsigned angle distance. */
static inline int angle_direction(u16 *fromPtr, u16 *toPtr, s16 *distance) {
    int from = *fromPtr & 0xFFF;
    int to = *toPtr & 0xFFF;
    u8 negative;
    int direction;
    int delta;
    int direction16;
    int diff;

    direction = -1;
    if ((unsigned int)to < (unsigned int)from) direction = 1;
    diff = from - to;
    negative = diff < 0;
    delta = diff;
    if (negative) delta = -delta;
    if (delta >= 0x801) {
        direction16 = (s16)direction;
        direction = -direction16;
        delta = 0x1000 - delta;
    }
    *distance = delta;
    return (s16)direction;
}

void FieldFx_EaseAngles(u16 *from, u16 *to, u16 speed) {
    s16 distance;
    int direction;
    int scaled;
    u16 old0, old1;
    s16 change;

    direction = angle_direction(&from[0], &to[0], &distance);
    scaled = distance * speed;
    if (scaled < 0) scaled += 0xFFF;
    old0 = to[0];
    distance = scaled >> 12;
    change = distance * direction;
    to[0] = old0 + change;

    direction = angle_direction(&from[1], &to[1], &distance);
    scaled = distance * speed;
    if (scaled < 0) scaled += 0xFFF;
    old1 = to[1];
    distance = scaled >> 12;
    change = distance * direction;
    to[1] = old1 + change;
}

int FieldFx_GetDistanceNonZero(s16 *from, s16 *to)
{
    int dx = to[0] - from[0];
    int dy = to[1] - from[1];
    int dz = to[2] - from[2];
    int length = SquareRoot0((dx * dx) + (dy * dy) + (dz * dz));

    if (length == 0) {
        length = 1;
    }

    return length;
}

void FieldFx_RandomScatterOffsetYZ(s16 *out)
{
    s16 *out_reg;
    int y;
    int phase;
    int temp;
    int z;

    out_reg = out;
    g_FieldFxRandomPhaseCounter = (g_FieldFxRandomPhaseCounter + 1) & 7;
    temp = rand() & 0x1FF;
    phase = g_FieldFxRandomPhaseCounter;
    temp += (phase << 9) & 0xC00;
    phase &= 1;
    y = temp + 0x100;
    if ((phase & 1) != 0) {
        z = (rand() & 0x1FF) + 0x100;
    } else {
        z = -(rand() & 0x1FF) - 0x100;
    }

    out_reg[0] = 0;
    out_reg[1] = y;
    out_reg[2] = z;
}

void FieldFx_RandomScatterOffsetXY(s16 *out)
{
    s16 *out_reg;
    int x;
    int phase;
    int temp;
    int y;

    out_reg = out;
    g_FieldFxRandomPhaseCounter = (g_FieldFxRandomPhaseCounter + 1) & 7;
    temp = rand() & 0x1FF;
    phase = g_FieldFxRandomPhaseCounter;
    temp += (phase << 9) & 0xC00;
    phase &= 1;
    x = temp + 0x100;
    if ((phase & 1) != 0) {
        y = (rand() & 0x1FF) + 0x100;
    } else {
        y = -(rand() & 0x1FF) - 0x100;
    }

    out_reg[0] = x;
    out_reg[1] = y;
    out_reg[2] = 0;
}

/* `segments` triangles around `position`: the rim points alternate between
 * radius `width` and `height`, color0 shades the centre and color1 the
 * rim at intensity/128; a nonzero rotation->flags also applies the view
 * rotation. Drawing stops at the first triangle beyond the depth range.
 * Matching debt: twelve register pins and five empty constraints. CPU
 * matrix loads and depth arithmetic are C; each GTE instruction is separate. */
void FieldFx_DrawStarFan(GteShortVector *position, int width, int height, int segments,
                   GteRotation *rotation, int scale_x, int scale_y,
                   RenderColor *color0, RenderColor *color1, int intensity,
                   int mode)
{
    FieldG3Packet template;
    s16 radii[2];
    GteShortVector vertices[3];
    GteShortVector anchor;
    GteShortVector defaultRotation = g_FieldFxZeroVector;
    GteVector scale = g_FieldFxUnitScale;
    RenderColor centre;
    RenderColor rim;
    GteMatrix matrix;
    u32 depth;
    GteMatrix *view;
    FieldG3Packet *packet;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    int bias;
    int i;
    int angle;
    int next;
    int lastY;

    if (segments < 4)
        return;
    bias = (u16)D_800F3374;
    view = D_800BCFA4.value;
    /* Preserve position/color allocation without pinning the arguments. */
        if (color0 == 0) {
        centre.r = centre.g = centre.b = 0;
    } else {
        centre.r = color0->r * intensity / 128;
        centre.g = color0->g * intensity / 128;
        centre.b = color0->b * intensity / 128;
    }
    if (color1 == 0) {
        rim.r = rim.g = rim.b = 0;
    } else {
        rim.r = color1->r * intensity / 128;
        rim.g = color1->g * intensity / 128;
        rim.b = color1->b * intensity / 128;
    }
    gte_ldrotmatrix((const GteMatrixWords *)view);
    gte_ldtransmatrix((const GteMatrixWords *)view);
    anchor.x = position->x;
    anchor.y = position->y;
    anchor.z = position->z;
    gte_lwc2_0_0(&anchor);
    gte_lwc2_1_4(&anchor);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    if (rotation == 0)
        rotation = (GteRotation *)&defaultRotation;
    scale.x = scale_x;
    scale.y = scale_y;
    {
        s32 *translation = matrix.t;
        gte_swc2_25_0(translation);
        gte_swc2_26_4(translation);
        gte_swc2_27_8(translation);
    }
    RotMatrixYXZ((GteShortVector *)rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    if (rotation->flags)
        MulRotMatrix(&matrix);
    gte_ldrotmatrix((const GteMatrixWords *)&matrix);
    gte_ldtransmatrix((const GteMatrixWords *)&matrix);
    SetPolyG3(&template);
    template.r0 = centre.r;
    template.g0 = centre.g;
    template.b0 = centre.b;
    template.r2 = template.r1 = rim.r;
    template.g2 = template.g1 = rim.g;
    template.b2 = template.b1 = rim.b;
    packet = (FieldG3Packet *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += segments * sizeof(FieldG3Packet);
    vertices[0].x = vertices[0].y = vertices[0].z = vertices[1].z = vertices[2].z = 0;
    radii[0] = width;
    radii[1] = height;
    for (i = 0; i < segments; i++, packet++) {
        /* Prevent a separate induction pointer for packet->code. */
        angle = (i << 12) / segments;
        next = ((i + 1) << 12) / segments;
        vertices[1].x = rcos(angle) * radii[i & 1] / 4096;
        vertices[1].y = rsin(angle) * radii[i & 1] / 4096;
        vertices[2].x = rcos(next) * radii[(i + 1) & 1] / 4096;
        lastY = rsin(next) * radii[(i + 1) & 1] / 4096;
        /* Keep vertex-address setup after the final coordinate calculation. */
        asm volatile("" : : : "$4");
        {
            register GteShortVector *v0 asm("$4");
            register GteShortVector *v1 asm("$3");
            GteShortVector *v2;
            v0 = &vertices[0];
            v1 = &vertices[1];
            vertices[2].y = lastY;
            v2 = &vertices[2];
            gte_lwc2_0_0(v0);
            gte_lwc2_1_4(v0);
            gte_lwc2_2_0(v1);
            gte_lwc2_3_4(v1);
            gte_lwc2_4_0(v2);
            gte_lwc2_5_4(v2);
        }
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtpt_command();
        *packet = template;
        {
            s16 *xy0 = &packet->x0;
            s16 *xy1 = &packet->x1;
            s16 *xy2 = &packet->x2;
            gte_stsxy3(xy0, xy1, xy2);
        }
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_avsz3_command();
        {
            register s32 z asm("$12");
            u32 *out = &depth;
            asm volatile("" : "=r"(out) : "0"(out));
            gte_getsz3(z);
            gte_cop2_hazard_slot();
            *out = z >> 2;
        }
        depth -= bias;
        if (depth >= 0x1000)
            return;
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[g_ActiveDrawSlot], depth);
        if (mode != 0xFF) {
            drawMode = (RenderTintMode *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
            g_DrawPacketOffset += sizeof(RenderTintMode);
            SetDrawTPage((char *)drawMode, 0, 1, GetTPage(0, mode, 0, 0));
            if (packet) {
                packet->code |= 2;
                TILE_OT_ADDPRIM(ot, packet, link);
            }
            TILE_OT_ADDPRIM(ot, drawMode, link);
        } else if (packet) {
            TILE_OT_ADDPRIM(ot, packet, link);
        }
    }
}

/* `segments` quads around `position` between radius `inner` (shaded by
 * color0) and radius `outer` (shaded by color1) at intensity/128; a
 * nonzero rotation->flags also applies the view rotation. Drawing stops at
 * the first quad beyond the depth range.
 * Matching debt: twelve register pins and four empty constraints. CPU
 * matrix loads and depth arithmetic are C; each GTE instruction is separate. */
void FieldFx_DrawRingBand(GteShortVector *position, int inner, int outer, int segments,
                   GteRotation *rotation, int scale_x, int scale_y,
                   struct RenderColor *color0, struct RenderColor *color1,
                   int intensity, int mode)
{
    FieldG4Packet template;
    GteShortVector vertices[4];
    GteShortVector anchor;
    GteShortVector defaultRotation = g_FieldFxZeroVector;
    GteVector scale = g_FieldFxUnitScale;
    RenderColor innerColor;
    RenderColor outerColor;
    GteMatrix matrix;
    u32 depth;
    GteMatrix *view;
    FieldG4Packet *packet;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    RenderGpuTag *entry;
    int bias;
    int i;
    int angle;
    int next;

    if (segments < 4)
        return;
    bias = (u16)D_800F3374;
    view = D_800BCFA4.value;
    /* Preserve the position/color register allocation without argument pins. */
        if (color0 == 0) {
        innerColor.r = innerColor.g = innerColor.b = 0;
    } else {
        innerColor.r = color0->r * intensity / 128;
        innerColor.g = color0->g * intensity / 128;
        innerColor.b = color0->b * intensity / 128;
    }
    if (color1 == 0) {
        outerColor.r = outerColor.g = outerColor.b = 0;
    } else {
        outerColor.r = color1->r * intensity / 128;
        outerColor.g = color1->g * intensity / 128;
        outerColor.b = color1->b * intensity / 128;
    }
    gte_ldrotmatrix((const GteMatrixWords *)view);
    gte_ldtransmatrix((const GteMatrixWords *)view);
    anchor.x = position->x;
    anchor.y = position->y;
    anchor.z = position->z;
    gte_lwc2_0_0(&anchor);
    gte_lwc2_1_4(&anchor);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    if (rotation == 0)
        rotation = (GteRotation *)&defaultRotation;
    scale.x = scale_x;
    scale.y = scale_y;
    {
        s32 *translation = matrix.t;
        gte_swc2_25_0(translation);
        gte_swc2_26_4(translation);
        gte_swc2_27_8(translation);
    }
    RotMatrixYXZ((GteShortVector *)rotation, &matrix);
    if (rotation->flags)
        MulRotMatrix(&matrix);
    ScaleMatrix(&matrix, &scale);
    gte_ldrotmatrix((const GteMatrixWords *)&matrix);
    gte_ldtransmatrix((const GteMatrixWords *)&matrix);
    SetPolyG4(&template);
    template.r1 = template.r0 = outerColor.r;
    template.g1 = template.g0 = outerColor.g;
    template.b1 = template.b0 = outerColor.b;
    template.r2 = innerColor.r;
    template.g2 = innerColor.g;
    template.b2 = innerColor.b;
    template.r3 = innerColor.r;
    template.g3 = innerColor.g;
    template.b3 = innerColor.b;
    packet = (FieldG4Packet *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += segments * sizeof(FieldG4Packet);
    vertices[0].z = vertices[1].z = vertices[2].z = vertices[3].z = 0;
    for (i = 0; i < segments; i++, packet++) {
        /* Prevent a separate induction pointer for packet->code. */
        angle = (i << 12) / segments;
        next = ((i + 1) << 12) / segments;
        vertices[0].x = rcos(angle) * outer / 4096;
        vertices[0].y = rsin(angle) * outer / 4096;
        vertices[1].x = rcos(next) * outer / 4096;
        vertices[1].y = rsin(next) * outer / 4096;
        vertices[2].x = rcos(angle) * inner / 4096;
        vertices[2].y = rsin(angle) * inner / 4096;
        vertices[3].x = rcos(next) * inner / 4096;
        vertices[3].y = rsin(next) * inner / 4096;
        {
            register GteShortVector *v0 asm("$4");
            register GteShortVector *v1 asm("$3");
            GteShortVector *v2;
            v0 = &vertices[0];
            v1 = &vertices[1];
            v2 = &vertices[2];
            gte_lwc2_0_0(v0);
            gte_lwc2_1_4(v0);
            gte_lwc2_2_0(v1);
            gte_lwc2_3_4(v1);
            gte_lwc2_4_0(v2);
            gte_lwc2_5_4(v2);
        }
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtpt_command();
        *packet = template;
        {
            s16 *xy0 = &packet->x0;
            s16 *xy1 = &packet->x1;
            s16 *xy2 = &packet->x2;
            gte_stsxy3(xy0, xy1, xy2);
        }
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_avsz3_command();
        {
            u32 *out = &depth;
            gte_stszotz(out);
        }
        {
            GteShortVector *last = &vertices[3];
            gte_lwc2_0_0(last);
            gte_lwc2_1_4(last);
        }
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtps_command();
        depth -= bias;
        if (depth >= 0x1000)
            return;
        gte_stsxy2(&packet->x3);
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[g_ActiveDrawSlot], depth);
        entry = ot.tag;
        if (mode != 0xFF) {
            drawMode = (RenderTintMode *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
            g_DrawPacketOffset += sizeof(RenderTintMode);
            SetDrawTPage((char *)drawMode, 0, 1, GetTPage(0, mode, 0, 0));
            if (packet) {
                packet->code |= 2;
                RING_OT_ADDPRIM(entry, packet, link);
            }
            RING_OT_ADDPRIM(entry, drawMode, link);
        } else if (packet) {
            RING_OT_ADDPRIM(entry, packet, link);
        }
    }
}

/* Triangle with its apex at `position` and a `breadth` wide base `length`
 * away along local Z, turned by `rotation` and shaded per corner from
 * color0..color2 at intensity/128; mode 0xFF draws it opaque, anything
 * else adds a semi-transparent draw mode.
 * The camera and local matrix uploads are gte_ldrotmatrix and
 * gte_ldtransmatrix; the corners go to the GTE through gte_ldv3. */
void FieldFx_DrawRotatedTriangle(GteShortVector *position, GteRotation *rotation, int length,
                   int breadth, RenderColor *color0, RenderColor *color1,
                   RenderColor *color2, int intensity, int mode)
{
    GteShortVector corners[3];
    RenderColor black;
    GteMatrix matrix;
    u32 depth;
    FieldG3Packet *packet;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    GteMatrix *view;
    int bias;

    black = g_FieldFxBlackColor;
    bias = (u16)g_FieldFxSpriteParams.depth;
    /* Keep intensity in s6 without pinning the later draw-mode lifetime. */
    
    packet = (FieldG3Packet *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += sizeof(FieldG3Packet);
    view = D_800BCFA4.value;
    {
        const GteMatrixWords *words = (const GteMatrixWords *)(view);
        gte_ldrotmatrix(words);
        gte_ldtransmatrix(words);
    }
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    corners[1].x = -breadth / 2;
    corners[1].y = corners[2].y = 0;
    corners[0].x = corners[0].y = corners[0].z = 0;
    corners[1].z = corners[2].z = length;
    corners[2].x = breadth / 2;
    {
        s32 *translation = matrix.t;
        gte_swc2_25_0(translation);
        gte_swc2_26_4(translation);
        gte_swc2_27_8(translation);
    }
    RotMatrixYXZ((GteShortVector *)rotation, &matrix);
    MulRotMatrix(&matrix);
    {
        const GteMatrixWords *words = (const GteMatrixWords *)(&matrix);
        gte_ldrotmatrix(words);
        gte_ldtransmatrix(words);
    }
    gte_ldv3(&corners[0], &corners[1], &corners[2]);
    gte_rtpt_padded();
    SetPolyG3(packet);
    if (color0 == 0) {
        color0 = &black;
    }
    packet->r0 = color0->r * (s16)intensity / 128;
    packet->g0 = color0->g * (s16)intensity / 128;
    packet->b0 = color0->b * (s16)intensity / 128;
    if (color1 == 0) {
        color1 = &black;
    }
    packet->r1 = color1->r * (s16)intensity / 128;
    packet->g1 = color1->g * (s16)intensity / 128;
    packet->b1 = color1->b * (s16)intensity / 128;
    if (color2 == 0) {
        color2 = &black;
    }
    packet->r2 = color2->r * (s16)intensity / 128;
    packet->g2 = color2->g * (s16)intensity / 128;
    packet->b2 = color2->b * (s16)intensity / 128;
    gte_stmac0(&depth);
    if (depth != 0) {
        gte_stsxy3(&packet->x0, &packet->x1, &packet->x2);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_avsz3_command();
        gte_stszotz(&depth);
        depth -= bias;
        if (depth < 0x1000) {
            TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[g_ActiveDrawSlot], depth);
            if (mode != 0xFF) {
                drawMode = (RenderTintMode *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
                g_DrawPacketOffset += sizeof(RenderTintMode);
                SetDrawTPage(drawMode, 0, 1, GetTPage(0, mode, 0, 0));
                if (packet) {
                    packet->code |= 2;
                    TILE_OT_ADDPRIM(ot, packet, link);
                }
                TILE_OT_ADDPRIM(ot, drawMode, link);
            } else if (packet) {
                TILE_OT_ADDPRIM(ot, packet, link);
            }
        }
    }
}

/* Push the pair (head, tail) onto the `count`-deep history and draw the
 * ribbon through it: quad i joins pairs i and i+1 and fades from
 * (count - i)/count to (count - i - 1)/count of color0 (head side) and
 * color1 (tail side) at scale/128. A missing head repeats the previous
 * pair; count >= 1000 instead clears the first count - 999 pairs. Mode
 * 0xFF draws opaque, anything else semi-transparent.
 * The camera upload is gte_ldrotmatrix and gte_ldtransmatrix. */
void FieldFx_DrawHistoryTrail(GteShortVector *head, GteShortVector *tail, u32 count,
                   u8 *color0, u8 *color1, int scale, FieldTrailPair *history,
                   int mode)
{
    u8 headColor[3];
    u8 tailColor[3];
    u32 depth;
    FieldG4Packet *packet;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    GteMatrix *view;
    int bias;
    u32 i;
    u32 last;
    FieldTrailPair *pair;
    s16 fade0;
    s16 fade1;

    view = D_800BCFA4.value;
    bias = (u16)g_FieldFxSpriteParams.depth;
    if (count >= 1000) {
        count -= 999;
        for (i = 0; i < count; i++) {
            history[i].head.vector.pad = 0;
            history[i].tail.vector.pad = 0;
        }
        return;
    }
    last = count - 1;
    pair = &history[last];
    for (i = 0; i < count; i++) {
        pair[1].head.words[0] = pair[0].head.words[0];
        pair[1].head.words[1] = pair[0].head.words[1];
        pair[1].tail.words[0] = pair[0].tail.words[0];
        pair[1].tail.words[1] = pair[0].tail.words[1];
        pair--;
    }
    if (head != 0) {
        history[0].head.vector.x = head->x;
        history[0].head.vector.y = head->y;
        history[0].head.vector.z = head->z;
        history[0].head.vector.pad = 1;
        history[0].tail.vector.x = tail->x;
        history[0].tail.vector.y = tail->y;
        history[0].tail.vector.z = tail->z;
        history[0].tail.vector.pad = 1;
    } else {
        history[0].head.vector.x = history[1].head.vector.x;
        history[0].head.vector.y = history[1].head.vector.y;
        history[0].head.vector.z = history[1].head.vector.z;
        history[0].tail.vector.x = history[1].tail.vector.x;
        history[0].tail.vector.y = history[1].tail.vector.y;
        history[0].tail.vector.z = history[1].tail.vector.z;
    }
    packet = (FieldG4Packet *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += count * sizeof(FieldG4Packet);
    if (color0 == 0) {
        headColor[0] = headColor[1] = headColor[2] = 0;
    } else {
        headColor[0] = color0[0] * scale / 128;
        headColor[1] = color0[1] * scale / 128;
        headColor[2] = color0[2] * scale / 128;
    }
    if (color1 == 0) {
        tailColor[0] = tailColor[1] = tailColor[2] = 0;
    } else {
        tailColor[0] = color1[0] * scale / 128;
        tailColor[1] = color1[1] * scale / 128;
        tailColor[2] = color1[2] * scale / 128;
    }
    gte_ldrotmatrix(view);
    gte_ldtransmatrix(view);
    for (i = 0; i < count; i++, history++, packet++) {
        if (history[0].head.vector.pad & history[0].tail.vector.pad & history[1].head.vector.pad &
            history[1].tail.vector.pad) {
            gte_ldv3(&history[0].head.vector, &history[0].tail.vector, &history[1].head.vector);
            gte_rtpt_padded();
            gte_stmac0(&depth);
            if (depth == 0) {
                return;
            }
            gte_stsxy3(&packet->x0, &packet->x1, &packet->x2);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_avsz3_command();
            gte_stszotz(&depth);
            depth -= bias;
            if (depth >= 0x1000) {
                return;
            }
            gte_ldv0(&history[1].tail.vector);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtps_command();
            SetPolyG4(packet);
            gte_stsxy2(&packet->x3);
            fade0 = 128 - i * 128 / count;
            fade1 = 128 - (i + 1) * 128 / count;
            packet->r0 = headColor[0] * fade0 / 128;
            packet->g0 = headColor[1] * fade0 / 128;
            packet->b0 = headColor[2] * fade0 / 128;
            packet->r1 = tailColor[0] * fade0 / 128;
            packet->g1 = tailColor[1] * fade0 / 128;
            packet->b1 = tailColor[2] * fade0 / 128;
            packet->r2 = headColor[0] * fade1 / 128;
            packet->g2 = headColor[1] * fade1 / 128;
            packet->b2 = headColor[2] * fade1 / 128;
            packet->r3 = tailColor[0] * fade1 / 128;
            packet->g3 = tailColor[1] * fade1 / 128;
            packet->b3 = tailColor[2] * fade1 / 128;
            TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[g_ActiveDrawSlot], depth);
            if (mode != 0xFF) {
                drawMode = (RenderTintMode *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
                g_DrawPacketOffset += sizeof(RenderTintMode);
                SetDrawTPage(drawMode, 0, 1, GetTPage(0, mode, 0, 0));
                if (packet) {
                    packet->code |= 2;
                    TILE_OT_ADDPRIM(ot, packet, link);
                }
                TILE_OT_ADDPRIM(ot, drawMode, link);
            } else if (packet) {
                TILE_OT_ADDPRIM(ot, packet, link);
            }
        }
    }
}

/* Full-screen 320x240 tile in `color` scaled by scale/128, linked at
 * `depth`; any blend mode other than 0xFF adds a semi-transparent draw mode. */
void FieldFx_DrawScreenTint(u8 *color, int scale, int abr, u32 depth)
{
    RenderTintTile *tile;
    RenderTintMode *mode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;

    tile = (RenderTintTile *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += sizeof(RenderTintTile);
    SetTile(tile);
    tile->r = color[0] * scale / 128;
    tile->g = color[1] * scale / 128;
    tile->b = color[2] * scale / 128;
    if (depth < 0x1000) {
        tile->x = 0;
        tile->y = 0;
        tile->width = 320;
        tile->height = 240;
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[g_ActiveDrawSlot], depth);
        if (abr != 0xFF) {
            mode = (RenderTintMode *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
            g_DrawPacketOffset += sizeof(RenderTintMode);
            SetDrawTPage(mode, 0, 1, GetTPage(0, abr, 0, 0));
            if (tile) {
                tile->command |= 2;
                TILE_OT_ADDPRIM(ot, tile, link);
            }
            TILE_OT_ADDPRIM(ot, mode, link);
        } else if (tile) {
            TILE_OT_ADDPRIM(ot, tile, link);
        }
    }
}

int Geo_ClipAlternatingArcPoint(int radius, int divisor, int segment) {
    int angle = (segment << 10) / divisor;
    int direction = ((segment & 1) << 1) - 1;
    int product = rcos(angle) * radius;
    int y;
    GeomState *state;

    if (product < 0) {
        product += 0xFFF;
    }
    product >>= 12;
    y = product * direction;

    state = g_GeomState;
    state->clip_min_x = state->clip_min_y = -128;
    state = g_GeomState;
    state->clip_max_x = state->clip_max_y = 128;

    return Render_ClipPoint(0, (s16)y, 0);
}

/* Projected one-pixel tile plus a 3x3 halo at a quarter of its colour. */
void FieldFx_DrawHaloedPoint(void *position, void *colorArg, int scale, int abr)
{
    u8 *color = colorArg;
    FieldTilePoint *point;
    RenderTintTile *glow;
    RenderTintMode *mode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    u32 depth;

    point = (FieldTilePoint *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += sizeof(FieldTilePoint);
    glow = (RenderTintTile *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += sizeof(RenderTintTile);
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    SetTile1(point);
    SetSemiTrans(point, 1);
    SetTile(glow);
    glow->r = point->r = color[0] * scale / 128;
    glow->g = point->g = color[1] * scale / 128;
    glow->b = point->b = color[2] * scale / 128;
    glow->r >>= 2;
    glow->g >>= 2;
    glow->b >>= 2;
    gte_stsxy2(&point->x);
    gte_stszotz(&depth);
    depth -= (u16)g_FieldFxSpriteParams.depth;
    if (depth < 0x1000) {
        glow->x = point->x - 1;
        glow->y = point->y - 1;
        glow->height = 3;
        glow->width = 3;
        AddPrim((u32 *)D_800B0E38.ordering[g_ActiveDrawSlot] + depth, (u32 *)point);
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[g_ActiveDrawSlot], depth);
        if (abr != 0xFF) {
            mode = (RenderTintMode *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
            g_DrawPacketOffset += sizeof(RenderTintMode);
            SetDrawTPage(mode, 0, 1, GetTPage(0, abr, 0, 0));
            if (glow) {
                glow->command |= 2;
                TILE_OT_ADDPRIM(ot, glow, link);
            }
            TILE_OT_ADDPRIM(ot, mode, link);
        } else if (glow) {
            TILE_OT_ADDPRIM(ot, glow, link);
        }
    }
}

/* One-pixel tile at the projected `position` in `color` scaled by
 * scale/128, linked at its view depth; any blend mode other than 0xFF adds a
 * semi-transparent draw mode. */
void FieldFx_DrawPointTile(GteShortVector *position, u8 *color, int scale, int abr)
{
    FieldTilePoint *tile;
    RenderTintMode *mode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    u32 depth;

    tile = (FieldTilePoint *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += sizeof(FieldTilePoint);
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    SetTile1(tile);
    tile->r = color[0] * scale / 128;
    tile->g = color[1] * scale / 128;
    tile->b = color[2] * scale / 128;
    gte_stsxy2(&tile->x);
    {
        s32 *out = (s32 *)&depth;
        gte_stszotz(out);
    }
    depth -= (u16)g_FieldFxSpriteParams.depth;
    if (depth < 0x1000) {
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[g_ActiveDrawSlot], depth);
        if (abr != 0xFF) {
            mode = (RenderTintMode *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
            g_DrawPacketOffset += sizeof(RenderTintMode);
            SetDrawTPage(mode, 0, 1, GetTPage(0, abr, 0, 0));
            if (tile) {
                tile->command |= 2;
                TILE_OT_ADDPRIM(ot, tile, link);
            }
            TILE_OT_ADDPRIM(ot, mode, link);
        } else if (tile) {
            TILE_OT_ADDPRIM(ot, tile, link);
        }
    }
}

/* Rectangle `length` deep and `breadth` wide in the local XZ plane at
 * `position`, turned by `rotation`, gouraud shaded from color0 (near edge)
 * to color1 (far edge) at intensity/128 and textured with the cell at
 * (u, v); mode 0xFF draws it opaque, anything else semi-transparent.
 * The camera and local matrix uploads are gte_ldrotmatrix and
 * gte_ldtransmatrix. Matching debt: one empty constraint on rotation and
 * length (see docs/CODE_ORGANIZATION.md). */
void FieldFx_DrawRotatedQuad(GteShortVector *position, GteRotation *rotation,
                   int length, int breadth, int u, int v, int texture_width,
                   int texture_height, int clut, RenderColor *color0,
                   RenderColor *color1, int intensity, int mode)
{
    GteShortVector corners[4];
    RenderColor black;
    GteMatrix matrix;
    u32 depth;
    FieldGt4Packet *packet;
    s16 scale;
    int bias;
    GteMatrix *view;

    view = D_800BCFA4.value;
    scale = intensity;
    bias = (u16)g_FieldFxSpriteParams.depth;
    black = g_FieldFxBlackColor;
    packet = (FieldGt4Packet *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += sizeof(FieldGt4Packet);
    SetPolyGT4(packet);
    if (mode == 0xFF) {
        packet->code &= ~2;
        packet->tpage = g_FieldFxSpriteParams.tpage;
    } else {
        packet->code |= 2;
        packet->tpage = g_FieldFxSpriteParams.tpage | GetTPage(0, mode, 0, 0);
    }
    /* Keep rotation and length in registers across the packet setup calls. */
    asm("" : : "r"(rotation), "r"(length));
    packet->clut = clut;
    gte_ldrotmatrix(view);
    gte_ldtransmatrix(view);
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    breadth /= 2;
    corners[0].x = corners[2].x = -breadth;
    corners[1].x = corners[3].x = breadth;
    corners[2].z = corners[3].z = length;
    corners[0].y = corners[1].y = corners[2].y = corners[3].y = 0;
    corners[0].z = corners[1].z = 0;
    {
        s32 *translation = matrix.t;
        gte_swc2_25_0(translation);
        gte_swc2_26_4(translation);
        gte_swc2_27_8(translation);
    }
    RotMatrixYXZ((GteShortVector *)rotation, &matrix);
    MulRotMatrix(&matrix);
    {
        const GteMatrixWords *words = (const GteMatrixWords *)(&matrix);
        gte_ldrotmatrix(words);
        gte_ldtransmatrix(words);
    }
    gte_ldv3(&corners[0], &corners[1], &corners[2]);
    gte_rtpt_padded();
    if (color0 == 0) {
        color0 = &black;
    }
    packet->r0 = packet->r1 = color0->r * scale / 128;
    packet->g0 = packet->g1 = color0->g * scale / 128;
    packet->b0 = packet->b1 = color0->b * scale / 128;
    if (color1 == 0) {
        color1 = &black;
    }
    packet->r2 = packet->r3 = color1->r * scale / 128;
    packet->g2 = packet->g3 = color1->g * scale / 128;
    packet->b2 = packet->b3 = color1->b * scale / 128;
    gte_stmac0(&depth);
    if (depth != 0) {
        {
            s16 *xy0 = &packet->x0;
            s16 *xy1 = &packet->x1;
            s16 *xy2 = &packet->x2;
            gte_stsxy3(xy0, xy1, xy2);
        }
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_avsz3_command();
        {
            u32 *out = &depth;
            gte_stszotz(out);
        }
        depth -= bias;
        if (depth < 0x1000) {
            {
                GteShortVector *last = &corners[3];
                gte_lwc2_0_0(last);
                gte_lwc2_1_4(last);
            }
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtps_command();
            packet->u0 = packet->u1 = u;
            packet->u2 = packet->u3 = u + texture_width - 1;
            packet->v0 = packet->v2 = v;
            packet->v1 = packet->v3 = v + texture_height - 1;
            gte_stsxy2(&packet->x3);
            AddPrim((unsigned int *)D_800B0E38.ordering[g_ActiveDrawSlot] + depth,
                    (unsigned int *)packet);
        }
    }
}

/* Haloed point at distance `x` from the view origin rotated by `y` about Z.
 * The halo is always drawn semi-transparent; `mode` is not read.
 * The rotation upload is gte_ldrotmatrix. */
void FieldFx_DrawOrbitPoint(int x, int y, void *color, int scale, int mode)
{
    GteShortVector offset;
    GteMatrix matrix;
    FieldTilePoint *point;
    RenderTintTile *glow;
    RenderTintMode *blend;
    u8 *rgb = color;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    u32 depth;

    memset(&offset, 0, sizeof(offset));
    offset.x = x;
    offset.y = 0;
    offset.z = 0;
    point = (FieldTilePoint *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += sizeof(FieldTilePoint);
    glow = (RenderTintTile *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += sizeof(RenderTintTile);
    matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = 0x1000;
    matrix.m[0][1] = matrix.m[0][2] = matrix.m[1][0] = matrix.m[1][2] =
        matrix.m[2][0] = matrix.m[2][1] = 0;
    RotMatrixZ(y, &matrix);
    gte_ldrotmatrix(&matrix);
    gte_ldv0(&offset);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    SetTile1(point);
    SetTile(glow);
    SetSemiTrans(point, 1);
    glow->r = point->r = rgb[0] * scale / 128;
    glow->g = point->g = rgb[1] * scale / 128;
    glow->b = point->b = rgb[2] * scale / 128;
    glow->r >>= 2;
    glow->g >>= 2;
    glow->b >>= 2;
    gte_stsxy2(&point->x);
    gte_stszotz(&depth);
    depth -= (u16)g_FieldFxSpriteParams.depth;
    if (depth < 0x1000) {
        glow->x = point->x - 1;
        glow->y = point->y - 1;
        glow->height = 3;
        glow->width = 3;
        AddPrim((u32 *)D_800B0E38.ordering[g_ActiveDrawSlot] + depth, (u32 *)point);
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[g_ActiveDrawSlot], depth);
        blend = (RenderTintMode *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
        g_DrawPacketOffset += sizeof(RenderTintMode);
        SetDrawTPage(blend, 0, 1, GetTPage(0, 1, 0, 0));
        if (glow) {
            glow->command |= 2;
            TILE_OT_ADDPRIM(ot, glow, link);
        }
        TILE_OT_ADDPRIM(ot, blend, link);
    }
}

/* Gouraud line from `from` (color0 at scale0/128) to `to` (color1 at
 * scale1/128) linked at the mean depth, with a 16x16 glow cell stretched
 * along it; mode 0xFF draws both opaque, anything else semi-transparent. */
void FieldFx_DrawGlowLine(GteShortVector *from, GteShortVector *to, u8 *color0,
                   u8 *color1, int scale0, int scale1, int mode)
{
    u32 depth[2];
    FieldLineG2Packet *line;
    FieldGlowQuadPacket *glow;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    int angle;
    int across;
    int acrossY;
    int along;
    int alongY;

    line = (FieldLineG2Packet *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += sizeof(FieldLineG2Packet);
    glow = (FieldGlowQuadPacket *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += sizeof(FieldGlowQuadPacket);
    gte_ldv0(from);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    line->r0 = color0[0] * scale0 / 128;
    line->g0 = color0[1] * scale0 / 128;
    line->b0 = color0[2] * scale0 / 128;
    {
        s32 *out;
        out = (s32 *)&depth[0];
        gte_stszotz(out);
    }
    gte_stsxy2(&line->x0);
    line->tag.length = 4;
    line->code = 0x50;
    gte_ldv0(to);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    line->r1 = color1[0] * scale1 / 128;
    line->g1 = color1[1] * scale1 / 128;
    line->b1 = color1[2] * scale0 / 128;
    {
        s32 *out;
        out = (s32 *)&depth[1];
        gte_stszotz(out);
    }
    gte_stsxy2(&line->x1);
    depth[0] = (int)(depth[0] + depth[1]) / 2 - (u16)g_FieldFxSpriteParams.depth;
    if (depth[0] < 0x1000) {
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[g_ActiveDrawSlot], depth[0]);
        if (mode != 0xFF) {
            drawMode = (RenderTintMode *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
            g_DrawPacketOffset += sizeof(RenderTintMode);
            SetDrawTPage(drawMode, 0, 1, GetTPage(0, mode, 0, 0));
            if (line) {
                line->code |= 2;
                TILE_OT_ADDPRIM(ot, line, link);
            }
            TILE_OT_ADDPRIM(ot, drawMode, link);
        } else if (line) {
            TILE_OT_ADDPRIM(ot, line, link);
        }
        angle = ratan2(line->y1 - line->y0, line->x1 - line->x0);
        across = rsin(-angle) / 512;
        acrossY = rcos(-angle) / 512;
        along = rcos(angle) / 256;
        alongY = rsin(angle) / 256;
        glow->tag.length = 12;
        glow->code = 0x3C;
        if (mode == 0xFF) {
            glow->code = 0x3C;
            glow->tpage = g_FieldFxTpageTableEntry1;
        } else {
            glow->code = 0x3E;
            glow->tpage = g_FieldFxTpageTableEntry1 | GetTPage(0, mode, 0, 0);
        }
        glow->clut = GetClut(0x20, 0x1D6);
        glow->x0 = line->x0 - along + across;
        glow->y0 = line->y0 - alongY + acrossY;
        glow->x1 = line->x0 - along - across;
        glow->y1 = line->y0 - alongY - acrossY;
        glow->x2 = line->x1 + along + across;
        glow->y2 = line->y1 + alongY + acrossY;
        glow->x3 = line->x1 + along - across;
        glow->y3 = line->y1 + alongY - acrossY;
        glow->u0 = 0x90;
        glow->u2 = 0x90;
        glow->v0 = 0xD0;
        glow->v1 = 0xD0;
        glow->u1 = 0x9F;
        glow->u3 = 0x9F;
        glow->v2 = 0xDF;
        glow->v3 = 0xDF;
        glow->r0 = line->r0;
        glow->g0 = line->g0;
        glow->b0 = line->b0;
        glow->r1 = line->r0;
        glow->g1 = line->g0;
        glow->b1 = line->b0;
        glow->r2 = line->r1;
        glow->g2 = line->g1;
        glow->b2 = line->b1;
        glow->r3 = line->r1;
        glow->g3 = line->g1;
        glow->b3 = line->b1;
        AddPrim((unsigned int *)D_800B0E38.ordering[g_ActiveDrawSlot] + depth[0],
                (unsigned int *)glow);
    }
}

/* Draw a textured ribbon `width` wide through `count` points: every joint
 * is widened across the screen direction of the segment that leaves it,
 * shaded from color0 to color1 (each at scale/128) along the ribbon and
 * textured from u..u+du by v..v+height. Mode 0xFF draws opaque, anything
 * else semi-transparent. */
void FieldFx_DrawTexturedRibbon(GteShortVector *points, s16 count, int width, int u, int v,
                   int du, int height, int tpage, int clut, int scale,
                   RenderColor *color0, RenderColor *color1, int mode)
{
    GteShortVector *point;
    GteShortVector angles;
    GteShortVector right;
    GteShortVector left;
    FieldRibbonColor color0Scaled;
    FieldRibbonColor color1Scaled;
    FieldRibbonColor color;
    FieldScreenPoint screen0;
    FieldScreenPoint screen1;
    GteMatrix matrix;
    u32 unused;
    u32 depth;
    FieldRibbonPacket *packet;
    int i;
    u32 previousDepth;
    int bias;
    int t;
    u32 shade;
    int dx;
    int dy;
    int angle;
    int joint;

    packet = (FieldRibbonPacket *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += count * sizeof(FieldRibbonPacket);
    bias = (u16)g_FieldFxSpriteParams.depth;
    color0Scaled.bytes.rgb[0] = color0->r * scale / 128;
    color0Scaled.bytes.rgb[1] = color0->g * scale / 128;
    color0Scaled.bytes.rgb[2] = color0->b * scale / 128;
    color1Scaled.bytes.rgb[0] = color1->r * scale / 128;
    color1Scaled.bytes.rgb[1] = color1->g * scale / 128;
    color1Scaled.bytes.rgb[2] = color1->b * scale / 128;
    right.x = width / 2;
    previousDepth = 0;
    right.y = right.z = 0;
    left.y = left.z = 0;
    left.x = -width / 2;
    angles.x = angles.y = 0;
    point = points;
    for (i = 0; i < count; i++) {
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        gte_ldv3(&point[0], &point[1], &point[0]);
        gte_rtpt_padded();
        packet->tag.length = 12;
        packet->c0.bytes.code = 0x3C;
        if (mode == 0xFF) {
            packet->c0.bytes.code = 0x3C;
            packet->tpage = tpage;
        } else {
            packet->c0.bytes.code = 0x3E;
            packet->tpage = GetTPage(0, mode, 0, 0) | tpage;
        }
        packet->clut = clut;
        gte_stsxy3(&screen0, &screen1, &unused);
        gte_avsz3_padded();
        gte_stszotz(&depth);
        depth -= bias;
        if (depth >= 0x1000) {
            return;
        }
        dx = screen1.x - screen0.x;
        dy = screen1.y - screen0.y;
        angle = ratan2(dy, dx) + 0x400;
        t = (i << 12) / count;
        LoadAverageCol(&color0Scaled, &color1Scaled, 0x1000 - t, t, &color);
        if (i == 0) {
            joint = angle;
            angles.z = joint;
            RotMatrix(&angles, &matrix);
            gte_ldv0(&point[0]);
            gte_rt();
            packet->c0.bytes.rgb[0] = packet->c1.bytes.rgb[0] = color.bytes.rgb[0];
            packet->c0.bytes.rgb[1] = packet->c1.bytes.rgb[1] = color.bytes.rgb[1];
            packet->c0.bytes.rgb[2] = packet->c1.bytes.rgb[2] = color.bytes.rgb[2];
            gte_stmac(matrix.t);
            gte_ldrotmatrix(&matrix);
            gte_ldtransmatrix(&matrix);
            gte_ldv3(&right, &left, &right);
            gte_rtpt_padded();
            packet->u0 = packet->u1 = u;
            packet->v0 = v;
            packet->v1 = v + height;
            previousDepth = depth;
            gte_stsxy3(&packet->p0, &packet->p1, &unused);
            packet++;
        } else if (i < count - 1) {
            joint = angle;
            angles.z = joint;
            RotMatrix(&angles, &matrix);
            gte_ldv0(&point[0]);
            gte_rt();
            shade = color.word;
            packet[-1].c3.word = shade;
            packet[-1].c2.word = shade;
            gte_stmac(matrix.t);
            gte_ldrotmatrix(&matrix);
            gte_ldtransmatrix(&matrix);
            gte_ldv3(&right, &left, &right);
            gte_rtpt_padded();
            packet[-1].v2 = v;
            packet[-1].v3 = v + height;
            packet[-1].u2 = packet[-1].u3 = u + du * i / count;
            gte_stsxy3(&packet[-1].p2, &packet[-1].p3, &unused);
            AddPrim((unsigned int *)D_800B0E38.ordering[g_ActiveDrawSlot] + previousDepth,
                    (unsigned int *)&packet[-1]);
            packet->p0.word = packet[-1].p2.word;
            packet->p1.word = packet[-1].p3.word;
            packet->c0.bytes.rgb[0] = packet->c1.bytes.rgb[0] = color.bytes.rgb[0];
            packet->c0.bytes.rgb[1] = packet->c1.bytes.rgb[1] = color.bytes.rgb[1];
            packet->c0.bytes.rgb[2] = packet->c1.bytes.rgb[2] = color.bytes.rgb[2];
            packet->u0 = packet->u1 = packet[-1].u2;
            packet->v0 = v;
            packet->v1 = v + height;
            previousDepth = depth;
            packet++;
        } else {
            joint = angle;
            angles.z = joint;
            RotMatrix(&angles, &matrix);
            gte_ldv0(&point[0]);
            gte_rt();
            shade = color.word;
            packet[-1].c3.word = shade;
            packet[-1].c2.word = shade;
            gte_stmac(matrix.t);
            gte_ldrotmatrix(&matrix);
            gte_ldtransmatrix(&matrix);
            gte_ldv3(&right, &left, &right);
            gte_rtpt_padded();
            packet[-1].v2 = v;
            packet[-1].v3 = v + height;
            packet[-1].u2 = packet[-1].u3 = u + du * i / count;
            gte_stsxy3(&packet[-1].p2, &packet[-1].p3, &unused);
            AddPrim((unsigned int *)D_800B0E38.ordering[g_ActiveDrawSlot] + previousDepth,
                    (unsigned int *)&packet[-1]);
            packet->p0.word = packet[-1].p2.word;
            packet->p1.word = packet[-1].p3.word;
            packet->c0.bytes.rgb[0] = packet->c1.bytes.rgb[0] = color.bytes.rgb[0];
            packet->c0.bytes.rgb[1] = packet->c1.bytes.rgb[1] = color.bytes.rgb[1];
            packet->c0.bytes.rgb[2] = packet->c1.bytes.rgb[2] = color.bytes.rgb[2];
            packet->u0 = packet->u1 = packet[-1].u2;
            packet->v0 = v;
            packet->v1 = v + height;
            angles.z = joint;
            RotMatrix(&angles, &matrix);
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            gte_ldv0(&point[1]);
            gte_rt();
            packet->c2.word = packet->c3.word = color1Scaled.word;
            gte_stmac(matrix.t);
            gte_ldrotmatrix(&matrix);
            gte_ldtransmatrix(&matrix);
            gte_ldv3(&right, &left, &right);
            gte_rtpt_padded();
            packet->v2 = v;
            packet->u2 = packet->u3 = u + du;
            packet->v3 = v + height;
            gte_stsxy3(&packet->p2, &packet->p3, &unused);
            AddPrim((unsigned int *)D_800B0E38.ordering[g_ActiveDrawSlot] + depth,
                    (unsigned int *)packet);
        }
        point++;
    }
}

void FieldFx_UpdatePointHistory(RenderHistoryPoint *history, s16 count,
                                 RenderHistoryPoint *value, int reset) {
    RenderHistoryPoint *cursor;
    int i;

    if (reset) {
        count++;
        for (i = 0; i < count; i++) {
            history->vector.x = value->vector.x;
            history->vector.y = value->vector.y;
            history->vector.z = value->vector.z;
            history++;
        }
    } else {
        cursor = history + (count - 1);
        for (i = 0; i < count;) {
            i++;
            cursor[1].words[0] = cursor[0].words[0];
            cursor[1].words[1] = cursor[0].words[1];
            cursor--;
        }
        history->words[0] = value->words[0];
        history->words[1] = value->words[1];
    }
}

/* Matching debt: two empty barriers and five volatile stack parameters keep
 * every register save before the stack argument loads (see
 * docs/CODE_ORGANIZATION.md). */
void FieldFx_DrawPulledSprite(GteShortVector *position, int scale_x, int scale_y,
                   int texture, volatile int input_clut, volatile int input_page,
                   volatile int input_intensity, RenderColor *volatile input_color,
                   volatile int input_pull)
{
    GteShortVector center;
    GteShortVector screen;
    u32 depth;
    FieldStripPacket *packet;
    int r, g, b;
    int u, v;
    int width, height;
    int clut;
    int page;
    int intensity;
    register RenderColor *color;
    int pull;

    asm volatile("" : : : "memory");
    pull = input_pull;
    clut = input_clut;
    page = input_page;
    intensity = input_intensity;
    color = input_color;
    asm volatile("" : : : "$6");
    center.x = 0xA0;
    center.y = 0x78;
    packet = (FieldStripPacket *)(D_800B0E38.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += sizeof(FieldStripPacket);
    if (color == 0) {
        r = g = b = intensity;
    } else {
        r = intensity * color->r / 128;
        g = intensity * color->g / 128;
        b = intensity * color->b / 128;
    }
    packet->r0 = r;
    packet->g0 = g;
    packet->b0 = b;
    if (g_FieldFxSpriteParams.parameter06 != 0) {
        u = texture & 0xF;
        v = 0;
        if (u >= 8) {
            u -= 8;
            v = 0x20;
        }
        u <<= 4;
        v += texture / 16 * 16;
    } else {
        u = (texture & 0xF) << 4;
        v = texture / 16 * 16;
    }
    gte_ldrotmatrix(D_800BCFA4.value);
    gte_ldtransmatrix(D_800BCFA4.value);
    gte_ldv0(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    packet->tag.length = 9;
    packet->code = 0x2C;
    if (page == 0xFF) {
        packet->code &= ~2;
        packet->tpage = g_FieldFxSpriteParams.tpage;
    } else {
        packet->code |= 2;
        packet->tpage = g_FieldFxSpriteParams.tpage | GetTPage(0, page, 0, 0);
    }
    packet->clut = clut;
    width = g_FieldFxSpriteParams.extent_x;
    height = g_FieldFxSpriteParams.extent_y;
    packet->u0 = packet->u2 = u;
    packet->v1 = packet->v0 = v;
    packet->u1 = packet->u3 = u + width - 1;
    packet->v2 = packet->v3 = v + height - 1;
    width = width * scale_x / 8192;
    height = height * scale_y / 8192;
    gte_stszotz(&depth);
    depth -= (u16)g_FieldFxSpriteParams.depth;
    if (pull != 0) {
        depth = 14;
    }
    if (depth < 0x1000) {
        gte_stsxy2(&screen);
        pull *= 2;
        LoadAverageShort12(&screen, &center, 0x1000 - pull, pull, &screen);
        packet->x0 = packet->x2 = screen.x - width;
        packet->y0 = packet->y1 = screen.y - height;
        packet->x1 = packet->x3 = screen.x + width;
        packet->y2 = packet->y3 = screen.y + height;
        AddPrim((unsigned int *)STRIP_OT(depth), (unsigned int *)packet);
    }
}

int FieldFx_PlaySoundAtOwner(int arg0, int arg1) {
    FieldActor *actor;
    s16 position[3];

    actor = g_FieldFxOwner->actor;
    position[0] = actor->render_object.matrices->translation[0];
    position[1] = actor->render_object.matrices->translation[1];
    position[2] = actor->render_object.matrices->translation[2];
    return Asset_Find08w(arg0, arg1, position[0], position[1], position[2]);
}

int FieldFx_GetOwnerProgress(void) {
    FieldFxObjectPrefix *ctx;
    FieldActorState *node;
    int value;

    ctx = g_FieldFxOwner;
    node = ctx->actor->state;
    value = 0x80;
    if (node != 0) {
        value = node->progress;
        if (value >= 0x41) {
            value = 0x80;
        }
    }
    return value;
}

int FieldFx_SpawnTask(int id)
{
    /* Reused for the callback's additional payload size after the slot scan. */
    int result;
    FieldFxTaskContext *ctx = g_FieldFxTaskContext;
    FieldFxTaskSlot *slot = ctx->slots;
    int size;
    char *payload;
    for (result = 0; result < 8; result++, slot++) {
        if (slot->id == 0xFFFF)
            break;
    }
    if (result == 8)
        return -1;
    ctx->count++;
    slot->id = id;
    slot->age = 0;
    size = ctx->table->sizes[id];
    payload = ctx->cursor;
    ctx->used += size;
    if (ctx->used > 0x97C) {
        payload = ctx->arena;
        ctx->used = size;
        ctx->cursor = payload;
    }
    slot->end = payload + size;
    slot->start = payload;
    g_FieldFxActiveTaskSlot = slot;
    result = ctx->table->callbacks[slot->id](0, payload, ctx->argument);
    if (!result)
        slot->end = 0;
    ctx->cursor += size + result;
    ctx->used += result;
    return size + result;
}

/* Runs the owner's task script until it yields, then steps every live task.
 * Opcodes: 0 stop (end of script), 1 set variable, 2 spawn task,
 * 3 count down a variable and jump while positive, 4 wait on a timer or
 * a variable, 5 toggle the owner's held state. */
int FieldFx_UpdateTasks(FieldFxTaskOwner *owner)
{
    FieldFxTaskContext *context = &owner->tasks;
    FieldFxTaskSlot *slot;
    FieldActorState *state;
    unsigned int flags;
    int count;
    int running = 1;
    int result;
    int i;
    s16 op, a, b;

    g_FieldFxOwner = &owner->prefix;
    g_FieldFxTaskContext = context;
    while (running) {
        if (context->delay != 0) {
            if (--context->delay != 0)
                break;
            context->delay = 0;
        }
        op = *context->pc.script;
        context->pc.script++;
        a = *context->pc.script;
        context->pc.script++;
        b = *context->pc.script;
        context->pc.script++;
        switch (op) {
        case 0:
            context->pc.script -= 3;
            if (context->count == 0) {
                owner->prefix.reserved00 = 4;
                if (g_FieldFxTaskContext->flags) {
                    if (g_FieldFxOwner->actor->state != 0) {
                        g_FieldFxOwner->actor->state->core_flags &= 0xC0FFFFFF;
                        *g_FieldFxOwner->actor->state->action_state = 4;
                        g_FieldFxTaskContext->flags = 0;
                    }
                }
            }
            running = 0;
            break;
        case 1:
            context->variables[a] = b;
            break;
        case 2:
            FieldFx_SpawnTask(a);
            break;
        case 3:
            if ((s16)--context->variables[b] <= 0) {
                context->variables[b] = 0;
                running = 0;
            } else {
                /* Retail takes the operand as an absolute address. */
                context->pc.address = a - 6;
            }
            break;
        case 4:
            if (b != 1) {
                context->delay = a;
                running = 0;
            } else if (context->variables[a] == 0) {
                running = 0;
                context->pc.script -= 3;
            }
            break;
        case 5:
            if (a == 0) {
                g_FieldFxOwner->actor->state->core_flags =
                    (g_FieldFxOwner->actor->state->core_flags & 0xC0FFFFFF) | 0x1000000;
                g_FieldFxTaskContext->flags = 1;
            } else if (g_FieldFxTaskContext->flags && g_FieldFxOwner->actor && g_FieldFxOwner->actor->state) {
                u8 *mode = g_FieldFxOwner->actor->state->action_state;
                if (*mode == 1)
                    *mode = 2;
            }
            break;
        }
    }
    if (g_FieldFxTaskContext->flags && g_FieldFxOwner->actor && g_FieldFxOwner->actor->state) {
        state = g_FieldFxOwner->actor->state;
        flags = state->core_flags;
        count = (flags >> 24) & 0x3F;
        if (count >= 2)
            state->core_flags = (flags & 0xC0FFFFFF) | (((count - 1) & 0x3F) << 24);
        if ((int)((state->core_flags >> 24) & 0x3F) > 0) {
            if (*state->action_state == 0) {
                if (state->control10.command_value <= 0)
                    owner->prefix.reserved00 = 4;
            }
        }
        if (*state->action_state == 0) {
            if (state->core_flags & 0x180E)
                owner->prefix.reserved00 = 4;
            if (g_FieldFxOwner->actor->mode < 2)
                return 0;
        }
    }
    if (context->flags)
        g_FieldFxAltPaletteFlag = Asset_SearchByKeyType(owner->prefix.asset_type);
    slot = context->slots;
    for (i = 0; i < 8; i++, slot++) {
        if (slot->id != 0xFFFF) {
            g_FieldFxActiveTaskSlot = slot;
            g_FieldFxCurrentAge = slot->age;
            result = context->table->callbacks[slot->id](1, slot->start, context->argument);
            if (slot->end) {
                if (result == 2) {
                    if (!FieldFx_UpdateCallbackList(slot->end))
                        result = 1;
                } else {
                    FieldFx_UpdateCallbackList(slot->end);
                }
            }
            slot->age++;
            if (result == 1) {
                slot->id = 0xFFFF;
                context->count--;
            }
        }
    }
    return 0;
}

int FieldFx_InitTaskContext(FieldFxTaskOwner *owner)
{
    FieldFxTaskContext *context = &owner->tasks;
    FieldFxTaskSlot *slot;
    int i;

    owner->prefix.owner_kind = 1;
    owner->prefix.live_slots = 0;
    context->delay = 0;
    context->used = 0;
    context->cursor = context->arena;
    context->count = 0;
    context->flags = 0;
    for (i = 6; i >= 0; i--) {
        context->variables[i] = 0;
    }

    slot = context->slots;
    for (i = 0; i < 8; i++, slot++) {
        slot->id = 0xFFFF;
        slot->age = 0;
        slot->start = 0;
    }

    return 0;
}

int FieldFx_InitTaskProgram(FieldFxTaskOwner *owner, int skip, int arg2, int arg3, int arg4, int arg5) {
    FieldFxTaskContext *ctx;
    int (*callback)(int, int, int, int);

    ctx = &owner->tasks;
    if (skip == 0) {
        callback = ((FieldFxTaskProgram *)owner->tasks.table)->initialize;
        g_FieldFxOwner = &owner->prefix;
        g_FieldFxTaskContext = ctx;
        if (callback != 0) {
            owner->tasks.argument = callback(arg2, arg3, arg4, arg5);
        }
    }
    return 0;
}

/* Draws every live task of the owner: loads the current view matrix into
 * the GTE, then runs each task callback and its particle list in mode 2. */
int FieldFx_DrawTasks(FieldFxTaskOwner *owner)
{
    FieldFxTaskContext *context = &owner->tasks;
    FieldFxTaskSlot *slot = context->slots;
    int i;

    g_FieldFxOwner = &owner->prefix;
    g_FieldFxTaskContext = context;
    if (context->flags)
        g_FieldFxAltPaletteFlag = Asset_SearchByKeyType(owner->prefix.asset_type);
    for (i = 0; i < 8; i++, slot++) {
        if (slot->id != 0xFFFF) {
            gte_ldrotmatrix((const GteMatrixWords *)D_800BCFA4.value);
            gte_ldtransmatrix((const GteMatrixWords *)D_800BCFA4.value);
            g_FieldFxActiveTaskSlot = slot;
            g_FieldFxCurrentAge = slot->age;
            context->table->callbacks[slot->id](2, slot->start, context->argument);
            if (slot->end)
                FieldFx_DrawCallbackList(slot->end);
        }
    }
    return 0;
}

int FieldFx_CloseTaskOwner(u8 *state) {
    state[0] = 4;
    return 0;
}

int FieldFx_InitEffectResources(void) {
    void **table;

    FieldFx_InitTexturePageTable();

    table = &g_FieldEffectAssetTable;
    g_FieldFxModel6DEAE684 = Asset_FindTable08ByU32Key(*table, 0x6DEAE684);
    D_800F32D8 = Asset_FindTable08ByU32Key(*table, 0x8AB5B684);
    D_800F3418 = Asset_FindTable08ByU32Key(*table, 0x5AB32504);
    g_FieldFxTwinModelB = Asset_FindTable08ByU32Key(*table, 0x5AB32104);
    g_FieldFxModel83ACD504 = Asset_FindTable08ByU32Key(*table, 0x83ACD504);
    g_FieldFxGlintModel = Asset_FindTable08ByU32Key(*table, 0x57E8EB04);
    FieldFx_LoadEffectTexturePage(0);

    return 0;
}
