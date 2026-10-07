/* MASPSX_FLAGS: --expand-div */
#include "pe1/gte.h"
#include "pe1/room_fx.h"
#include "pe1/render_object.h"
#include "common.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_prim.h"
#include "pe1/field_shape_quads.h"
#include "pe1/gte_types.h"
#include "pe1/field_orbit_point.h"
#include "pe1/random.h"
#include "pe1/field_star_fan.h"
#include "pe1/field_ring_band.h"

/* Matching debt: three transfer-register pins per function, plus five empty
 * pointer/memory barriers across this TU. Matrix loads are C; each GTE
 * instruction and hazard nop uses its individual macro. */

void FieldEng_TransformMatrixPoint(RoomFxTransformOwner *owner, int index,
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

void func_800CE9D4(RoomFxTransformOwner *owner, int index, GteShortVector *out)
{
    GteShortVector direction = D_800C2258;
    GteShortVector origin = D_800C2260;
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
    FieldEng_CalculateLookAngles(from, vector, out);
}

void FieldEng_RotateVector(const GteMatrixWords *matrix,
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

typedef struct ArcPointComponent {
    s16 value;
    s16 reserved02;
} ArcPointComponent;

typedef struct AlternatingArcPoints {
    ArcPointComponent component[8];
} AlternatingArcPoints;

int ratan2(int y, int x);
int rsin(int angle);
int rcos(int angle);
int func_800C6B20(void *arg0);

static inline int div4096(int value) {
    if (value < 0) {
        value += 0xFFF;
    }
    return value >> 12;
}

int func_800CEB8C(s16 *a, s16 *b, int radius) {
    /* Match note: target reuses $s0 for the angle and then the x offset. */
    register int angle asm("$16");
    AlternatingArcPoints points;
    int dx;
    int dz;
    int x;
    register int z asm("$6");

    dx = b[0] - a[0];
    dz = b[2] - a[2];
    angle = -ratan2(dz, dx);
    x = div4096(rsin(angle) * radius);
    z = div4096(rcos(angle) * radius);

    points.component[0].value = a[0] - x;
    points.component[1].value = a[2] - z;
    points.component[2].value = a[0] + x;
    points.component[3].value = a[2] + z;
    points.component[4].value = b[0] - x;
    points.component[5].value = b[2] - z;
    points.component[6].value = b[0] + x;
    points.component[7].value = b[2] + z;

    return func_800C6B20(&points);
}

void func_800CECAC(void) {
    int i;

    for (i = 0; i < 2; i++) {
        *(short *)((char *)D_800E2850 + i * 4) = GetTPage(i, 0, 0x380, 0x100);
    }

    for (i = 0; i < 2; i++) {
        *(short *)((char *)D_800E2850 + i * 4 + 2) = GetTPage(i, 0, 0x340, 0x100);
    }
}

int LoadImage(RECT *rect, void *pixels);

extern void *D_800B0E18;
extern void *D_800B0E1C;
extern s16 D_800F34E4;

void func_800CED3C(int index) {
    RECT rect;
    void *pixels;

    rect.x = 0x380;
    rect.y = 0x100;
    rect.w = 0x40;
    rect.h = 0x100;

    if (index == 0) {
        pixels = D_800B0E18;
    } else {
        pixels = D_800B0E1C;
    }

    LoadImage(&rect, pixels);
    D_800F34E4 = index;
}

void func_800CEDA8(int index) {
    RECT rect;
    void *pixels;

    if (D_800F34E4 != index) {
        rect.x = 0x380;
        rect.y = 0x100;
        rect.w = 0x40;
        rect.h = 0x100;

        if (index == 0) {
            pixels = D_800B0E18;
        } else {
            pixels = D_800B0E1C;
        }

        LoadImage(&rect, pixels);
        D_800F34E4 = index;
    }
}

/* Matched field-shape quads. CPU-side matrix loads, depth scaling and packet
 * writes are C; each GTE instruction and hazard nop has its own macro.
 * Matching debt: pinned transfer registers/pointers and empty constraints,
 * including the template-copy endpoint used to preserve preheader order. */
void func_800CEE20(GteShortVector *position, GteRotation *rotation,
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

    level = D_800C2268;
    if (rotation == 0) {
        rotation = &level;
    }
    template.tag.length = 9;
    template.code = 0x2C;
    if (page == 0xFF) {
        template.code &= ~2;
        template.tpage = D_800F3368.tpage;
    } else {
        template.code |= 2;
        width = GetTPage(0, page, 0, 0);
        template.tpage = D_800F3368.tpage | width;
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
    if (D_800F3368.parameter06 != 0) {
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
    if (D_800F3368.palette == 4 && D_800F3428 != 0) {
        v += 0x60;
    }
    width = D_800F3368.extent_x;
    height = D_800F3368.extent_y;
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
    packet = (FieldStripPacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += D_800E1210[D_800F3368.parameter0A] * sizeof(FieldStripPacket);
    bias = (u16)D_800F3368.depth;
    RotMatrixYXZ(rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    gte_swc2_25_0(matrix.t);
    gte_swc2_26_4(matrix.t);
    gte_swc2_27_8(matrix.t);
    if (rotation->flags == 1) {
        MulRotMatrix(&matrix);
    }
    {
        register const GteMatrixWords *words asm("$17") = (const GteMatrixWords *)&matrix;
        gte_ldrotmatrix(words);
        gte_ldtransmatrix(words);
    }
    vertex = D_800E13BC[D_800F3368.parameter0A];
    i = 0;
    if (i < D_800E1210[D_800F3368.parameter0A]) {
        {
            register void *copyEnd asm("$14") = &template.x3;
            asm("" : : "r"(copyEnd));
        }
        depthOut = &depth;
        for (;;) {
            {
                register GteShortVector *v1 = &vertex[1];
                register GteShortVector *v2 asm("$2") = &vertex[2];
                asm volatile("" : "=r"(v1), "=r"(v2) : "0"(v1), "1"(v2));
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
                register s16 *p0 asm("$4") = &packet->x0;
                register s16 *p1 = &packet->x1;
                register s16 *p2 asm("$2") = &packet->x2;
                asm volatile("" : "=r"(p0), "=r"(p1), "=r"(p2) : "0"(p0), "1"(p1), "2"(p2));
                gte_stsxy0_precise(p0);
                gte_stsxy1_precise(p1);
                gte_stsxy2_precise(p2);
            }
                        gte_stmac0(depthOut);
            if (depth == 0) {
                break;
            }
            {
                register s32 z asm("$12");
                gte_getsz3(z);
                gte_cop2_hazard_slot();
                z >>= 2;
                *depthOut = z;
                asm("" : : "m"(*depthOut) : "$2", "memory");
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
            if ((u32)depth < 0x1000) {
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
            if (i >= D_800E1210[D_800F3368.parameter0A])
                break;
        }
    }
}

/* Field colour animation: colour-key interpolation and the rotating
 * 16-entry CLUT row upload. */

extern u16 D_800E21A8[];

int StoreImage(RECT *rect, void *pixels);

void func_800CF3AC(void *key, void *color, int time)
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

void func_800CF4B4(int arg0, int arg1, u16 *pixels) {
    RECT rect;
    int i;
    u16 *dst;
    int magic;
    int source_offset;

    rect.x = (arg0 & 0xF) << 4;
    rect.y = D_800E1204[D_800F336C] + (arg0 / 16);
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
void FieldEng_TransformTranslation(const GteShortVector *input,
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

void MulRotMatrix(GteMatrix *matrix);

/* Build, optionally scale/compose, and install a rotation matrix.
 * Matching debt: three pinned word-transfer registers. Matrix reads are C;
 * each GTE control-register transfer uses its own instruction macro. */
void func_800CF658(GteRotation *rotation, s32 *scale, GteMatrix *matrix) {
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
    *(u32 *)packet = (*(u32 *)packet & 0xff000000) | (*(u32 *)ordering & 0xffffff);
    *(u32 *)ordering = (*(u32 *)ordering & 0xff000000) | ((u32)packet & 0xffffff);
}
void func_800CF6F8(void *ordering, void *packet, int mode)
{
    void *drawMode;
    if (mode != 255) {
        drawMode = D_800B0E38.packets[D_8009CDDC] + D_8009CDD8;
        D_8009CDD8 += 8;
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
void func_800CF844(GteShortVector *origin, GteShortVector *out, int radius,
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

    rollMatrix = D_800C2270;
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

void FieldEng_CalculateLookAngles(GteShortVector *from, GteShortVector *to,
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
void func_800CFB7C(GteShortVector *input, s16 distance, GteShortVector *out)
{
    GteShortVector rotation;
    GteShortVector direction = D_800C2260;
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

extern u16 D_800E21C8;

int func_800CFCF4(int from, int to, s16 *distance)
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

void func_800CFD50(u16 *from, u16 *to, u16 speed) {
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

int func_800CFE94(s16 *from, s16 *to)
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

void func_800CFF0C(s16 *out)
{
    s16 *out_reg;
    int y;
    int phase;
    int temp;
    int z;

    out_reg = out;
    D_800E21C8 = (D_800E21C8 + 1) & 7;
    temp = rand() & 0x1FF;
    phase = D_800E21C8;
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

void func_800CFFAC(s16 *out)
{
    s16 *out_reg;
    int x;
    int phase;
    int temp;
    int y;

    out_reg = out;
    D_800E21C8 = (D_800E21C8 + 1) & 7;
    temp = rand() & 0x1FF;
    phase = D_800E21C8;
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
void func_800D004C(GteShortVector *position, int width, int height, int segments,
                   GteRotation *rotation, int scale_x, int scale_y,
                   RenderColor *color0, RenderColor *color1, int intensity,
                   int mode)
{
    FieldG3Packet template;
    s16 radii[2];
    GteShortVector vertices[3];
    GteShortVector anchor;
    GteShortVector defaultRotation = D_800C2260;
    GteVector scale = D_800C2290;
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
    {
        register const GteMatrixWords *words asm("$16") = (const GteMatrixWords *)&matrix;
        gte_ldrotmatrix(words);
        gte_ldtransmatrix(words);
    }
    SetPolyG3(&template);
    template.r0 = centre.r;
    template.g0 = centre.g;
    template.b0 = centre.b;
    template.r2 = template.r1 = rim.r;
    template.g2 = template.g1 = rim.g;
    template.b2 = template.b1 = rim.b;
    packet = (FieldG3Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += segments * sizeof(FieldG3Packet);
    vertices[0].x = vertices[0].y = vertices[0].z = vertices[1].z = vertices[2].z = 0;
    radii[0] = width;
    radii[1] = height;
    for (i = 0; i < segments; i++, packet++) {
        /* Prevent a separate induction pointer for packet->code. */
        asm volatile("" : "=r"(packet) : "0"(packet));
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
            register s16 *xy0 asm("$4") = &packet->x0;
            register s16 *xy1 asm("$3") = &packet->x1;
            s16 *xy2 = &packet->x2;
            gte_stsxy0_precise(xy0);
            gte_stsxy1_precise(xy1);
            gte_stsxy2_precise(xy2);
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
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
        if (mode != 0xFF) {
            drawMode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
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
void func_800D0728(GteShortVector *position, int inner, int outer, int segments,
                   GteRotation *rotation, int scale_x, int scale_y,
                   struct RenderColor *color0, struct RenderColor *color1,
                   int intensity, int mode)
{
    FieldG4Packet template;
    GteShortVector vertices[4];
    GteShortVector anchor;
    GteShortVector defaultRotation = D_800C2260;
    GteVector scale = D_800C2290;
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
    {
        register const GteMatrixWords *words asm("$16") = (const GteMatrixWords *)&matrix;
        gte_ldrotmatrix(words);
        gte_ldtransmatrix(words);
    }
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
    packet = (FieldG4Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += segments * sizeof(FieldG4Packet);
    vertices[0].z = vertices[1].z = vertices[2].z = vertices[3].z = 0;
    for (i = 0; i < segments; i++, packet++) {
        /* Prevent a separate induction pointer for packet->code. */
        asm volatile("" : "=r"(packet) : "0"(packet));
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
            register s16 *xy0 asm("$4") = &packet->x0;
            register s16 *xy1 asm("$3") = &packet->x1;
            s16 *xy2 = &packet->x2;
            gte_stsxy0_precise(xy0);
            gte_stsxy1_precise(xy1);
            gte_stsxy2_precise(xy2);
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
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
        entry = ot.tag;
        if (mode != 0xFF) {
            drawMode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
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
