import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_SizedLayers.c'


class SizedLayersTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'tools/old-gcc/cc1').is_file() and shutil.which('mipsel-none-elf-as'), 'PSX tools unavailable')
    def test_target_layout(self):
        source = SOURCE.read_text().split('extern int', 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[OFF(Particle,x)==4 ? 1:-1];
typedef char b[OFF(Particle,size)==10 ? 1:-1];
typedef char c[sizeof(TrigEntry)==4 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_signed_math_and_helper_reloads(self):
        # The PSX symbols are two bytes apart in the same trig table.
        source = SOURCE.read_text().replace('extern short D_800966EE[];',
            '#define D_800966EE ((short *)((char *)D_800966EC + 2))')
        harness = '#include <assert.h>\n#include <stdint.h>\n#include <string.h>\n#include <limits.h>\n' + source + r'''
int D_800E27EC,D_800F3428,D_800966EC[4096];
unsigned short D_800F336C,D_800E1204[65536];
short D_800F336A;
int D_8019A3CC[1],D_8019A3C8[1];
static Particle particle,afterClut;
static int calls,wantSize,wantPalette,nextTimer,lastTimer,nextScale;
static unsigned short clut;
static int half(uint32_t x) { x&=65535; return x<32768 ? (int)x:(int)x-65536; }
static int asr5(int x) { return x>=0 ? x/32 : -((-x+31)/32); }
static unsigned short trigHalf(unsigned timer,int high) {
    uint32_t word=(uint32_t)D_800966EC[((timer-1)<<7)&0xF80];
    return high ? word>>16 : word;
}
unsigned short GetClut(int x,int y) {
    assert(calls++==0 && x==48 && y==wantPalette);
    D_800E27EC=nextTimer; D_800F336A=nextScale;
    particle.size=~particle.size; afterClut=particle;
    return clut;
}
void func_800CEE20(void *p,int zero,int sx,int sy,int texture,int palette,int one,int shade,void *owner) {
    assert(calls++==1 && p==&particle.x && owner==&particle);
    assert(zero==0 && sx==wantSize && sy==wantSize && texture==nextScale*5+216);
    assert(palette==clut && one==1 && shade==asr5(half(trigHalf(nextTimer,1))));
    D_800E27EC=lastTimer;
}
void func_800D004C(void *p,int w,int h,int mode,void *vector,int sx,int sy,void *a,void *b,int shade,int one) {
    unsigned index=(((unsigned)lastTimer-1)<<8)&0xF00;
    assert(calls++==2 && p==&particle.x && w==96 && h==96 && mode==8 && !vector);
    assert(sx==wantSize && sy==wantSize && a==D_8019A3CC && b==D_8019A3C8 && one==1);
    assert(shade==asr5(half(D_800966EC[index])));
}
int main(void) {
    int i,j,event,trigger,timers[]={INT_MIN,-32769,-1,0,1,7,8,9,INT_MAX};
    int sizes[]={-32768,-4097,-1,0,1,4095,32767};
    for(i=0;i<65536;i++) D_800E1204[i]=65535-i;
    for(i=0;i<4096;i++) {
        uint32_t word=((uint32_t)(i*1877+5)<<16)|(uint16_t)(i*3119+7);
        memcpy(&D_800966EC[i],&word,4);
    }
    for(i=0;i<65536;i++) for(j=0;j<7;j++) {
        unsigned index;
        uint32_t word;
        calls=0; D_800E27EC=i-32768; particle.size=sizes[j];
        D_800F336C=i; D_800F3428=j&1;
        wantPalette=D_800E1204[i]+(i==4 && D_800F3428 ? 4:0);
        index=((((unsigned)D_800E27EC-1)<<7)&0xF80);
        word=((uint32_t)(uint16_t)(i*19)<<16)|(uint16_t)i;
        memcpy(&D_800966EC[index],&word,4);
        wantSize=(int)((int64_t)half(i)*sizes[j]/4096);
        nextTimer=D_800E27EC+3; lastTimer=D_800E27EC+11;
        nextScale=half(i); clut=i;
        assert(func_8019404C(2,&particle)==0 && calls==3);
        assert(memcmp(&particle,&afterClut,sizeof(particle))==0);
    }
    for(trigger=0;trigger<9;trigger++) for(event=-3;event<=5;event++) if(event!=2) {
        calls=0; D_800E27EC=timers[trigger]; afterClut=particle;
        assert(func_8019404C(event,&particle)==(event==1 && D_800E27EC>=8));
        assert(calls==0 && memcmp(&particle,&afterClut,sizeof(particle))==0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory)/'layers'
            result = subprocess.run(['cc', '-std=gnu11', '-O2', '-fno-strict-aliasing', '-x', 'c', '-', '-o', str(exe)], input=harness, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)
