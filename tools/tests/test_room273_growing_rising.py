import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_TimedRising.c'


def callback_source():
    source = SOURCE.read_text()
    callback = source.index('int func_801974DC')
    emitter = source.index('int func_80197648')
    return source[:callback] + source[callback:emitter]


class GrowingRisingSprite(unittest.TestCase):
    def test_target_layout(self):
        source = callback_source() + r'''
typedef char layout[sizeof(Particle)==8 &&
    (unsigned long)&((Particle *)0)->y==2 &&
    (unsigned long)&((Particle *)0)->z==4 &&
    (unsigned long)&((Particle *)0)->speed==6 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT / 'tools/scripts/cc.sh'), str(path),
                                     str(path.with_suffix('.o'))], cwd=ROOT,
                                    capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_behavior(self):
        source = re.sub(r' asm\("\$\d+"\)', '', callback_source())
        source = re.sub(r'asm\(""[^;]*;', '', source)
        source = '#include <assert.h>\n#include <limits.h>\n#include <string.h>\n' + source + r'''
int D_800E27EC, D_800F3428, D_800966EC[4096];
short D_800F336A;
unsigned short D_800F336C, D_800E1204[8];
unsigned char D_8019AD70[4];
static Particle particle;
static int draws,cluts,wantPalette,wantSize,wantShade,wantTile;
unsigned short GetClut(int x, int y) {
    assert(cluts++==0 && !draws && x==16 && y==wantPalette);
    D_800E27EC=999;
    D_800F336A=-7;
    memset(D_800966EC,0,sizeof(D_800966EC));
    return 0xF123;
}
void func_800CEE20(Particle *p, void *rotation, int w, int h, int tile,
                  int clut, int flag, int shade, void *color) {
    assert(cluts==1 && draws++==0 && p==&particle && rotation==0);
    assert(w==wantSize && h==wantSize && tile==wantTile);
    assert(clut==0xF123 && flag==1 && shade==wantShade && color==D_8019AD70);
}
int main(void) {
    int times[]={INT_MIN,-1,0,1,31,32,INT_MAX};
    unsigned short values[]={0,1,32767,32768,65535};
    int frames[]={-33,-4,-1,0,1,3,4,31,32,63,64};
    int i,j,k,f,kind,variant;
    for(i=0;i<7;++i) for(j=0;j<5;++j) for(k=0;k<5;++k) {
        D_800E27EC=times[i]; particle.x=12; particle.z=-34;
        particle.y=values[j]; particle.speed=values[k];
        assert(func_801974DC(1,&particle)==(times[i]>=32));
        assert(particle.x==12 && particle.z==-34);
        assert(particle.y==(unsigned short)(times[i]<32 ? values[j]-values[k]:values[j]));
        assert(particle.speed==(unsigned short)(times[i]<32 ? values[k]+1:values[k]));
    }
    assert(!draws && !cluts);
    for(kind=0;kind<8;++kind) for(variant=0;variant<2;++variant)
        for(f=0;f<11;++f) for(j=0;j<5;++j) for(k=0;k<5;++k) {
            unsigned int shadeOffset=((unsigned int)frames[f]<<8)&0x3F00;
            unsigned int sizeOffset=((unsigned int)frames[f]<<7)&0x3F80;
            short sizeSample=(short)values[k];
            D_800E27EC=frames[f]+1;
            D_800966EC[shadeOffset/4]=0x12340000|values[j];
            memcpy((char *)D_800966EC+sizeOffset,&sizeSample,2);
            /* The two sample locations coincide for some wrapped frames. */
            wantShade=(short)D_800966EC[shadeOffset/4]>>5;
            wantSize=sizeSample+8192;
            wantTile=-7*(frames[f]/4)+200;
            D_800F336A=123; D_800F336C=kind; D_800F3428=variant;
            D_800E1204[kind]=65535;
            wantPalette=65535+(kind==4 && variant ? 4:0);
            particle.x=12; particle.y=65530; particle.z=-34; particle.speed=8;
            cluts=draws=0;
            assert(func_801974DC(2,&particle)==0 && cluts==1 && draws==1);
            assert(particle.x==12 && particle.y==65530 && particle.z==-34 && particle.speed==8);
        }
    assert(func_801974DC(0,&particle)==0);
    assert(func_801974DC(-1,&particle)==0);
    assert(func_801974DC(INT_MAX,&particle)==0);
    assert(draws==1 && cluts==1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', '-fno-strict-aliasing',
                            str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)
