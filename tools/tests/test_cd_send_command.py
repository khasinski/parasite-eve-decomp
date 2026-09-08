import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class CdSendCommandTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_command_state_transitions(self):
        source = (ROOT / "src/main/cdrom/CdRom_SendCmd.c").read_text()
        source = source.replace(
            '#include "pe1/psyq_cd.h"',
            '#define g_CdRomEventCommandState (*(unsigned char (*)[80])storage.bytes)\n'
            '#define g_CdRomCmdTimeout (*(int *)(storage.bytes + 64))')
        source = source.replace("extern int g_CdRomCmdLongTimeoutTable[];", "")
        harness = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
static union { void *alignment; unsigned char bytes[80]; } storage;
#define D_8009B558 (storage.bytes)
#define timeoutWords ((int *)(storage.bytes + 64))
static int g_CdRomCmdLongTimeoutTable[256];
static int commandValue, statusValue, hasParam, outcome, flushes, copies, sends;
static unsigned char parameters[4] = {0x12, 0x34, 0x56, 0x78};
''' + source + r'''
void CD_flush(void) {
    assert(flushes++ == 0 && copies == 0 && sends == 0);
    assert(storage.bytes[0] == 0xCC);
}
void Util_Copy4(void *dst, const void *src) {
    assert(flushes == 1 && copies++ == 0 && sends == 0);
    assert(dst == storage.bytes + 1 && src == parameters);
    assert(storage.bytes[0] == commandValue);
    memcpy(dst, src, 4);
}
int CD_cw(int command, void *param, int a, int b) {
    int rewritten = (commandValue == 7 && statusValue == 1) ||
                    (commandValue == 8 && statusValue != 1);
    assert(flushes == 1 && copies == hasParam && sends++ == 0);
    assert(command == (rewritten ? 1 : commandValue));
    assert(a == 0 && b == 1);
    /* Only the low 32 bits belong to the PS1 pointer slot. A host pointer
       store is wider, while the original command rewrite clears one word. */
    assert((uint32_t)(uintptr_t)param ==
           ((!rewritten && hasParam) ?
            (uint32_t)(uintptr_t)(storage.bytes + 1) : 0));
    assert(timeoutWords[0] == (g_CdRomCmdLongTimeoutTable[commandValue] ? 960 : 30));
    assert(timeoutWords[1] == 0);
    assert(storage.bytes[40] == 0xCC);
    if (hasParam) assert(memcmp(storage.bytes + 1, parameters, 4) == 0);
    /* The sender's post-call path must reload the command and clear both
       timeout words on failure, even when the low-level call changed them. */
    storage.bytes[0] = 0x5A;
    timeoutWords[0] = 111;
    timeoutWords[1] = 222;
    return outcome;
}
int main(void) {
    const int outcomes[] = {0, 1, -1, 256};
    int c, s, p, t, o;
    for (c = 0; c < 256; ++c) for (s = 0; s < 3; ++s)
    for (p = 0; p < 2; ++p) for (t = 0; t < 2; ++t)
    for (o = 0; o < 4; ++o) {
        int result;
        memset(&storage, 0xCC, sizeof(storage));
        commandValue = c; statusValue = s; hasParam = p; outcome = outcomes[o];
        flushes = copies = sends = 0;
        storage.bytes[51] = s;
        g_CdRomCmdLongTimeoutTable[c] = t ? -7 : 0;
        result = CdRom_SendCmd(c, p ? parameters : 0);
        assert(result == (outcome == 0));
        assert(flushes == 1 && copies == p && sends == 1);
        assert(storage.bytes[40] == (outcome ? 0xCC : 0x5A));
        assert(timeoutWords[0] == (outcome ? 0 : 111));
        assert(timeoutWords[1] == (outcome ? 0 : 222));
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "cd-send-command"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing",
                 "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
