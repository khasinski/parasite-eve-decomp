import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_ThresholdModelTriple.c'


def model_source():
    source = SOURCE.read_text()
    first_function = source.index('int func_80193B5C')
    model = source.index('int func_80193CB8(int mode')
    controller = source.index('int func_80193F30')
    return source[:first_function] + source[model:controller]


class ThresholdModel(unittest.TestCase):
    def test_target_layout(self):
        source = model_source() + r'''
typedef char layout[sizeof(Effect)==16 && sizeof(Matrix)==32 && sizeof(Vector)==16 &&
    (unsigned long)&((Effect *)0)->phase==6 &&
    (unsigned long)&((Effect *)0)->scaleXZ==8 &&
    (unsigned long)&((Effect *)0)->scaleY==10 &&
    (unsigned long)&((Effect *)0)->parameter12==12 &&
    (unsigned long)&((Effect *)0)->unknown14==14 &&
    (unsigned long)&((Matrix *)0)->t==20 ? 1:-1];
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
        source = re.sub(r' asm\("\$\d+"\)', '', model_source())
        source = re.sub(r'asm\(""[^;]*;', '', source)
        source = source.replace('memset(', 'TestMemset(')
        source = '#include <assert.h>\n#include <string.h>\n#include <limits.h>\n' + source + r'''
int D_800E27EC,D_800F3428,D_800966EC[4096];
unsigned short D_800E11FA,D_800E2850[8],D_800F336C,D_800E1204[8];
static unsigned char assets[4][8];
AssetState D_8019AE8C;
static Effect effect;
static int stage,wantedKind,wantedVariant;
static Vector *originalScale;
int GetTPage(int a,int b,int c,int d) {
    assert(stage++==0 && a==0 && b==1 && c==0 && d==0);
    D_800E11FA=3; D_800E2850[3]=0x1200;
    D_800F336C=wantedKind; D_800F3428=wantedVariant;
    return 0x12340056;
}
unsigned short GetClut(int x,int y) {
    assert(stage++==1 && x==0 && y==65534+(wantedKind==4 && wantedVariant ? 11:7));
    return 0xFEDC;
}
void GsSetOrign(int page,int clut) { assert(stage++==2 && page==0x1256 && clut==0xFEDC); }
void func_800C6ED8(int enabled) { assert(stage++==3 && enabled==1); }
void func_800C6EF8(void *p) {
    assert(stage++==4 && p==assets[0]); D_8019AE8C.asset=assets[1]; effect.parameter12=65535;
}
void func_800C6FA0(void *p,int value) {
    assert(stage++==5 && p==assets[1] && value==65535); effect.phase=1024;
}
int rcos(int angle) { assert(stage++==6 && angle==1024); effect.phase=-4096; return 1234; }
int rsin(int angle) { assert(stage++==7 && angle==-4096); return -789; }
void *TestMemset(void *p,int value,unsigned long size) {
    assert(stage++==8 && value==0 && size==16); originalScale=p;
    effect.scaleXZ=-32768; effect.scaleY=12345; return memset(p,value,size);
}
void Gte_ScaleMatrix(Matrix *m,Vector *v) {
    assert(stage++==9 && v!=originalScale);
    assert(v->x==-32768 && v->y==12345 && v->z==-32768 && v->pad==0);
    assert(m->m[0][0]==1234 && m->m[2][2]==1234 && m->m[1][1]==4096);
    assert(m->m[0][2]==-789 && m->m[2][0]==789);
    assert(m->m[0][1]==0 && m->m[1][0]==0 && m->m[1][2]==0 && m->m[2][1]==0);
    assert(m->t[0]==0 && m->t[1]==0 && m->t[2]==0);
    m->m[0][0]=4321; effect.x=11; effect.y=22; effect.z=-333; D_8019AE8C.asset=assets[2];
}
void func_800C71E4(void *p,Matrix *m) {
    assert(stage++==10 && p==assets[2] && m->m[0][0]==4321);
    assert(m->t[0]==11 && m->t[1]==22 && m->t[2]==-333); D_8019AE8C.asset=assets[3];
}
void func_800C6F4C(void *p) { assert(stage++==11 && p==assets[3]); }
int main(void) {
    int frames[]={-32768,-1,0,1,15,31,32,INT_MAX};
    int values[]={-32768,-4097,-1,0,1,4096,32767};
    int i,j,k,l;
    for(i=0;i<8;++i) for(j=0;j<7;++j) for(k=0;k<7;++k) for(l=0;l<7;++l) {
        Effect before;
        unsigned int bits;
        int index,shifted;
        memset(&effect,0xA5,sizeof(effect)); effect.phase=values[j]; before=effect;
        D_800E27EC=frames[i];
        if(frames[i]<32) {
            index=((unsigned int)frames[i]*32)&0xFFF; shifted=(index+1024)&0xFFF;
            memset(D_800966EC,0,sizeof(D_800966EC));
            bits=((unsigned int)(unsigned short)values[k]<<16)|(unsigned short)values[j];
            memcpy(&D_800966EC[index],&bits,4);
            bits=(unsigned int)(unsigned short)values[l]<<16; memcpy(&D_800966EC[shifted],&bits,4);
        }
        assert(func_80193CB8(1,&effect)==(frames[i]>=32));
        if(frames[i]>=32) assert(memcmp(&effect,&before,sizeof(effect))==0);
        else {
            assert(effect.scaleXZ==values[j]/2 && effect.scaleY==(short)((values[l]+4096)*2));
            assert(effect.phase==(short)(values[j]+256) && effect.parameter12==(unsigned short)(values[k]>>6));
            assert(effect.x==before.x && effect.y==before.y && effect.z==before.z && effect.unknown14==before.unknown14);
        }
    }
    for(wantedKind=3;wantedKind<=4;++wantedKind) for(wantedVariant=0;wantedVariant<2;++wantedVariant) {
        memset(&effect,0xA5,sizeof(effect)); stage=0; D_8019AE8C.asset=assets[0];
        D_800E11FA=0; D_800E2850[0]=0; D_800F336C=0;
        D_800E1204[wantedKind]=65534;
        assert(func_80193CB8(2,&effect)==0 && stage==12 && effect.unknown14==0xA5A5);
    }
    stage=0;
    assert(func_80193CB8(0,0)==0 && func_80193CB8(3,0)==0);
    assert(func_80193CB8(INT_MIN,0)==0 && func_80193CB8(INT_MAX,0)==0 && stage==0);
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
