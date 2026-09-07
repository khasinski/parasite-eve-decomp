import pathlib
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_SequenceController.c'


class SequenceTests(unittest.TestCase):
    def test_target_layout(self):
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
            path = pathlib.Path(directory) / 'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)

    def test_cpu_logic_at_gte_boundary(self):
        source = SOURCE.read_text().replace('#include "pe1/gte.h"', '')
        for reg in (2, 7, 12, 13, 14, 16):
            source = source.replace(f' asm("${reg}")', '')
        source = source.replace('asm volatile("" : : : "$2");', 'asm volatile("" : : : "memory");')
        # Stubs assert the COP2 interface only; they do not claim to emulate GTE.
        prefix = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
static void load_rotation(void *), load_translation(void *), load_vector(void *);
static void command(void);
static int read_ir(int);
#define gte_ldrotmatrix(p) load_rotation(p)
#define gte_ldtransmatrix(p) load_translation(p)
#define gte_ldv0(p) load_vector(p)
#define gte_rtv0tr_mac() command()
#define gte_mfc2_9(x) ((x)=read_ir(0))
#define gte_mfc2_10(x) ((x)=read_ir(1))
#define gte_mfc2_11(x) ((x)=read_ir(2))
'''
        harness = prefix + source + r'''
Actor *D_800F32D0;
Instance *D_8019A7F8,*g_PlayerEntity;
Vector D_8019A7A0;
short D_8019A7FC;
volatile short D_8019A7FE,D_8019A800,D_8019A802;
volatile unsigned char D_8019A804,D_8019A805,D_8019A806;
int D_800966EC[4096];
static Actor actor,otherActor;
static Instance instance,other,player;
static Owner owner;
static Action action;
static Matrix matrices[18];
static int phase,angles,turns,expectedSpeed;
static void load_rotation(void *p) { assert(phase++==0 && p==&matrices[17]); }
static void load_translation(void *p) { assert(phase++==1 && p==&matrices[17]); }
static void load_vector(void *p) {
    Vector *v=p; assert(phase++==2 && v->x==0 && v->y==-144 && v->z==128);
}
static void command(void) {
    assert(phase++==3);
    D_800F32D0=&otherActor;
}
static int read_ir(int i) { assert(phase++==4+i); return i==0 ? -32768 : i==1 ? 32767 : -123; }
int FieldEng_VecToAngle(Vector *a,Vector *b) {
    assert(phase==7 && angles++==0 && a==&player.position && b==&instance.position);
    instance.yaw=-777;
    return 0x12348001;
}
int FieldEng_TurnToward(int current,int target,int speed) {
    assert(turns++==0 && angles==1 && current==-777 && target==-32767 && speed==expectedSpeed);
    return -1234;
}
static void reset(void) {
    memset(&instance,0,sizeof(instance)); memset(&other,0,sizeof(other));
    actor.instance=&instance; otherActor.instance=&other; D_800F32D0=&actor;
    owner.action=&action; owner.flags=0x13579;
    instance.owner=&owner; instance.animation=13; instance.length=30; instance.yaw=123;
    other.transforms=matrices; D_8019A7F8=&other; g_PlayerEntity=&player;
    D_8019A804=D_8019A805=D_8019A806=0;
    D_8019A7FE=3; D_8019A800=4; D_8019A802=5;
    D_8019A7A0.pad=987; phase=angles=turns=0; action.state=1;
}
int main(void) {
    int frame,repeat,word,index,result,doTurn,event,state,animation;
    reset(); assert(func_801958DC(0)==0 && phase==0);
    assert(owner.flags==(0x13579|0x40000000) && D_8019A7FC==2);
    assert(!D_8019A804 && !D_8019A805 && !D_8019A806 && !D_8019A7FE && !D_8019A800 && !D_8019A802);
    for(frame=-32768;frame<=32767;frame++) for(repeat=-1;repeat<=2;repeat++) {
        reset(); D_8019A7FC=repeat; instance.frame=((uint32_t)(uint16_t)frame<<16)|123;
        index=((frame-19)*341)&4095; word=(int16_t)(frame*17);
        D_800966EC[index]=(uint16_t)word; expectedSpeed=word<0 ? -((-word)/32):word/32;
        doTurn=repeat>0 && frame>=19;
        result=repeat<=0 && frame>=29;
        assert(func_801958DC(1)==result && phase==7);
        assert(angles==doTurn && turns==doTurn);
        assert(D_8019A7FC==repeat-(repeat>0 && frame>=25));
        assert(instance.frame==(repeat>0 && frame>=25 ? 0x60000 : ((uint32_t)(uint16_t)frame<<16)|123));
        assert(instance.yaw==(doTurn ? -1234:123));
        assert(action.state==(result ? 4:2) && D_8019A804==result);
        assert(D_8019A7A0.x==-32768 && D_8019A7A0.y==32767 && D_8019A7A0.z==-123 && D_8019A7A0.pad==987);
    }
    for(event=-3;event<=5;event++) if(event<0 || event>1) {
        reset(); D_8019A7FC=-9;
        assert(func_801958DC(event)==0 && phase==0 && angles==0 && turns==0);
        assert(action.state==1 && D_8019A7FC==-9 && D_8019A7FE==3 && D_8019A800==4 && D_8019A802==5);
    }
    for(animation=0;animation<256;animation++) for(state=0;state<256;state++) {
        reset(); instance.animation=animation; action.state=state;
        instance.frame=30u<<16; D_8019A7FC=0;
        result=animation==13;
        assert(func_801958DC(1)==result && phase==7 && angles==0 && turns==0);
        assert(action.state==(result ? 4 : state==1 ? 2:state));
        assert(D_8019A804==result);
    }
    for(frame=-1;frame<=31;frame++) {
        reset(); instance.owner=0; instance.frame=(uint32_t)(uint16_t)frame<<16; D_8019A7FC=0;
        result=frame>=29;
        assert(func_801958DC(1)==result && phase==7 && action.state==1);
        assert(D_8019A804==result && angles==0 && turns==0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory)/'sequence'
            result = subprocess.run(['cc','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing','-x','c','-','-o',str(exe)],input=harness,text=True,capture_output=True)
            self.assertEqual(result.returncode,0,result.stderr)
            subprocess.run([str(exe)],check=True,timeout=20)


if __name__ == '__main__':
    unittest.main()
