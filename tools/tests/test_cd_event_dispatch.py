import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class CdEventDispatchTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_dispatch_and_callback_state_updates(self):
        source = (ROOT / "src/main/main/Render_DrawParticleGroup.c").read_text()
        # The retail symbol points inside a larger allocation; model its prefix.
        source = source.replace("extern int D_8009B598[];",
            "static int stateWords[13];\n#define D_8009B598 (stateWords + 11)")
        harness = source + r'''
#include <assert.h>
#include <limits.h>
static int step, eventValue, commandValue, flagsValue, timeoutValue, result;
static void dispatched(int handler, int event, void *data) {
    assert(step++ == 1 && data == &result);
    assert(event == ((flagsValue & 16) ? 5 : (eventValue & 255)));
    assert(handler == (commandValue == 31 ? 1 : commandValue == 32 ? 2 : 3));
    assert(stateWords[11] == 0 && stateWords[12] == 0);
    stateWords[11] = timeoutValue;
    stateWords[12] = 789;
    stateWords[1] = 123;
}
void CdRom_ProcessEventByte(int event) {
    assert(step++ == 0 && event == (eventValue & 255));
    stateWords[11] = 456; stateWords[12] = 456;
    ((unsigned char *)stateWords)[0] = flagsValue;
    stateWords[1] = commandValue;
}
void LIBDS_DSSYS_1_text_8B8(int event, void *data) { dispatched(1, event, data); }
void LIBDS_DSSYS_1_text_A9C(int event, void *data) { dispatched(2, event, data); }
void CdRom_CmdEventCallback(int event, u8 *data) { dispatched(3, event, data); }
int main(void) {
    const int events[] = {INT_MIN, -1, 0, 2, 5, 255, 256, INT_MAX};
    const int commands[] = {-1, 0, 30, 31, 32, 33, 255, INT_MAX};
    const int timeouts[] = {0, 1, -1};
    int a, b, c, f, i;
    for (a = 0; a < 8; ++a) for (b = 0; b < 8; ++b)
    for (f = 0; f < 256; ++f) for (c = 0; c < 3; ++c) {
        for (i = 0; i < 13; ++i) stateWords[i] = 987;
        eventValue = events[a]; commandValue = commands[b]; flagsValue = f;
        timeoutValue = timeouts[c]; step = 0;
        Render_DrawParticleGroup(eventValue, &result);
        assert(step == 2 && stateWords[1] == (timeoutValue ? 123 : 33));
        assert(stateWords[11] == timeoutValue && stateWords[12] == 789);
        for (i = 2; i < 11; ++i) assert(stateWords[i] == 987);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "cd-dispatch"
            result = subprocess.run(
                ["cc", "-I", str(ROOT / "include"),
                 "-include", str(ROOT / "tools/tests/host_psyq.h"), "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
