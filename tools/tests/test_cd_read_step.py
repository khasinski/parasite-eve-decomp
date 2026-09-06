import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class CdReadStepTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_timeout_and_completion_state(self):
        source = (ROOT / "src/main/main/Render_StepParticleCallback.c").read_text()
        source = source.replace("extern int D_8009B6B0[];",
            "static int words[7];\n#define D_8009B6B0 (words + 1)")
        harness = source + r'''
#include <assert.h>
#include <stdint.h>
static int ticks[2], calls, finished, notified, initialCount, change, enabled;
static int finalCount;
void (*D_8009B6D0)(int, void *);
static void done(int event, void *payload) {
    assert(finished == 1 && notified++ == 0);
    assert(event == (finalCount < 0 ? 5 : 2));
    assert((uintptr_t)payload == (change ? 456U : 123U));
}
int VSync(int query) {
    assert(query == -1 && calls < 2 && finished == 0);
    assert(words[1] == 1012);
    if (calls == 0) assert(words[2] == initialCount - 1);
    return ticks[calls++];
}
void Save_ProcessDataCallback(void) {
    assert(finished++ == 0);
    if (change) { words[2] = finalCount; words[4] = 456; }
    else assert(words[2] == finalCount);
    D_8009B6D0 = enabled ? done : 0;
}
int main(void) {
    const int counts[] = {-2, 0, 1, 2, 10};
    const int clocks[] = {1299, 1300, 1301};
    const int replacements[] = {-7, 0, 4};
    int i, a, b, c, m, on, n;
    for (i = 0; i < 5; ++i) for (a = 0; a < 3; ++a)
    for (b = 0; b < 3; ++b) for (c = 0; c < 3; ++c)
    for (m = 0; m < 2; ++m) for (on = 0; on < 2; ++on) {
        int remaining, complete, expectedCalls;
        for (n = 0; n < 7; ++n) words[n] = 0;
        words[0] = 3; words[1] = 1000; words[2] = counts[i];
        words[4] = 123; words[6] = 100;
        initialCount = counts[i]; ticks[0] = clocks[a]; ticks[1] = clocks[b];
        calls = finished = notified = 0; change = m; enabled = on;
        remaining = ticks[0] > 1300 ? -1 : initialCount - 1;
        expectedCalls = remaining == 0 ? 1 : 2;
        complete = remaining == 0 || ticks[1] > 1300;
        finalCount = change ? replacements[c] : remaining;
        D_8009B6D0 = on ? 0 : done;
        Render_StepParticleCallback();
        assert(calls == expectedCalls && finished == complete);
        assert(notified == (complete && enabled));
        assert(words[2] == (complete ? finalCount : remaining));
        assert(words[0] == 3 && words[1] == 1012 && words[6] == 100);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "cd-read-step"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
