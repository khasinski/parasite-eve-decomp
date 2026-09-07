import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_PhasedPointerSprite.c'


class PhasedPointerSprite(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text() + r'''
typedef char layout[sizeof(Effect)==4 && sizeof(Parameters)==8 &&
    (unsigned long)&((Parameters *)0)->value==2 &&
    (unsigned long)&((Parameters *)0)->phase==4 &&
    (unsigned long)&((Parameters *)0)->one==6 &&
    (unsigned long)&((Object *)0)->value==58 &&
    (unsigned long)&((Context *)0)->object==8 ? 1:-1];
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
        source = '#include <assert.h>\n#include <limits.h>\n#include <string.h>\n' + source + r'''
int D_800E27EC,D_800966EC[4096];
Context *D_800F32D0;
unsigned char D_8019ACDC[4],D_8019ACE0[4];
static Object object;
static Context context;
static Effect effect;
static int position,draws,wantSize,wantShade;
static unsigned short wantPhase;
void func_800D0728(void *p,int a,int b,int c,Parameters *params,
                  int w,int h,void *color,void *other,int shade,int flag) {
    assert(p==&position && a==32 && b==96 && c==8 && draws++==0);
    assert(params->zero==0 && params->value==object.value);
    assert(params->phase==wantPhase && params->one==1);
    assert(w==wantSize && h==wantSize && shade==wantShade && flag==1);
    assert(color==D_8019ACDC && other==D_8019ACE0);
}
int main(void) {
    int frames[]={-257,-1,0,1,15,31,32,63,64,255,256,65536};
    int times[]={INT_MIN,-1,0,15,16,INT_MAX};
    unsigned short values[]={0,1,32767,32768,65535};
    int f,j,k,i;
    D_800F32D0=&context; context.object=&object; effect.position=&position;
    memset(object.unknown,0xA5,sizeof(object.unknown));
    for(i=0;i<6;++i) {
        D_800E27EC=times[i];
        assert(func_8019A4CC(1,&effect)==(times[i]>=16));
    }
    assert(draws==0);
    for(f=0;f<12;++f) for(j=0;j<5;++j) for(k=0;k<5;++k) {
        unsigned int so=((unsigned int)frames[f]<<8)&0x3F00;
        unsigned int bo=((unsigned int)frames[f]<<9)&0x3E00;
        short sample=(short)values[j];
        D_800E27EC=frames[f]+1;
        D_800966EC[bo/4]=0x12340000|values[k];
        memcpy((char *)D_800966EC+so,&sample,2);
        wantSize=sample*2+4096;
        wantShade=(short)D_800966EC[bo/4]>>5;
        wantPhase=(unsigned int)frames[f]<<8;
        object.value=values[k]; draws=0;
        assert(func_8019A4CC(2,&effect)==0 && draws==1);
        assert(effect.position==&position && object.value==values[k]);
        for(i=0;i<58;++i) assert(object.unknown[i]==0xA5);
    }
    assert(func_8019A4CC(0,&effect)==0 && func_8019A4CC(-1,&effect)==0);
    assert(func_8019A4CC(INT_MAX,&effect)==0 && draws==1);
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
