import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class CdControlWaitTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_both_waiters(self):
        source = (ROOT / "src/main/cdrom/cd_rom4.c").read_text()
        harness = source + r'''
#include <assert.h>
static int commandValue, requestValue, terminal, pending, polls, sends;
static int paramData, resultData;
int Render_AllocParticleNode(int command, void *param, int a, int b) {
    assert(sends++ == 0 && polls == 0);
    assert(command == (commandValue & 255));
    assert(param == &paramData && a == 0 && b == 0);
    return requestValue;
}
int Render_FindParticleEffect(int request, void *result) {
    assert(sends == 1 && request == requestValue && result == &resultData);
    assert(request != 0 && polls <= pending);
    return polls++ < pending ? 256 : terminal;
}
int main(void) {
    int (*functions[])(unsigned char, void *, void *) = {cd_rom4, func_80080DC4};
    const int outcomes[] = {1, 2, 5, 258, 257};
    int f, c, r, p, t;
    for (f = 0; f < 2; ++f) for (c = 0; c < 512; ++c)
    for (r = 0; r < 2; ++r) for (p = 0; p < 4; ++p)
    for (t = 0; t < 5; ++t) {
        int result;
        commandValue = c; requestValue = r ? 7 : 0;
        pending = p; terminal = outcomes[t]; sends = polls = 0;
        result = functions[f](c, &paramData, &resultData);
        assert(sends == 1 && polls == (r ? p + 1 : 0));
        assert(result == (r && ((terminal & 255) == 2)));
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "cd-control-wait"
            result = subprocess.run(
                ["cc", "-I", str(ROOT / "include"), "-include",
                 str(ROOT / "tools/tests/host_psyq.h"), "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
