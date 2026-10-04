"""Link the candidate in a temporary overlay and compare every retail byte.

First run `make overlay-check OVERLAY=room_m273` to build the other objects.
This does not edit the manifest, production objects, or progress counters.
"""

import hashlib
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[3]
SOURCE = Path(__file__).with_name("RoomEffect_DirectedRings.c")
OVERLAY = "room_m273"
START = 0x79F0
SIZE = 0x554


def verify(source=SOURCE):
    with tempfile.TemporaryDirectory(prefix="pe-directed-rings-") as directory:
        work = Path(directory)
        obj = work / "candidate.o"
        result = subprocess.run(
            [str(ROOT / "tools/scripts/cc.sh"), str(source), str(obj)],
            cwd=ROOT, capture_output=True, text=True,
        )
        if result.returncode:
            raise RuntimeError(result.stdout + result.stderr)
        linkers = ROOT / "linkers/USA/overlays"
        script = (linkers / f"{OVERLAY}.ld").read_text()
        original = (
            "build/USA/overlays/room_m273/asm/USA/overlays/room_m273/"
            "func_801969D8.s.o"
        )
        if original not in script:
            raise RuntimeError("Expected assembly object missing from linker script")
        script = script.replace(original, str(obj))
        linker = work / "candidate.ld"
        linker.write_text(script)
        elf = work / "candidate.elf"
        command = ["mipsel-none-elf-ld", "-EL", "-T", str(linker)]
        for kind in ("funcs_auto", "syms_auto", "extra"):
            command.extend(["-T", str(linkers / f"undefined_{kind}.{OVERLAY}.txt")])
        subprocess.run(command + ["-o", str(elf)], cwd=ROOT, check=True)
        binary = work / "candidate.bin"
        subprocess.run(
            ["mipsel-none-elf-objcopy", "-O", "binary", str(elf), str(binary)],
            cwd=ROOT, check=True,
        )
        actual = binary.read_bytes()
        expected = (ROOT / f"original/USA/overlays/{OVERLAY}.bin").read_bytes()
        if actual != expected:
            differences = sum(a != b for a, b in zip(actual, expected))
            differences += abs(len(actual) - len(expected))
            raise AssertionError(f"Overlay mismatch: {differences} differing bytes")
        # Whole-overlay equality includes relocations and proves the candidate
        # neither moves nor changes surrounding functions and data.
        assert actual[START:START + SIZE] == expected[START:START + SIZE]
        print(f"Candidate range: {SIZE} bytes MATCH")
        print(f"{OVERLAY}: MATCH ({hashlib.sha1(actual).hexdigest()})")


if __name__ == "__main__":
    verify()
