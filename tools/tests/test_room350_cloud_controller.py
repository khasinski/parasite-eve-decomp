import pathlib
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'src/overlays/room_m350/RoomEffect_CloudController.c'


class CloudControllerTests(unittest.TestCase):
    @unittest.skipUnless((ROOT/'tools/old-gcc/cc1').is_file() and shutil.which('mipsel-none-elf-as'), 'PSX tools unavailable')
    def test_target_layout(self):
        source = SOURCE.read_text().split('extern Actor', 1)[0] + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(State)==12 ? 1:-1];
typedef char b[OFF(State,speed)==4 && OFF(State,targetSpeed)==6 ? 1:-1];
typedef char c[OFF(State,heightPhase)==8 && OFF(State,mode)==10 ? 1:-1];
typedef char d[OFF(Actor,instance)==8 ? 1:-1];
typedef char e[OFF(Owner,asset)==8 && OFF(Owner,action)==24 ? 1:-1];
typedef char f[OFF(Instance,animation)==14 && OFF(Instance,length)==15 ? 1:-1];
typedef char g[OFF(Instance,frame)==20 && OFF(Instance,position)==40 ? 1:-1];
typedef char h[OFF(Instance,yaw)==58 && OFF(Instance,transforms)==568 ? 1:-1];
typedef char i[sizeof(Matrix)==32 && OFF(Matrix,position)==20 ? 1:-1];
typedef char j[sizeof(Vector)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'layout.c'
            path.write_text(source)
            result = subprocess.run([str(ROOT/'tools/scripts/cc.sh'), str(path), str(path.with_suffix('.o'))], cwd=ROOT, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_state_machine_and_helper_boundaries(self):
        source = re.sub(r' asm\("\$\d+"\)', '', SOURCE.read_text())
        # Alias the separately named target globals to their actual matrix offsets.
        matrix_globals = 'extern Matrix testMatrix;\n'
        for i in range(9):
            matrix_globals += f'#define D_{0x8019A870+i*2:08X} testMatrix.rotation[{i//3}][{i%3}]\n'
        for i in range(3):
            matrix_globals += f'#define D_{0x8019A884+i*4:08X} testMatrix.position[{i}]\n'
        source = re.sub(r'extern volatile short D_8019A870[^;]*;', matrix_globals, source)
        source = re.sub(r'extern volatile int D_8019A884[^;]*;', '', source)
        # Transform and angle helpers are boundary stubs, not GTE emulation.
        harness = '#include <assert.h>\n#include <stdint.h>\n#include <string.h>\n#include <limits.h>\n' + source + r'''
Actor *D_800F32D0;
Instance *g_PlayerEntity;
volatile unsigned char D_8019A8BE,D_8019A8C0;
short D_8019A89E,D_8019A8B8,D_8019A8BA,D_8019A8BC,D_800942EC,D_800966EC[8192];
Matrix testMatrix;
static Actor actor,otherActor;
static Instance instance,otherInstance,players[2];
static Matrix transforms;
static Owner owner;
static Action action;
static State state,expected;
static int stage,stopped,turns,angleResult,turnResult,mutate,expectedSpeed;
static int expectedYaw,expectedPosition[3],expectedFrame,expectedRepeat,expectedAction,expectedStop,expectedAux;
static int word(uint32_t x) { int r; memcpy(&r,&x,4); return r; }
static int half(uint32_t x) { x&=65535; return x<32768 ? (int)x:(int)x-65536; }
int Asset_Find08w(int id,int asset,int x,int y,int z) {
    assert(id==0x5CB && asset==owner.asset);
    assert(x==(short)transforms.position[0] && y==(short)transforms.position[1] && z==(short)transforms.position[2]);
    assert(state.speed==0 && state.targetSpeed==0 && state.heightPhase==0 && state.mode==0);
    assert(D_8019A8BE==0 && D_8019A8C0==0 && D_8019A89E==0);
    return -9876;
}
void Akao_Cmd_21(int handle,int zero) {
    assert(handle==state.asset && zero==0 && D_8019A8BE==1 && stage==0);
    assert(!instance.owner || action.state==4); stopped++;
}
int func_800DFE20(int *a,int *b) {
    assert(stage++==0 && a==players[0].position && b==instance.position);
    assert(instance.frame==(uint32_t)expectedFrame && D_8019A8B8==expectedRepeat);
    if(mutate) { g_PlayerEntity=&players[1]; D_800F32D0=&otherActor; instance.frame=0; }
    return -123456;
}
int FieldEng_VecToAngle(int *a,int *b) {
    assert(stage++==1 && a==players[mutate].position && b==instance.position);
    assert(D_8019A8BA==(short)-123456);
    return angleResult;
}
int FieldEng_TurnToward(int yaw,int angle,int step) {
    assert(stage==2 && yaw==instance.yaw && angle==(angleResult&4095) && step==80);
    assert(D_8019A8BC==(angleResult&4095)); turns++;
    return turnResult;
}
int rcos(int yaw) {
    assert(stage++==2 && yaw==expectedYaw+2048 && state.speed==expectedSpeed);
    if(mutate) instance.yaw=-456;
    return 0x7FFF8123;
}
int rsin(int yaw) {
    assert(stage++==3 && yaw==(mutate ? -456:expectedYaw)+2048);
    if(mutate) state.speed=-1234;
    return -123456789;
}
void ApplyMatrixSV(Matrix *m,Vector *in,Vector *out) {
    assert(stage++==4 && m==&testMatrix && in==out);
    assert(in->x==0 && in->y==0 && in->z==(mutate ? -1234:expectedSpeed));
    assert(m->rotation[0][0]==half(0x7FFF8123) && m->rotation[0][2]==half((uint32_t)-123456789));
    assert(m->rotation[2][0]==half(123456789) && m->rotation[2][2]==half(0x7FFF8123));
    assert(m->rotation[1][1]==4096 && m->rotation[0][1]==0 && m->rotation[1][0]==0);
    assert(m->rotation[1][2]==0 && m->rotation[2][1]==0);
    assert(m->position[0]==0 && m->position[1]==0 && m->position[2]==0);
    out->x=-32768; out->z=32767;
    if(mutate) {
        instance.position[0]=INT_MAX; instance.position[1]=INT_MIN; instance.position[2]=-123;
        state.heightPhase=-2; D_800942EC=-32768;
    }
}
static void reset(void) {
    memset(&instance,0,sizeof(instance)); memset(&state,0,sizeof(state));
    actor.instance=&instance; otherActor.instance=&otherInstance; D_800F32D0=&actor;
    instance.owner=&owner; owner.action=&action; owner.asset=123; instance.transforms=&transforms;
    action.state=1; g_PlayerEntity=&players[0]; stage=stopped=turns=0;
    state.asset=1234; state.heightPhase=16;
    instance.animation=9; instance.length=40; instance.frame=31u<<16; instance.yaw=321;
    instance.position[0]=INT_MAX; instance.position[1]=123456789; instance.position[2]=INT_MIN;
    D_8019A8B8=1; D_8019A8BE=77; D_8019A8C0=88; D_8019A89E=99;
    D_800942EC=123; angleResult=321; turnResult=-321; mutate=0;
}
static void run(void) {
    int frame=(short)(instance.frame>>16),phase,mode,delta,result=0,k,expectedTurns=0;
    memcpy(&expected,&state,sizeof(state)); expectedYaw=instance.yaw;
    expectedFrame=instance.frame; expectedRepeat=D_8019A8B8; expectedAction=action.state;
    expectedStop=D_8019A8BE; expectedAux=D_8019A8C0;
    for(k=0;k<3;k++) expectedPosition[k]=instance.position[k];
    if(instance.animation!=9) goto verify;
    if(instance.owner && action.state==1) expectedAction=2;
    if(expectedRepeat>0) {
        if(frame>=30) { expectedFrame=0x50000; expectedRepeat=half((uint16_t)expectedRepeat-1); }
    } else if(frame>=instance.length-1) {
        if(instance.owner) expectedAction=4;
        expectedStop=1; result=1; goto verify;
    }
    mode=state.mode; delta=((angleResult&4095)-(uint16_t)instance.yaw)&4095;
    if(mode==0) {
        if(state.speed<=1280) { expectedYaw=half(turnResult); expectedTurns=1; }
        if(state.speed<=state.targetSpeed && ((expectedYaw-(angleResult&4095)+64)&4095)<=256) {
            expected.mode=1; expected.targetSpeed=1536; expectedAux=0;
        }
    } else if(mode==1) {
        if(state.targetSpeed>=state.speed && ((delta-1536)&4095)<1024) {
            expected.mode=0; expected.targetSpeed=512;
        }
    } else expected.targetSpeed=0;
    if(expected.speed<expected.targetSpeed) expected.speed=half((uint16_t)expected.speed+128);
    else if(expected.speed>expected.targetSpeed) expected.speed=half((uint16_t)expected.speed-32);
    expectedSpeed=expected.speed;
    phase=state.heightPhase;
    if(mutate) {
        expected.speed=-1234; phase=-2;
        expectedPosition[0]=INT_MAX; expectedPosition[1]=INT_MIN; expectedPosition[2]=-123;
    }
    expectedPosition[0]=word((uint32_t)expectedPosition[0]+((uint32_t)-32768<<12));
    expectedPosition[2]=word((uint32_t)expectedPosition[2]+((uint32_t)32767<<12));
    if(phase<16) {
        int sample=D_800966EC[(((uint32_t)(phase+1)<<8)&0x3F00)/2];
        int height=(4096-sample)/8-128;
        expected.heightPhase=half(phase+1);
        expectedPosition[1]=word((uint32_t)((mutate ? -32768:D_800942EC)-height)<<16);
    } else if(frame>=31) {
        int floor=word((uint32_t)D_800942EC<<16);
        expected.mode=2;
        expectedPosition[1]=floor<expectedPosition[1] ? word((uint32_t)expectedPosition[1]-0x100000):floor;
    }
verify:
    assert(func_80198CF8(1,&state)==result);
    assert(memcmp(&state,&expected,sizeof(state))==0);
    assert(action.state==expectedAction && D_8019A8BE==expectedStop && D_8019A8C0==expectedAux);
    assert(D_8019A8B8==expectedRepeat && stopped==result && turns==expectedTurns);
    assert(stage==(result || instance.animation!=9 ? 0:5));
    for(k=0;k<3;k++) assert(instance.position[k]==expectedPosition[k]);
}
int main(void) {
    int n,m,j;
    static const int targets[]={-32768,-1536,-1,0,512,1280,1536,32767};
    for(n=0;n<8192;n++) D_800966EC[n]=half((uint32_t)n*12345+0x8000);
    reset(); transforms.position[0]=INT_MIN; transforms.position[1]=INT_MAX; transforms.position[2]=-65537;
    assert(func_80198CF8(0,&state)==0 && state.asset==-9876);
    for(n=0;n<65536;n++) for(m=0;m<3;m++) for(j=0;j<8;j++) {
        reset(); state.speed=half(n); state.targetSpeed=targets[j]; state.mode=m;
        angleResult=n; turnResult=n^0x8765; run();
    }
    for(n=0;n<65536;n++) for(m=-1;m<=1;m++) {
        reset(); instance.frame=(uint32_t)n<<16; instance.length=n&255; D_8019A8B8=m;
        state.mode=2; instance.owner=n&1 ? &owner:0; run();
    }
    for(n=0;n<65536;n++) for(m=0;m<2;m++) {
        reset(); state.heightPhase=half(n); state.mode=2; mutate=m;
        instance.position[1]=word((uint32_t)n*65537); D_800942EC=half(n); run();
    }
    for(n=0;n<256;n++) { reset(); instance.animation=n; run(); }
    reset(); assert(func_80198CF8(-1,&state)==0 && func_80198CF8(2,&state)==0 && stage==0);
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
