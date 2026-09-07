import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_ScaledLayer.c'


def callback_source():
    source = SOURCE.read_text()
    callback = source.index('int func_8019353C')
    emitter = source.index('int func_801936F0')
    return source[:callback] + source[callback:emitter]


class ScaledLayerPair(unittest.TestCase):
    def test_target_layout(self):
        source = callback_source() + r'''
typedef char layout[sizeof(Vector)==8 && sizeof(Effect)==12 && sizeof(int)==4 &&
    (unsigned long)&((Vector *)0)->x==0 &&
    (unsigned long)&((Vector *)0)->y==2 &&
    (unsigned long)&((Vector *)0)->z==4 &&
    (unsigned long)&((Vector *)0)->w==6 &&
    (unsigned long)&((Effect *)0)->parameter==8 ? 1:-1];
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
        source = '#include <assert.h>\n#include <string.h>\n#include <limits.h>\n' + callback_source() + r'''
int D_800E27EC;
short D_800966EC[8192];
unsigned short D_800942EC;
unsigned char D_8019AB70[4];
static Effect input,original;
static Vector *savedPosition,*savedRotation;
static int calls,frame,wantedSize,wantedShade;
void func_800D004C(Vector *p,int w,int h,int count,Vector *r,int sx,int sy,void *a,void *b,int shade,int flag) {
    assert(w==128 && h==128 && count==12 && flag==1);
    assert(a==D_8019AB70 && b==input.parameter);
    assert(sx==wantedSize && sy==sx && shade==wantedShade);
    if(calls++==0) {
        assert(p!=&input.position && r!=p);
        assert(memcmp(p,&original.position,sizeof(Vector))==0);
        assert(r->x==1024 && r->y==0 && r->z==(short)(frame*170) && r->w==-1);
        savedPosition=p; savedRotation=r;
        p->x=21; p->y=22; p->z=23; p->w=24;
        r->x=31; r->y=32; r->z=33; r->w=34;
        D_800942EC=0x8123;
        D_800E27EC=7;
        memset(D_800966EC,0x55,sizeof(D_800966EC));
        wantedSize=(short)(wantedSize*3/2); wantedShade>>=1;
    } else {
        assert(calls==2 && p==savedPosition && r==savedRotation);
        assert(p->x==21 && p->y==(short)0x8123 && p->z==23 && p->w==24);
        assert(r->x==31 && r->y==32 && r->z==33 && r->w==34);
        D_800942EC=0x7654;
    }
}
int main(void) {
    int counters[]={-1,0,1,7,8,32,257,32769,65537};
    int lows[]={-32768,-4096,-2049,-1,0,1,4096,32767};
    int highs[]={-32768,-33,-1,0,31,32,32767};
    int scales[]={-16384,-4096,-1,0,1,4096,16384};
    int i,j,k,l;
    for(i=0;i<9;++i) for(j=0;j<8;++j) for(k=0;k<7;++k) for(l=0;l<7;++l) {
        unsigned int bits=((unsigned int)(unsigned short)highs[k]<<16)|(unsigned short)lows[j];
        int index;
        D_800E27EC=counters[i]; frame=(short)(counters[i]-1);
        index=(((unsigned int)frame<<9)&0x3E00)/4;
        memset(D_800966EC,0,sizeof(D_800966EC));
        memcpy((char *)D_800966EC + index * 4,&bits,4);
        memset(&input,0xA5,sizeof(input));
        input.position.x=-32768; input.position.y=32767; input.position.z=-1;
        input.position.w=scales[l]; original=input;
        wantedSize=(short)(((long long)lows[j]*2+4096)*scales[l]/4096);
        wantedShade=highs[k]>>5;
        calls=0;
        assert(func_8019353C(2,&input)==0 && calls==2);
        assert(memcmp(&input,&original,sizeof(input))==0);
    }
    calls=0;
    for(i=-2;i<=9;++i) { D_800E27EC=i; assert(func_8019353C(1,0)==(i>=8)); }
    D_800E27EC=INT_MIN; assert(func_8019353C(1,0)==0);
    D_800E27EC=INT_MAX; assert(func_8019353C(1,0)==1);
    assert(func_8019353C(0,0)==0 && func_8019353C(3,0)==0);
    assert(func_8019353C(INT_MIN,0)==0 && func_8019353C(INT_MAX,0)==0 && calls==0);
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
