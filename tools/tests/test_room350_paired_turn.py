import pathlib
import re
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_PairedTurnController.c'


class PairedTurnTests(unittest.TestCase):
    def test_layout(self):
        source = SOURCE.read_text().split('extern Actor', 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[OFF(Instance,animation)==14 ? 1:-1];
typedef char b[OFF(Instance,frame)==20 ? 1:-1];
typedef char c[OFF(Instance,yaw)==58 ? 1:-1];
typedef char d[OFF(Instance,position)==0x1FC ? 1:-1];
typedef char e[OFF(Instance,transforms)==0x238 ? 1:-1];
typedef char f[OFF(Owner,action)==24 ? 1:-1];
typedef char g[sizeof(Matrix)==32 ? 1:-1];
typedef char h[sizeof(Vector)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)

    def test_cpu_and_cop2_boundary(self):
        source = SOURCE.read_text().replace('#include "pe1/gte.h"', '')
        source = re.sub(r' asm\("\$\d+"\)', '', source)
        source = source.replace('::: "$2"', '::: "memory"')
        source = source.replace('(unsigned int)base', '(uintptr_t)base')
        source = source.replace('D_8019A77E[],', '')
        prefix = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
static void transfer(int,int),load_vector(void *),command(void);
static int read_ir(int);
#define D_8019A77E ((short *)&D_8019A778[0].pad)
#define gte_ctc2_0(x) transfer(0,x)
#define gte_ctc2_1(x) transfer(1,x)
#define gte_ctc2_2(x) transfer(2,x)
#define gte_ctc2_3(x) transfer(3,x)
#define gte_ctc2_4(x) transfer(4,x)
#define gte_ctc2_5(x) transfer(5,x)
#define gte_ctc2_6(x) transfer(6,x)
#define gte_ctc2_7(x) transfer(7,x)
#define gte_ldv0(p) load_vector(p)
#define gte_rtv0tr_mac() command()
#define gte_mfc2_9(x) ((x)=read_ir(0))
#define gte_mfc2_10(x) ((x)=read_ir(1))
#define gte_mfc2_11(x) ((x)=read_ir(2))
'''
        harness = prefix + source + r'''
Actor *D_800F32D0;
Instance *D_8019A774,*g_PlayerEntity;
Vector D_8019A778[2];
short D_8019A798,D_800966EC[8192];
volatile unsigned char D_8019A79A,D_8019A79C;
static Actor actor,otherActor;
static Instance instance,model,otherModel,player,other;
static Owner owner;
static Action action;
static Matrix matrices[16],otherMatrices[16];
static int phase,iteration,angles,turns,expectedSpeed,expectedBias;
static void transfer(int index,int value) {
    assert(phase++==index);
    assert(value==(iteration ? 200:100)+index);
}
static void load_vector(void *p) {
    Vector *v=p;
    assert(phase++==8 && v->x==(iteration ? 112:-112) && v->y==0 && v->z==-80);
    /* Target leaves pad uninitialized; do not read it. */
}
static void command(void) {
    assert(phase++==9);
    D_8019A774=&otherModel;
    D_800F32D0=&otherActor;
}
static int read_ir(int index) {
    int value=index==0 ? -32768 : index==1 ? 32767 : -123;
    assert(phase++==10+index);
    if(index==2) { phase=0; iteration++; }
    return value;
}
int FieldEng_VecToAngle(Vector *a,Vector *b) {
    assert(iteration==2 && angles++==0 && a==&player.position && b==&instance.position);
    instance.yaw=-777;
    return 0x12348001;
}
int FieldEng_TurnToward(int current,int target,int speed) {
    assert(turns++==0 && angles==1 && current==-777);
    assert(target==(int16_t)(0x8001+expectedBias) && speed==expectedSpeed);
    return -1234;
}
static void reset(void) {
    memset(&instance,0,sizeof(instance));
    actor.instance=&instance; otherActor.instance=&other; D_800F32D0=&actor;
    instance.owner=&owner; owner.action=&action; owner.flags=0x13579;
    instance.animation=11; instance.length=42; instance.yaw=123;
    model.transforms=matrices; otherModel.transforms=otherMatrices;
    D_8019A774=&model; g_PlayerEntity=&player;
    D_8019A79A=0; D_8019A79C=93; action.state=1;
    phase=iteration=angles=turns=0;
    memset(D_8019A778,0x55,sizeof(D_8019A778));
}
int main(void) {
    int frame,repeat,index,sample,doTurn,result,i,animation,state;
    for(i=0;i<8;i++) {
        ((int *)&matrices[11])[i]=100+i;
        ((int *)&otherMatrices[15])[i]=200+i;
    }
    reset(); assert(func_80193D6C(0)==0 && iteration==0);
    assert(owner.flags==(0x13579|0x40000000) && D_8019A798==3);
    assert(D_8019A79A==0 && D_8019A79C==0);
    for(frame=-32768;frame<=32767;frame++) for(repeat=-1;repeat<=3;repeat++) {
        reset(); D_8019A798=repeat;
        instance.frame=((uint32_t)(uint16_t)frame<<16)|123;
        index=(((frame-28)*680)&0x3FF8)/2;
        sample=(int16_t)(frame*17); D_800966EC[index]=sample;
        expectedSpeed=sample/32;
        expectedBias=((repeat-(frame>=40))&1) ? 128:-128;
        doTurn=repeat>0 && frame>=28;
        result=repeat<=0 && frame>=41;
        assert(func_80193D6C(1)==result && iteration==2 && phase==0);
        assert(angles==doTurn && turns==doTurn);
        assert(D_8019A798==repeat-(repeat>0 && frame>=40));
        assert(instance.frame==(repeat>0 && frame>=40 ? 0x30000 : ((uint32_t)(uint16_t)frame<<16)|123));
        assert(instance.yaw==(doTurn ? -1234:123));
        assert(action.state==(result ? 4:2) && D_8019A79A==result && D_8019A79C==93);
        for(i=0;i<2;i++) assert(D_8019A778[i].x==-32768 && D_8019A778[i].y==32767 && D_8019A778[i].z==-123 && D_8019A778[i].pad==i);
    }
    for(animation=0;animation<256;animation++) for(state=0;state<256;state++) {
        reset(); instance.animation=animation; action.state=state;
        instance.frame=41u<<16; D_8019A798=0; result=animation==11;
        assert(func_80193D6C(1)==result && iteration==2 && angles==0);
        assert(action.state==(result ? 4:state==1 ? 2:state));
    }
    for(frame=-2;frame<=44;frame++) {
        reset(); instance.owner=0; instance.frame=(uint32_t)(uint16_t)frame<<16; D_8019A798=0;
        assert(func_80193D6C(1)==(frame>=41) && iteration==2 && action.state==1);
    }
    for(i=-3;i<=4;i++) if(i!=0 && i!=1) {
        reset(); D_8019A798=123;
        assert(func_80193D6C(i)==0 && iteration==0 && D_8019A798==123 && action.state==1);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory)/'paired'
            result = subprocess.run(['cc','-std=gnu11','-O2','-fno-strict-aliasing','-x','c','-','-o',str(exe)], input=harness, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)


if __name__ == '__main__':
    unittest.main()
