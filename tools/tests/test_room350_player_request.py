import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_FallingTexturePair.c'


def emitter_source():
    source = SOURCE.read_text()
    callback = source.index('int func_801937B4')
    emitter = source.index('int func_801938E4')
    return source[:callback] + 'extern int func_801937B4(int, short *);\n' + source[emitter:]


class PlayerRequestTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'tools/old-gcc/cc1').is_file() and shutil.which('mipsel-none-elf-as'), 'PSX tools unavailable')
    def test_target_layout(self):
        source = emitter_source() + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Particle)==8 ? 1:-1];
typedef char b[sizeof(Transform)==32 ? 1:-1];
typedef char c[OFF(Transform,position)==20 ? 1:-1];
typedef char d[OFF(Instance,transforms)==0x238 ? 1:-1];
typedef char e[OFF(Emitter,pool)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'),str(path),str(path.with_suffix('.o'))],cwd=ROOT,capture_output=True,text=True)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_request_retry_and_player_reload(self):
        harness = '#include <assert.h>\n#include <stdint.h>\n#include <string.h>\n' + emitter_source() + r'''
Instance *g_PlayerEntity;
Emitter *D_800F33E0;
unsigned char D_8019A79A,D_8019A79C;
volatile short D_800F3368,D_800F3376,D_800F3378,D_800F3372,D_800F3374,D_800F336E;
unsigned short D_800F336C;
short D_800F336A;
volatile unsigned short D_800E11FA,D_800F3370;
unsigned short D_800E2850[65536];
static Instance players[2];
static Transform transforms[2];
static Emitter emitter;
static Particle particle,expected;
static int pool,calls,setups,setupResult,fail;
static int half(uint32_t x) { x &=65535; return x<32768 ? (int)x:(int)x-65536; }
int func_801937B4(int e,short *p) { return 0; }
int func_800CE560(void *p,int stride,int count,int (*cb)()) {
    assert(p==&pool && stride==8 && count==4 && cb==func_801937B4);
    setups++; return setupResult;
}
Particle *func_800CE610(void *p) {
    assert(p==&pool); calls++;
    if(fail) return 0;
    g_PlayerEntity=&players[1]; D_8019A79A=255; D_8019A79C=123;
    return &particle;
}
static void reset(void) {
    calls=setups=0; g_PlayerEntity=&players[0]; D_800F33E0=&emitter;
    D_8019A79A=0; D_8019A79C=1;
    memset(&particle,0x5A,sizeof(particle)); expected=particle;
}
static void verify(void) {
    expected.x=half(transforms[1].position[0]); expected.y=half(transforms[1].position[1]);
    expected.z=half(transforms[1].position[2]); expected.frame=0;
    assert(memcmp(&expected,&particle,sizeof(particle))==0);
    assert(D_8019A79A==255 && D_8019A79C==0);
}
int main(void) {
    int stop,request,failure,i,event;
    emitter.pool=&pool; players[0].transforms=&transforms[0]; players[1].transforms=&transforms[1];
    transforms[1].position[0]=0x12348000; transforms[1].position[1]=0x56787FFF; transforms[1].position[2]=-1;
    for(stop=0;stop<256;stop++) for(request=0;request<256;request++) for(failure=0;failure<2;failure++) {
        reset(); D_8019A79A=stop; D_8019A79C=request; fail=failure;
        assert(func_801938E4(1)==(stop ? 2:0));
        assert(calls==(!stop && request!=0) && setups==0);
        if(!stop && request && !fail) verify();
        else {
            assert(D_8019A79A==stop && D_8019A79C==request);
            assert(memcmp(&expected,&particle,sizeof(particle))==0);
        }
    }
    reset(); fail=1;
    assert(func_801938E4(1)==0 && calls==1 && D_8019A79C==1);
    fail=0; assert(func_801938E4(1)==0 && calls==2); verify();
    for(i=0;i<65536;i++) {
        reset(); fail=0;
        transforms[1].position[0]=0x12340000|i;
        transforms[1].position[1]=-i-1; transforms[1].position[2]=i*30001;
        assert(func_801938E4(1)==0 && calls==1); verify();
    }
    for(setupResult=-2;setupResult<=2;setupResult++) {
        reset(); assert(func_801938E4(0)==setupResult && setups==1 && calls==0);
        assert(D_8019A79A==0 && D_8019A79C==1);
    }
    for(i=0;i<65536;i++) {
        reset(); D_800E11FA=i; D_800E2850[i]=65535-i;
        assert(func_801938E4(2)==0 && calls==0 && setups==0);
        assert(D_800F3368==32 && D_800F336A==2 && D_800F3376==32 && D_800F3378==32);
        assert(D_800F336C==3 && D_800F336E==1 && D_800F3372==0 && D_800F3374==0);
        assert(D_800F3370==65535-i && D_8019A79C==1);
    }
    for(event=-3;event<=5;event++) if(event<0 || event>2) {
        reset(); assert(func_801938E4(event)==0 && calls==0 && setups==0 && D_8019A79C==1);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe=pathlib.Path(directory)/'request'
            result=subprocess.run(['cc','-std=gnu11','-O2','-fno-strict-aliasing','-x','c','-','-o',str(exe)],input=harness,text=True,capture_output=True)
            self.assertEqual(result.returncode,0,result.stderr)
            subprocess.run([str(exe)],check=True,timeout=20)
