#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/field_textured_model.h"

#define MODEL_VERTEX(model, offset) ((FieldModelVertex *)&(model)->bytes[offset])
#define MODEL_COLOR(model, offset) (((FieldModelColor *)&(model)->bytes[offset])->word)

/* Composes the placement onto the view matrix, then walks the flat and
 * gouraud textured triangles and quads, culling back faces by NCLIP and
 * linking the visible ones at their averaged depth. */
void func_800C71E4(FieldTexturedModel *model, GteMatrix *placement)
{
    FieldModelHeader *header;
    FieldModelTri *tri;
    FieldModelQuad *quad;
    FieldModelVertex *v0;
    FieldModelVertex *v1;
    FieldModelVertex *v2;
    FieldModelVertex *v3;
    FieldModelFt3Packet *ft3;
    FieldModelGt3Packet *gt3;
    FieldModelFt4Packet *ft4;
    FieldModelGt4Packet *gt4;
    FieldModelScratch *sp;
    int i;

    sp = FIELD_MODEL_SCRATCH;
    sp->tpage = D_800F346C;
    sp->clut = D_800F3414;
    sp->semiTrans = D_800F33E4;
    CompMatrix(D_800BCFA4.matrix, placement, &sp->matrix);
    SetTransMatrix(&sp->matrix);
    SetRotMatrix(&sp->matrix);
    header = &model->header;
    tri = header->tris;
    ft3 = (FieldModelFt3Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    for (i = 0; i < header->flatTris; i++, tri++) {
        v0 = MODEL_VERTEX(model, tri->vertex[0]);
        v1 = MODEL_VERTEX(model, tri->vertex[1]);
        v2 = MODEL_VERTEX(model, tri->vertex[2]);
        gte_ldv3(v0, v1, v2);
        ft3->uv0 = tri->uv[0];
        gte_rtpt_padded();
        ft3->uv1 = tri->uv[1];
        ft3->uv2 = tri->uv[2];
        gte_nclip();
        gte_stmac0(&sp->winding);
        if (sp->winding > 0) {
            gte_avsz3_padded();
            gte_stotz(&sp->depth);
            sp->depth += D_800F3420;
            ft3->tpage = sp->tpage;
            ft3->clut = sp->clut;
            if (sp->depth > 0) {
                FieldModelLink link;

                ft3->color0.word = MODEL_COLOR(model, v0->color);
                gte_stsxy3(&ft3->x0, &ft3->x1, &ft3->x2);
                ft3->tag.length = 7;
                ft3->color0.rgb.code = 0x24;
                if (sp->semiTrans) {
                    ft3->color0.rgb.code |= 2;
                } else {
                    ft3->color0.rgb.code &= ~2;
                }
                ft3->tag.address = TEXTURED_MODEL_OT(sp->depth)->address;
                link.tag = &ft3->tag;
                D_8009CDD8 += sizeof(FieldModelFt3Packet);
                TEXTURED_MODEL_OT(sp->depth)->address = link.word;
                ft3++;
            }
        }
    }
    gt3 = (FieldModelGt3Packet *)ft3;
    for (i = 0; i < header->shadedTris; i++, tri++) {
        v0 = MODEL_VERTEX(model, tri->vertex[0]);
        v1 = MODEL_VERTEX(model, tri->vertex[1]);
        v2 = MODEL_VERTEX(model, tri->vertex[2]);
        gte_ldv3(v0, v1, v2);
        gt3->uv0 = tri->uv[0];
        gte_rtpt_padded();
        gt3->uv1 = tri->uv[1];
        gt3->uv2 = tri->uv[2];
        gte_nclip();
        gte_stmac0(&sp->winding);
        if (sp->winding > 0) {
            gte_avsz3_padded();
            gt3->tpage = sp->tpage;
            gt3->clut = sp->clut;
            gte_stotz(&sp->depth);
            sp->depth += D_800F3420;
            if (sp->depth > 0) {
                FieldModelLink link;

                gt3->color0.word = MODEL_COLOR(model, v0->color);
                gt3->color1.word = MODEL_COLOR(model, v1->color);
                gt3->color2.word = MODEL_COLOR(model, v2->color);
                gte_stsxy3(&gt3->x0, &gt3->x1, &gt3->x2);
                gt3->tag.length = 9;
                gt3->color0.rgb.code = 0x34;
                if (sp->semiTrans) {
                    gt3->color0.rgb.code |= 2;
                } else {
                    gt3->color0.rgb.code &= ~2;
                }
                gt3->tag.address = TEXTURED_MODEL_OT(sp->depth)->address;
                link.tag = &gt3->tag;
                D_8009CDD8 += sizeof(FieldModelGt3Packet);
                TEXTURED_MODEL_OT(sp->depth)->address = link.word;
                gt3++;
            }
        }
    }
    quad = (FieldModelQuad *)tri;
    ft4 = (FieldModelFt4Packet *)gt3;
    for (i = 0; i < header->flatQuads; i++, quad++) {
        v0 = MODEL_VERTEX(model, quad->vertex[0]);
        v1 = MODEL_VERTEX(model, quad->vertex[1]);
        v2 = MODEL_VERTEX(model, quad->vertex[2]);
        gte_ldv3(v0, v1, v2);
        ft4->uv0 = quad->uv[0];
        gte_rtpt_padded();
        ft4->uv1 = quad->uv[1];
        ft4->uv2 = quad->uv[2];
        gte_nclip();
        ft4->uv3 = quad->uv[3];
        gte_stmac0(&sp->winding);
        if (sp->winding > 0) {
            gte_avsz3_padded();
            ft4->tpage = sp->tpage;
            ft4->clut = sp->clut;
            gte_stotz(&sp->depth);
            sp->depth += D_800F3420;
            if (sp->depth > 0) {
                FieldModelLink link;

                ft4->color0.word = MODEL_COLOR(model, v0->color);
                gte_stsxy3(&ft4->x0, &ft4->x1, &ft4->x2);
                v3 = MODEL_VERTEX(model, quad->vertex[3]);
                gte_ldv0(v3);
                gte_rtps();
                gte_stsxy2(&ft4->x3);
                ft4->tag.length = 9;
                ft4->color0.rgb.code = 0x2C;
                if (sp->semiTrans) {
                    ft4->color0.rgb.code |= 2;
                } else {
                    ft4->color0.rgb.code &= ~2;
                }
                ft4->tag.address = TEXTURED_MODEL_OT(sp->depth)->address;
                link.tag = &ft4->tag;
                D_8009CDD8 += sizeof(FieldModelFt4Packet);
                TEXTURED_MODEL_OT(sp->depth)->address = link.word;
                ft4++;
            }
        }
    }
    gt4 = (FieldModelGt4Packet *)ft4;
    for (i = 0; i < header->shadedQuads; i++, quad++) {
        v0 = MODEL_VERTEX(model, quad->vertex[0]);
        v1 = MODEL_VERTEX(model, quad->vertex[1]);
        v2 = MODEL_VERTEX(model, quad->vertex[2]);
        gte_ldv3(v0, v1, v2);
        gt4->uv0 = quad->uv[0];
        gte_rtpt_padded();
        gt4->uv1 = quad->uv[1];
        gt4->uv2 = quad->uv[2];
        gte_nclip();
        gte_stmac0(&sp->winding);
        if (sp->winding > 0) {
            gte_avsz3_padded();
            gt4->uv3 = quad->uv[3];
            gte_stotz(&sp->depth);
            sp->depth += D_800F3420;
            if (sp->depth > 0) {
                FieldModelLink link;

                gt4->color0.word = MODEL_COLOR(model, v0->color);
                gte_stsxy3(&gt4->x0, &gt4->x1, &gt4->x2);
                v3 = MODEL_VERTEX(model, quad->vertex[3]);
                gt4->color1.word = MODEL_COLOR(model, v1->color);
                gte_ldv0(v3);
                gt4->color2.word = MODEL_COLOR(model, v2->color);
                gte_rtps();
                gt4->color3.word = MODEL_COLOR(model, v3->color);
                gte_stsxy2(&gt4->x3);
                gt4->tpage = sp->tpage;
                gt4->clut = sp->clut;
                gt4->tag.length = 12;
                gt4->color0.rgb.code = 0x3C;
                if (sp->semiTrans) {
                    gt4->color0.rgb.code = 0x3E;
                } else {
                    gt4->color0.rgb.code = 0x3C;
                }
                gt4->tag.address = TEXTURED_MODEL_OT(sp->depth)->address;
                link.tag = &gt4->tag;
                D_8009CDD8 += sizeof(FieldModelGt4Packet);
                TEXTURED_MODEL_OT(sp->depth)->address = link.word;
                gt4++;
            }
        }
    }
}
