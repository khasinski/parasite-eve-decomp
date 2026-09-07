import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_PositionLayers.c'


def callback_source():
    source = SOURCE.read_text()
    callback = source.index('int func_801977F8')
    emitter = source.index('int func_80197A48')
    return source[:callback] + source[callback:emitter]


class TripleLayer(unittest.TestCase):
    def test_target_layout(self):
        source = callback_source() + r'''
typedef char layout[sizeof(Vector)==8 && sizeof(int)==4 &&
    (unsigned long)&((Vector *)0)->x==0 &&
    (unsigned long)&((Vector *)0)->y==2 &&
    (unsigned long)&((Vector *)0)->z==4 &&
    (unsigned long)&((Vector *)0)->pad==6 ? 1:-1];
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
        source = '#include <assert.h>\n#include <string.h>\n#include <limits.h>\n' + source + r'''
int D_800E27EC,D_800F3428,D_800966EC[4096];
unsigned short D_800F336C,D_800E1204[8],D_800942EC;
unsigned char D_8019AB70[4],D_8019AD74[4],D_8019AD78[4];
static int stage,frame,sample1,sample2,expectedPalette,sampleIndex;
static Vector input,original,*positionPtr,*rotationPtr;
static int shade(int sample) { return sample>>21; }
unsigned short GetClut(int x,int y) {
    assert(stage++==0 && x==80 && y==expectedPalette);
    D_800E27EC=19;
    D_800966EC[sampleIndex]=0x12345678;
    memset(&input,0xEE,sizeof(input));
    return 0xFEDC;
}
void func_800CEE20(Vector *p,Vector *r,int w,int h,int tile,int clut,int flag,int color,void *extra) {
    assert(stage++==1 && p!=&input && r!=p);
    assert(memcmp(p,&original,sizeof(original))==0);
    assert(r->x==0 && r->y==0 && r->z==(short)((unsigned int)frame<<8) && r->pad==0);
    assert(w==(short)sample1*3+2048 && h==w && tile==74);
    assert(clut==0xFEDC && flag==1 && color==shade(sample1) && !extra);
    positionPtr=p; rotationPtr=r;
    r->x=11; r->y=12; r->z=-257; r->pad=13;
    p->x=21; p->y=22; p->z=23; p->pad=24;
    D_800966EC[sampleIndex]=sample2;
    D_800E27EC=27;
}
void func_800D004C(Vector *p,int w,int h,int count,Vector *r,int sx,int sy,void *a,void *b,int color,int flag) {
    assert(p==positionPtr && r==rotationPtr && flag==1 && color==shade(sample2));
    if(stage++==2) {
        assert(w==512 && h==512 && count==16 && sx==(short)sample2+1024 && sy==sx);
        assert(a==D_8019AB70 && b==D_8019AD74);
        assert(r->x==11 && r->y==12 && r->z==-129 && r->pad==13);
        assert(p->x==21 && p->y==22 && p->z==23 && p->pad==24);
        r->x=101; r->y=102; r->z=103; r->pad=104;
        p->x=31; p->y=32; p->z=33; p->pad=34;
        D_800942EC=0x8123;
        D_800966EC[sampleIndex]=0;
        D_800E27EC=39;
    } else {
        assert(stage==4 && w==768 && h==768 && count==8 && sx==4096 && sy==4096);
        assert(a==D_8019AD78 && b==D_8019AB70);
        assert(r->x==1024 && r->y==0 && r->z==(short)((unsigned int)frame<<8) && r->pad==1);
        assert(p->x==31 && p->y==(short)0x8123 && p->z==33 && p->pad==34);
    }
}
int main(void) {
    int counters[]={0,1,2,15,16,64,128,257};
    unsigned int samples[]={0,0x7FFFFFFF,0x80008000,0xFFFF0001,0x0020FFFF};
    int i,j,k,kind,variant;
    for(i=0;i<8;++i) for(j=0;j<5;++j) for(k=0;k<5;++k)
        for(kind=3;kind<=4;++kind) for(variant=0;variant<2;++variant) {
            D_800E27EC=counters[i]; frame=counters[i]-1;
            sampleIndex=(((unsigned int)frame<<8)&0x3F00)/4;
            sample1=(int)samples[j]; sample2=(int)samples[k];
            memset(D_800966EC,0x5A,sizeof(D_800966EC)); D_800966EC[sampleIndex]=sample1;
            D_800F336C=kind; D_800F3428=variant;
            D_800E1204[kind]=65534; expectedPalette=65534+(kind==4 && variant ? 4:0);
            input.x=-32768; input.y=32767; input.z=-1; input.pad=0x1234; original=input;
            stage=0;
            assert(func_801977F8(2,&input)==0 && stage==4);
        }
    stage=0;
    for(i=-2;i<=17;++i) { D_800E27EC=i; assert(func_801977F8(1,0)==(i>=16)); }
    D_800E27EC=INT_MIN; assert(func_801977F8(1,0)==0);
    D_800E27EC=INT_MAX; assert(func_801977F8(1,0)==1);
    assert(func_801977F8(0,0)==0 && func_801977F8(3,0)==0);
    assert(func_801977F8(INT_MIN,0)==0 && func_801977F8(INT_MAX,0)==0 && stage==0);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)
