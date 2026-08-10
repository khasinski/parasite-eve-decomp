import pathlib
import json
import contextlib
import io
import tempfile
import unittest

from tools.scripts import organization_debt


class OrganizationDebtTests(unittest.TestCase):
    def test_scopes_and_classifies_opaque_names(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            (root / "main" / "battle").mkdir(parents=True)
            (root / "overlays" / "room_m001").mkdir(parents=True)
            (root / "main" / "battle" / "func_80012345.c").write_text("")
            (root / "main" / "battle" / "misc7.c").write_text("")
            (root / "overlays" / "room_m001" / "func_80100000.c").write_text("")
            (root / "overlays" / "room_m001" / "door_effect.c").write_text("")

            counts = organization_debt.collect(root)

        self.assertEqual(counts["main"], {
            "address_files": 1,
            "placeholder_files": 1,
        })
        self.assertEqual(counts["overlays"], {
            "address_files": 1,
            "placeholder_files": 0,
        })

    def test_check_rejects_new_placeholder_file(self):
        counts = {
            "main": {"address_files": 0, "placeholder_files": 2},
            "overlays": {"address_files": 0, "placeholder_files": 0},
        }
        baseline = {
            "main": {"address_files": 0, "placeholder_files": 1},
            "overlays": {"address_files": 0, "placeholder_files": 0},
        }
        with tempfile.TemporaryDirectory() as tmp:
            path = pathlib.Path(tmp) / "baseline.json"
            path.write_text(json.dumps(baseline))
            with contextlib.redirect_stdout(io.StringIO()):
                result = organization_debt.check(counts, path)

        self.assertEqual(result, 1)

    def test_check_rejects_stale_baseline_after_rename(self):
        counts = {
            "main": {"address_files": 0, "placeholder_files": 0},
            "overlays": {"address_files": 0, "placeholder_files": 0},
        }
        baseline = {
            "main": {"address_files": 1, "placeholder_files": 0},
            "overlays": {"address_files": 0, "placeholder_files": 0},
        }
        with tempfile.TemporaryDirectory() as tmp:
            path = pathlib.Path(tmp) / "baseline.json"
            path.write_text(json.dumps(baseline))
            with contextlib.redirect_stdout(io.StringIO()):
                result = organization_debt.check(counts, path)

        self.assertEqual(result, 1)


if __name__ == "__main__":
    unittest.main()
