import concurrent.futures
import os
import pathlib
import shutil
import subprocess
import sys
import tarfile
import tempfile
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]


class StockCompilerInstallTests(unittest.TestCase):
    def test_parallel_install_does_not_overwrite_running_executable(self):
        for script, directory in (("setup_stock_cc1.sh", "old-gcc"),
                                  ("setup_stock_cc281.sh", "old-gcc-281")):
            with self.subTest(script=script), tempfile.TemporaryDirectory() as temp:
                root = pathlib.Path(temp)
                scripts = root / "tools/scripts"
                scripts.mkdir(parents=True)
                installer = scripts / script
                shutil.copy2(ROOT / "tools/scripts" / script, installer)
                target = root / "tools" / directory
                target.mkdir()
                compiler = target / "cc1"
                payload = root / "payload"
                if sys.platform == "darwin":
                    # Relocated Apple system executables may be killed by AMFI.
                    # Linux exercises ETXTBSY with a real running ELF binary.
                    payload.write_text('#!/bin/sh\nexec sleep "$@"\n')
                    payload.chmod(0o755)
                else:
                    shutil.copy(shutil.which("sleep"), payload)
                shutil.copy(payload, compiler)
                inode = compiler.stat().st_ino
                archive = root / "fixture.tar.gz"
                with tarfile.open(archive, "w:gz") as tar:
                    tar.add(payload, arcname="release/cc1")
                    tar.add(shutil.which("true"), arcname="release/cpp")
                fakebin = root / "bin"
                fakebin.mkdir()
                curl = fakebin / "curl"
                curl.write_text('#!/bin/sh\ncp "$FIXTURE" "$3"\n')
                curl.chmod(0o755)
                env = dict(os.environ, FIXTURE=str(archive),
                           PATH=str(fakebin) + os.pathsep + os.environ["PATH"])
                process = subprocess.Popen([str(compiler), "60"])
                try:
                    def install(_):
                        subprocess.run(["bash", str(installer)], env=env, check=True,
                                       capture_output=True, text=True, timeout=30)
                    with concurrent.futures.ThreadPoolExecutor(max_workers=2) as pool:
                        list(pool.map(install, range(2)))
                    self.assertIsNone(process.poll())
                    self.assertNotEqual(inode, compiler.stat().st_ino)
                    self.assertEqual(compiler.read_bytes(),
                                     payload.read_bytes())
                    self.assertTrue(os.access(compiler, os.X_OK))
                    if directory == "old-gcc":
                        self.assertEqual((target / "cpp").read_bytes(),
                                         pathlib.Path(shutil.which("true")).read_bytes())
                    self.assertFalse(list(target.glob(".install.*")))
                finally:
                    process.terminate()
                    process.wait(timeout=10)
