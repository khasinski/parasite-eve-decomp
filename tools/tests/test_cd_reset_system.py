import pathlib
import shutil
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]


class CdResetSystemTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_reset_order_and_preserved_fields(self):
        source = (ROOT / "src/main/cdrom/CdRom_ResetDsReadSystem.c").read_text()
        # Replace the register binding and map the PS1 data page to host RAM.
        source = source.replace('register int *resetPage asm("$1");',
                                'static int *resetPage;')
        source = source.replace('(int *)0x800A0000', 'resetMemory')
        source = source.replace(
            'extern unsigned char D_800A3515[], D_800A3525[], D_800A3535[];',
            '#define D_800A3515 ((unsigned char *)D_800A3510 + 5)\n'
            '#define D_800A3525 ((unsigned char *)D_800A3510 + 21)\n'
            '#define D_800A3535 ((unsigned char *)D_800A3510 + 37)')
        harness = '#include <assert.h>\n#include <string.h>\n' \
                  'static int resetMemory[0x4000 / 4];\n' + source + r'''
int D_800B8AB0[3], D_800A3510[12];
CdQueue D_800A3540[8];
CdRequest D_800A3610[8];
int D_800A3604, D_800A3600, g_CdPendingReadCount, D_800A3690;
static int stage, queues, fill;
static void status_cleared(void) {
    unsigned char *bytes = (unsigned char *)D_800A3510;
    int i, j;
    for (i = 0; i < 3; ++i) {
        assert(D_800B8AB0[i] == 0);
        for (j = 0; j < 16; ++j)
            assert(bytes[i * 16 + j] == (j < 13 ? 0 : fill));
    }
}
void CdRom_AbortCmd(void) {
    assert(stage++ == 0 && queues == 0);
    assert(((unsigned char *)D_800B8AB0)[0] == fill);
}
void CQ_clear_queue(CdQueue *queue) {
    int i, j;
    assert(stage == 1 && queue == &D_800A3540[queues]);
    status_cleared();
    assert(D_800A3600 == 123 && D_800A3604 == 456 && g_CdPendingReadCount == 789);
    for (i = 0; i < 8; ++i) for (j = 0; j < 24; ++j)
        assert(((unsigned char *)&D_800A3540[i])[j] == (i < queues ? 0 : fill));
    memset(queue, 0, sizeof(*queue));
    ++queues;
}
void DS_read_cbready(void) {
    int i, j;
    assert(stage++ == 1 && queues == 8);
    status_cleared();
    assert(D_800A3600 == 0 && D_800A3604 == 0 && g_CdPendingReadCount == 0);
    for (i = 0; i < 8; ++i) for (j = 0; j < 16; ++j)
        assert(((unsigned char *)&D_800A3610[i])[j] == (j < 4 ? 0 : fill));
    for (i = 0; i < sizeof(resetMemory); ++i)
        assert(((unsigned char *)resetMemory)[i] ==
               (i >= 0x3690 && i < 0x3694 ? 0 : fill));
}
void DsReadCallback(void *callback) {
    assert(stage++ == 2 && callback == 0);
}
void CdRom_EnableDsReadSystem(void) { assert(stage++ == 3); }
int main(void) {
    for (fill = 1; fill <= 255; ++fill) {
        memset(D_800B8AB0, fill, sizeof(D_800B8AB0));
        memset(D_800A3510, fill, sizeof(D_800A3510));
        memset(D_800A3540, fill, sizeof(D_800A3540));
        memset(D_800A3610, fill, sizeof(D_800A3610));
        memset(resetMemory, fill, sizeof(resetMemory));
        D_800A3600 = 123; D_800A3604 = 456; g_CdPendingReadCount = 789;
        stage = queues = 0;
        assert(CdRom_ResetDsReadSystem() == 1);
        assert(stage == 4 && queues == 8);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "cd-reset-system"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing",
                 "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
