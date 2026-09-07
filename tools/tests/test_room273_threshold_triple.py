import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m273/RoomEffect_ThresholdTriple.c'


class ThresholdTriple(unittest.TestCase):
    def test_target_layout(self):
        source = SOURCE.read_text() + r'''
typedef char layout[sizeof(Effect)==16 && sizeof(AssetState)==8 &&
    (unsigned long)&((AssetState *)0)->pool==4 &&
    (unsigned long)&((Effect *)0)->phase==6 &&
    (unsigned long)&((Effect *)0)->unknown==8 &&
    (unsigned long)&((Context *)0)->pool==8 &&
    (unsigned long)&((StateContext *)0)->state==8 &&
    (unsigned long)&((State *)0)->kind==14 &&
    (unsigned long)&((State *)0)->value22==22 &&
    (unsigned long)&((State *)0)->value26==26 &&
    (unsigned long)&((State *)0)->transform==0x238 &&
    (unsigned long)&((Transform *)0)->x==0x594 &&
    (unsigned long)&((Transform *)0)->z==0x59C ? 1:-1];
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
        source = re.sub(r'asm\(""[^;]*;', '', SOURCE.read_text())
        source = '#include <assert.h>\n#include <string.h>\n#include <limits.h>\n' + source + r'''
static Context context;
static StateContext stateContext;
static State state,replacement;
static Transform transforms[3];
static Effect records[3],*secondaryRecord;
static unsigned char primary[128],otherPrimary[128],secondary[32],asset[16],archive[16];
Context *D_800F33E0=&context;
StateContext *D_800F32D0=&stateContext;
AssetState D_8019AE8C;
void *D_8019AE90=secondary,*D_800B0E64=archive;
unsigned char D_8019AE9A;
unsigned short D_800942EC;
static int init,attempts,aux,updates,renders,failAt;
int func_80193CB8(void) { assert(0); return 0; }
int func_80193B5C(void) { assert(0); return 0; }
void *Asset_FindTable08ByU32Key(void *p,unsigned int key) {
    assert(init++==0 && p==archive && key==0xC5941704); return asset;
}
void func_800C6D5C(void *p,int a,int b) {
    assert(init++==1 && p==asset && D_8019AE8C.asset==asset && a==0 && b==0);
}
int func_800CE560(void *p,int stride,int count,int (*callback)()) {
    assert(init++==2 && p==primary && stride==16 && count==6 && callback==func_80193CB8); return 100;
}
int func_800CE5AC(void **p,int offset,int stride,int count,int (*callback)()) {
    assert(init++==3 && p==&D_8019AE8C.pool && offset==100 && stride==4 && count==2 && callback==func_80193B5C);
    *p=secondary; return 28;
}
void *func_800CE610(void *p) {
    int n;
    if(p==secondary) {
        assert(aux++==0 && attempts==1 && records[0].phase==0);
        assert(records[0].x==-1 && records[0].y==100 && records[0].z==-32768);
        return &secondaryRecord;
    }
    n=attempts++;
    assert(n<3 && p==(n==0 ? primary:otherPrimary));
    context.pool=otherPrimary;
    stateContext.state=&replacement;
    state.transform=&transforms[n]; D_800942EC=100+n;
    return n==failAt ? 0:&records[n];
}
void func_800CE688(void *p) { assert(p==secondary && updates++==0); }
void func_800CE78C(void *p) { assert(p==secondary && renders++==0); }
int main(void) {
    int values[]={-32768,-1,0,1,32767};
    int stop,kind,i,j,f,n;
    context.pool=primary;
    assert(func_80193F30(0)==128 && init==4 && D_8019AE8C.pool==secondary);
    for(n=0;n<3;++n) { transforms[n].x=65535+n; transforms[n].z=32768+n; }
    for(stop=0;stop<2;++stop) for(kind=8;kind<=9;++kind) for(i=0;i<5;++i)
        for(j=0;j<5;++j) for(f=0;f<4;++f) {
            int active=!stop && kind==9 && values[i]>0 && values[j]<=0;
            int successes=active ? (f<3 ? f:3):0;
            D_8019AE9A=stop; state.kind=kind; state.value22=values[i]; state.value26=values[j];
            state.transform=0; stateContext.state=&state; context.pool=primary;
            attempts=aux=updates=renders=0; failAt=f; secondaryRecord=0;
            memset(records,0xA5,sizeof(records));
            assert(func_80193F30(1)==(stop ? 2:0));
            assert(attempts==(active ? (f<3 ? f+1:3):0) && updates==!stop && renders==0);
            assert(aux==(successes>0) && secondaryRecord==(successes ? &records[0]:0));
            for(n=0;n<3;++n) {
                unsigned char untouched[8]; memset(untouched,0xA5,8);
                assert(memcmp(records[n].unknown,untouched,8)==0);
                if(n<successes) {
                    assert(records[n].x==(short)(65535+n) && records[n].y==100+n);
                    assert(records[n].z==(short)(32768+n) && records[n].phase==n*384);
                } else assert(records[n].x==(short)0xA5A5 && records[n].phase==(short)0xA5A5);
            }
        }
    for(stop=0;stop<2;++stop) { D_8019AE9A=stop; renders=0; assert(func_80193F30(2)==0 && renders==1); }
    attempts=updates=renders=0;
    assert(func_80193F30(-1)==0 && func_80193F30(3)==0);
    assert(func_80193F30(INT_MIN)==0 && func_80193F30(INT_MAX)==0);
    assert(!attempts && !updates && !renders && init==4);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=c99', '-O2', str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)
