import pathlib
import re
import subprocess
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[2]
CUSTOM_FLAGS = (
    "stack-return-delay",
    "store-return-delay",
    "la-return-delay",
    "la-call-delay",
    "store-call-delay",
    "store-jump-delay",
    "store-branch-delay",
    "load-dest-temp",
    "label-branch-delay",
    "post-call-jump-target",
)


class StockMaspsxTests(unittest.TestCase):
    def test_custom_maspsx_patches_are_not_present(self):
        patch_dir = ROOT / "tools" / "patches"
        patches = list(patch_dir.glob("*maspsx*.patch")) if patch_dir.exists() else []
        self.assertEqual(patches, [])

    def test_wrapper_and_tracked_text_do_not_use_custom_flags(self):
        pattern = re.compile(r"--(?:%s)(?:=|\b)" % "|".join(CUSTOM_FLAGS))
        offenders = []
        tracked = subprocess.run(
            ["git", "ls-files", "src", "candidates", "tools/scripts"],
            cwd=ROOT,
            check=True,
            capture_output=True,
            text=True,
        ).stdout.splitlines()

        for relative in tracked:
            path = ROOT / relative
            if path.suffix in {".c", ".h", ".md", ".py", ".sh"}:
                if pattern.search(path.read_text(errors="replace")):
                    offenders.append(path.relative_to(ROOT))

        self.assertEqual(offenders, [])

    def test_setup_installs_unmodified_pinned_upstream(self):
        setup = (ROOT / "tools" / "scripts" / "setup_maspsx.sh").read_text()
        self.assertIn('REV="42b862c988fe7a13fe4e7ac0ebec90ed6b9fb763"', setup)
        self.assertNotIn("git apply", setup)
        self.assertNotIn("PATCH=", setup)


if __name__ == "__main__":
    unittest.main()
