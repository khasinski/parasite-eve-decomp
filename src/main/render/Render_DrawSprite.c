#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_nop.h"
#include "pe1/render_camera.h"
#include "pe1/field_movement.h"
#include "pe1/battle_runtime.h"
/* Build the camera basis from yaw using the retail GTE operations. */
int Render_DrawSprite(void)
{
    GteMatrixStorage rotation;
    GteShortVector axis;
    GteVector up,right,newUp,forward;
    int angle,cosine,sine;
    if((D_800BE9A0&0xF000)==0x7000)angle = D_800BD022;
    else angle = D_800BD020;
    {
        int masked = angle&4095;
        if(D_8009D254 && (D_8009D2E8&16)) {
            angle+=2048;
            angle+=(((unsigned int)((Combatant *)D_8009D254->core)->stateFlags)>>7)&0xC00;
            masked = angle&4095;
        }
        cosine = rcos(masked);
        sine = rsin(masked);
    }
    rotation.matrix.m[0][2] = sine;
    rotation.matrix.m[2][0] = -sine;
    rotation.matrix.m[1][1] = 4096;
    axis.z = 4096;
    asm volatile("":::"memory");
    rotation.matrix.m[0][0] = cosine;
    rotation.matrix.m[2][2] = cosine;
    rotation.matrix.t[0] = rotation.matrix.t[1] = rotation.matrix.t[2] = 0;
    rotation.matrix.m[0][1] = rotation.matrix.m[1][0] = rotation.matrix.m[1][2] = rotation.matrix.m[2][1] = 0;
    axis.x = 0;
    axis.y = 0;
    {
        register u32 *matrix asm("$2") = rotation.words;
        register u32 a asm("$12");
        register u32 b asm("$13");
        register u32 c asm("$14");
        asm("":"=r"(matrix):"0"(matrix):"memory");
        a = matrix[0];
        b = matrix[1];
        asm(""::"r"(a),"r"(b));
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = matrix[2];
        b = matrix[3];
        c = matrix[4];
        asm(""::"r"(a),"r"(b),"r"(c));
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);
        a = matrix[5];
        b = matrix[6];
        asm(""::"r"(a),"r"(b));
        gte_ctc2_5(a);
        c = matrix[7];
        gte_ctc2_6(b);
        gte_ctc2_7(c);
    }
    gte_lwc2_0_0(&axis);
    gte_lwc2_1_4(&axis);
    PE1_NOP();
    PE1_NOP();
    gte_mvmva_rotation_v0_translation_sf12();
    gte_swc2_25_0(&forward);
    gte_swc2_26_4(&forward);
    gte_swc2_27_8(&forward);
    {
        register GteVector *source asm("$2");
        register u32 a asm("$12");
        register u32 b asm("$13");
        register u32 c asm("$14");
        up.y = 4096;
        asm volatile("":::"memory");
        source = &up;
        asm(""::"r"(source):"memory");
        up.x = 0;
        up.z = 0;
        asm("":"=r"(source):"0"(source):"memory");
        a = source->x;
        b = source->y;
        asm(""::"r"(a),"r"(b));
        gte_ctc2_0(a);
        c = source->z;
        gte_ctc2_2(b);
        gte_ctc2_4(c);
    }
    {
        gte_lwc2_11_8(&forward);
        gte_lwc2_9_0(&forward);
        gte_lwc2_10_4(&forward);
        PE1_NOP();
        PE1_NOP();
        gte_op_sf12_command();
        gte_swc2_25_0(&right);
        gte_swc2_26_4(&right);
        gte_swc2_27_8(&right);
    }
    {
        register GteVector *source asm("$3") = &forward;
        register u32 a asm("$12");
        register u32 b asm("$13");
        register u32 c asm("$14");
        asm("":"=r"(source):"0"(source):"memory");
        a = source->x;
        b = source->y;
        asm(""::"r"(a),"r"(b));
        gte_ctc2_0(a);
        c = source->z;
        gte_ctc2_2(b);
        gte_ctc2_4(c);
    }
    {
        gte_lwc2_11_8(&right);
        gte_lwc2_9_0(&right);
        gte_lwc2_10_4(&right);
        PE1_NOP();
        PE1_NOP();
        gte_op_sf12_command();
        gte_swc2_25_0(&newUp);
        gte_swc2_26_4(&newUp);
        gte_swc2_27_8(&newUp);
    }
    {
        int xx = right.x,xy = newUp.x,xz = forward.x;
        int yx = right.y,yy = newUp.y,yz = forward.y;
        int zx = right.z,zy = newUp.z,zz = forward.z;
        D_800BD014 = 0;
        D_800BD018 = 0;
        D_800BD01C = 0;
        D_800BD000.m[0][0] = xx;
        D_800BD002 = xy;
        D_800BD004 = xz;
        D_800BD006 = yx;
        D_800BD008 = yy;
        D_800BD00A = yz;
        D_800BD00C = zx;
        D_800BD00E = zy;
        D_800BD010 = zz;
    }
    return 0;
}
