import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_TransformedFlare.c'


class TransformedFlareTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'tools/old-gcc/cc1').is_file() and shutil.which('mipsel-none-elf-as'), 'PSX tools unavailable')
    def test_target_layout(self):
        source = SOURCE.read_text().split('extern Actor', 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Vector)==8 ? 1:-1];
typedef char b[sizeof(Matrix)==32 ? 1:-1];
typedef char c[OFF(Matrix,position)==20 ? 1:-1];
typedef char d[OFF(Particle,shade)==4 ? 1:-1];
typedef char e[OFF(Instance,yaw)==58 ? 1:-1];
typedef char f[OFF(Actor,instance)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_cpu_logic_at_gte_boundary(self):
        source = SOURCE.read_text().replace('#include "pe1/gte.h"', '')
        source = re.sub(r' asm\("\$\d+"\)', '', source)
        # COP2 boundary stubs check call order and CPU logic, not GTE arithmetic.
        prefix = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
static void load_rotation(void *),load_translation(void *),load_vector(void *),command(void);
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
Matrix D_8019A870;
short D_8019A890,D_8019A892,D_8019A894;
int D_800E27EC,D_800966EC[4096],D_800F3428,D_8019A690[4];
unsigned short D_800F336C,D_800E1204[65536];
short D_800F336A;
static Actor actor,otherActor;
static Instance instance,other;
static Particle particle,snapshot;
static Vector *vector;
static int step,ir[3],timerBefore,nextTimer,nextScale,nextShade,wantPalette;
static unsigned short clut;
static int half(uint32_t x) { x&=65535; return x<32768 ? (int)x:(int)x-65536; }
static int asr5(int x) { return x>=0 ? x/32 : -((-x+31)/32); }
static void load_rotation(void *p) { assert(step++==0 && p==&D_8019A870); }
static void load_translation(void *p) { assert(step++==1 && p==&D_8019A870); }
static void load_vector(void *p) {
    vector=p; assert(step++==2 && vector->x==0 && vector->y==particle.y && vector->z==particle.z);
    vector->pad=0x2468;
}
static void command(void) { assert(step++==3); D_800F32D0=&otherActor; }
static int read_ir(int i) { assert(step++==4+i); return ir[i]; }
unsigned short GetClut(int x,int y) {
    assert(step++==7 && x==0 && y==wantPalette);
    D_800E27EC=nextTimer; D_800F336A=nextScale; particle.shade=nextShade;
    snapshot=particle; return clut;
}
void func_800CEE20(Vector *p,Vector *r,int size,int scale,int texture,int palette,int one,int shade,void *colors) {
    assert(step++==8 && p==vector && size==0x2200 && scale==4096);
    assert(p->x==half((uint32_t)ir[0]+D_8019A890));
    assert(p->y==half((uint32_t)ir[1]+D_8019A892));
    assert(p->z==half((uint32_t)ir[2]+D_8019A894) && p->pad==0x2468);
    assert(r->x==half((uint32_t)timerBefore*384) && r->y==other.yaw && r->z==0 && r->pad==1);
    assert(texture==nextScale*(nextTimer&7)+64 && palette==clut && one==1 && shade==nextShade);
    assert(colors==&D_8019A690[nextTimer&3]);
}
int main(void) {
    int i,phase,event,result,edge,timers[]={INT_MIN,-32769,-1,0,7,8,INT_MAX};
    actor.instance=&instance; otherActor.instance=&other;
    for(i=0;i<65536;i++) D_800E1204[i]=65535-i;
    for(i=0;i<65536;i++) {
        unsigned index; uint32_t word=0x5A5A0000u|i;
        D_800E27EC=i-32768; index=((unsigned)D_800E27EC<<8)&0xF00;
        memcpy(&D_800966EC[index],&word,4);
        particle.y=123; particle.z=-456; particle.shade=777; snapshot=particle; step=0;
        result=D_800E27EC>=8;
        assert(func_801981D0(1,&particle)==result && step==0);
        if(!result) snapshot.shade=asr5(half(i));
        assert(memcmp(&snapshot,&particle,sizeof(particle))==0);
        D_800E27EC=-1; memcpy(&D_800966EC[0xF00],&word,4);
        assert(func_801981D0(1,&particle)==0 && particle.shade==asr5(half(i)));
    }
    for(i=0;i<65536;i++) for(phase=0;phase<8;phase++) {
        step=0; D_800F32D0=&actor; D_800E27EC=timerBefore=i-32768;
        particle.y=half(i); particle.z=half(65535-i); particle.shade=321;
        ir[0]=i; ir[1]=-i-1; ir[2]=i*30001;
        D_8019A890=half(i*7); D_8019A892=half(i*11); D_8019A894=half(i*19);
        other.yaw=half(i*23); instance.yaw=~other.yaw;
        D_800F336C=i; D_800F3428=phase&1; D_800F336A=123;
        wantPalette=D_800E1204[i]+(i==4 && D_800F3428 ? 4:0);
        nextTimer=(i-32768)*8+phase; nextScale=half(i); nextShade=half(i*29); clut=i;
        assert(func_801981D0(2,&particle)==0 && step==9);
        assert(memcmp(&snapshot,&particle,sizeof(particle))==0);
    }
    for(edge=0;edge<7;edge++) for(event=-3;event<=5;event++) if(event!=2) {
        step=0; D_800E27EC=timers[edge]; snapshot=particle;
        assert(func_801981D0(event,&particle)==(event==1 && D_800E27EC>=8) && step==0);
        if(event!=1 || D_800E27EC>=8) assert(memcmp(&snapshot,&particle,sizeof(particle))==0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory)/'flare'
            result = subprocess.run(['cc', '-std=gnu11', '-O2', '-fno-strict-aliasing', '-x', 'c', '-', '-o', str(exe)], input=harness, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)
