import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_AttachedCloud.c'


class AttachedCloudCallbackTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'tools/old-gcc/cc1').is_file() and shutil.which('mipsel-none-elf-as'), 'PSX tools unavailable')
    def test_target_layout(self):
        source = SOURCE.read_text().split('extern int', 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Vector)==8 ? 1:-1];
typedef char b[sizeof(Particle)==20 ? 1:-1];
typedef char c[OFF(Particle,position)==4 ? 1:-1];
typedef char d[OFF(Particle,size)==10 ? 1:-1];
typedef char e[OFF(Particle,animatedSize)==12 ? 1:-1];
typedef char f[OFF(Particle,firstShade)==14 ? 1:-1];
typedef char g[OFF(Particle,secondShade)==16 ? 1:-1];
typedef char h[OFF(Particle,reserved)==18 ? 1:-1];
typedef char i[sizeof(TrigEntry)==4 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_update_and_draw_boundaries(self):
        source = SOURCE.read_text().replace(' asm("$2")', '')
        # EC and EE are two halfwords of the same target trig table.
        source = source.replace('D_800966EC[],D_800966EE[]', 'D_800966EC[]')
        harness = '#include <assert.h>\n#include <stdint.h>\n#include <string.h>\n#include <limits.h>\n#define D_800966EE (D_800966EC+1)\n' + source + r'''
int D_800E27EC,D_800F3428;
short D_800966EC[8192],D_800F336A;
unsigned short D_800F336C,D_800E1204[65536];
int D_8019A3D0[4];
static Particle particle;
static int anchor[3],draws,cluts,first,mutate;
static short expectedPosition[3];
static int word(uint32_t x) { int result; memcpy(&result,&x,4); return result; }
static int half(uint32_t x) { x &= 65535; return x<32768 ? (int)x:(int)x-65536; }
static int asr5(int x) { return x>=0 ? x/32:-((-x+31)/32); }
unsigned short GetClut(int x,int y) {
    int expected=D_800E1204[D_800F336C];
    if(D_800F336C==4 && D_800F3428) expected+=4;
    assert(x==32 && y==expected && draws==cluts);
    cluts++;
    if(mutate) {
        particle.size=-123; particle.animatedSize=234;
        particle.firstShade=-321; particle.secondShade=-432;
        D_800F336A=-765;
    }
    return (unsigned short)(0xFFFF-cluts);
}
void func_800CEE20(void *p,int r,int width,int height,int texture,int clut,int one,int shade,void *color) {
    Vector *v=p;
    int firstDraw=first && draws==0;
    assert(cluts==draws+1 && r==0 && one==1 && color==D_8019A3D0);
    assert(v->x==expectedPosition[0] && v->y==expectedPosition[1] && v->z==expectedPosition[2]);
    assert(width==(firstDraw ? particle.size:particle.animatedSize) && height==width);
    assert(shade==(firstDraw ? particle.firstShade:particle.secondShade));
    assert(texture==D_800F336A*2+216 && clut==(unsigned short)(0xFFFF-cluts));
    draws++;
    if(mutate) {
        D_800F336C=4; D_800F3428=-1; D_800E1204[4]=65535;
        particle.position[0]=123; anchor[1]=INT_MAX;
    }
}
static void update(int timer,int size) {
    Particle before;
    int shade,product;
    memset(&particle,0x5A,sizeof(particle)); particle.size=size;
    memcpy(&before,&particle,sizeof(particle));
    D_800E27EC=timer;
    assert(func_80198860(1,&particle)==(timer>=8 ? 1:0));
    if(timer<8) {
        shade=D_800966EC[1+((((uint32_t)timer*3)<<8)&0x3F00)/2];
        product=D_800966EC[(((uint32_t)timer<<9)&0x3E00)/2]*size;
        before.firstShade=asr5(shade);
        before.secondShade=half(128-((uint32_t)timer<<4));
        before.animatedSize=half(word((uint32_t)product*3)/4096);
    }
    assert(memcmp(&before,&particle,sizeof(particle))==0);
}
int main(void) {
    int n,j,k;
    static const int sizes[]={-32768,-32767,-4096,-1,0,1,4096,32767};
    static const int timers[]={INT_MIN,INT_MIN+1,-65537,-65536,-32769,-1,0,7,8,32768,65535,INT_MAX};
    for(n=0;n<8192;n++) D_800966EC[n]=half((uint32_t)n*12345+0x8000);
    for(n=0;n<65536;n++) for(j=0;j<8;j++) update(half(n),sizes[j]);
    for(n=0;n<65536;n++) for(j=0;j<sizeof(timers)/sizeof(timers[0]);j++) update(timers[j],half(n));
    for(n=0;n<65536;n++) for(mutate=0;mutate<2;mutate++) {
        memset(&particle,0x5A,sizeof(particle));
        particle.anchor=anchor; particle.firstShade=half(n);
        particle.size=half((uint32_t)n*13); particle.animatedSize=half((uint32_t)n*17);
        particle.secondShade=half((uint32_t)n*29);
        for(k=0;k<3;k++) {
            anchor[k]=word((uint32_t)n*0x1234567u+(uint32_t)k*0x7FFFFFFFu);
            particle.position[k]=half((uint32_t)n*31+k);
            expectedPosition[k]=half((uint32_t)anchor[k]+(uint16_t)particle.position[k]);
        }
        D_800F336C=n; D_800E1204[n]=n^0xA357; D_800F3428=n&1;
        D_800F336A=half(n); first=particle.firstShade>0; draws=cluts=0;
        assert(func_80198860(2,&particle)==0 && draws==1+first && cluts==draws);
    }
    assert(func_80198860(0,0)==0 && func_80198860(-1,0)==0 && func_80198860(3,0)==0);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'test.c'
            path.write_text(harness)
            binary = path.with_suffix('')
            result = subprocess.run(['cc', '-std=gnu11', '-O2', '-fno-strict-aliasing', str(path), '-o', str(binary)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)
            subprocess.run([str(binary)], check=True)
