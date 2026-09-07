import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_ColoredRisingSprite.c'


class ColoredRisingSprite(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text() + r'''
typedef char layout[sizeof(Particle)==8 && sizeof(Color)==4 &&
    (unsigned long)&((Particle *)0)->y==2 &&
    (unsigned long)&((Particle *)0)->z==4 &&
    (unsigned long)&((Particle *)0)->speed==6 &&
    (unsigned long)&((Color *)0)->unknown==3 ? 1:-1];
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
        source = re.sub(r' asm\("\$\d+"\)', '', SOURCE.read_text())
        source = re.sub(r'asm\(""[^;]*;', '', source)
        source = '#include <assert.h>\n#include <limits.h>\n#include <string.h>\n' + source + r'''
Color D_8018F1E4;
int D_800E27EC, D_800F3428, D_800966EC[4096];
unsigned short D_800F336C, D_800E1204[8];
static Particle particle;
static Color wantColor;
static int draws,cluts,wantPalette,wantSize,wantShade;
static unsigned int shadeOffset;
unsigned short GetClut(int x, int y) {
    assert(cluts++==0 && !draws && x==0 && y==wantPalette);
    D_800E27EC=999;
    memset(D_800966EC,0,sizeof(D_800966EC));
    D_800966EC[shadeOffset/4]=0x12348001;
    memset(&D_8018F1E4,0,sizeof(D_8018F1E4));
    return 0xF123;
}
void func_800CEE20(Particle *p, void *rotation, int w, int h, int tile,
                  int clut, int flag, int shade, Color *color) {
    assert(cluts==1 && draws++==0 && p==&particle && rotation==0);
    assert(w==wantSize && h==wantSize && tile==102);
    assert(clut==0xF123 && flag==1 && shade==wantShade);
    assert(color!=&D_8018F1E4 && memcmp(color,&wantColor,4)==0);
    color->unknown=0;
}
int main(void) {
    int times[]={INT_MIN,-1,0,1,15,16,INT_MAX};
    unsigned short values[]={0,1,32767,32768,65535};
    int frames[]={-33,-4,-1,0,1,3,4,15,16,31,32,63,64};
    int i,j,k,f,kind,variant;
    for(i=0;i<7;++i) for(j=0;j<5;++j) for(k=0;k<5;++k) {
        D_800E27EC=times[i]; particle.x=12; particle.z=-34;
        particle.y=values[j]; particle.speed=values[k];
        assert(func_80195E10(1,&particle)==(times[i]>=16));
        assert(particle.x==12 && particle.z==-34);
        assert(particle.y==(unsigned short)(times[i]<16 ? values[j]-values[k]:values[j]));
        assert(particle.speed==(unsigned short)(times[i]<16 ? values[k]+1:values[k]));
    }
    assert(!draws && !cluts);
    for(kind=0;kind<8;++kind) for(variant=0;variant<2;++variant)
        for(f=0;f<13;++f) for(j=0;j<5;++j) {
            unsigned int sizeOffset=((unsigned int)frames[f]<<8)&0x3F00;
            short sizeSample=(short)values[j];
            shadeOffset=((unsigned int)frames[f]<<9)&0x3E00;
            memset(D_800966EC,0,sizeof(D_800966EC));
            memcpy((char *)D_800966EC+sizeOffset,&sizeSample,2);
            D_800E27EC=frames[f]+1;
            wantSize=sizeSample*2;
            wantShade=(short)0x8001>>6;
            D_800F336C=kind; D_800F3428=variant;
            D_800E1204[kind]=65535;
            wantPalette=65535+(kind==4 && variant ? 9:5);
            D_8018F1E4.r=1; D_8018F1E4.g=128;
            D_8018F1E4.b=255; D_8018F1E4.unknown=37;
            wantColor=D_8018F1E4;
            particle.x=12; particle.y=65530; particle.z=-34; particle.speed=8;
            cluts=draws=0;
            assert(func_80195E10(2,&particle)==0 && cluts==1 && draws==1);
            assert(particle.x==12 && particle.y==65530 && particle.z==-34 && particle.speed==8);
        }
    assert(func_80195E10(0,&particle)==0);
    assert(func_80195E10(-1,&particle)==0);
    assert(func_80195E10(INT_MAX,&particle)==0);
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
