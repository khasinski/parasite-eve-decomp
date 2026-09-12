import contextlib
import io
import json
import pathlib
import tempfile
import unittest
from unittest import mock

import yaml

from tools.scripts import progress_report


ROOT = pathlib.Path(__file__).resolve().parents[2]


class ProgressPublicationTests(unittest.TestCase):
    def test_source_only_ci_installs_the_psx_layout_toolchain(self):
        workflow = yaml.load((ROOT / ".github/workflows/ci.yml").read_text(),
                             Loader=yaml.BaseLoader)
        job = workflow["jobs"]["source-only"]
        steps = job["steps"]
        cross_binutils = next(s for s in steps
                              if s.get("name") == "Install cross binutils")
        compiler = next(s for s in steps
                        if "setup_stock_cc1.sh" in s.get("run", ""))
        self.assertIn("binutils-mipsel-linux-gnu", cross_binutils["run"])
        self.assertIn("mipsel-none-elf-$tool", cross_binutils["run"])
        self.assertIn("setup_stock_cc1.sh", compiler["run"])
        self.assertIn("setup_stock_cc281.sh", compiler["run"])
        self.assertIn("setup_maspsx.sh", compiler["run"])
        self.assertIn("setup_gas27.sh", compiler["run"])
        self.assertEqual(job["env"]["PE_CPP"],
                         "${{ github.workspace }}/tools/old-gcc/cpp")

    def test_both_ci_jobs_use_bare_metal_linker_before_building(self):
        workflow = yaml.load((ROOT / ".github/workflows/ci.yml").read_text(),
                             Loader=yaml.BaseLoader)
        for name in ("source-only", "build-and-report"):
            with self.subTest(job=name):
                steps = workflow["jobs"][name]["steps"]
                install = next(i for i, step in enumerate(steps)
                               if "setup_linker.sh" in step.get("run", ""))
                build = next(i for i, step in enumerate(steps)
                             if "make " in step.get("run", ""))
                self.assertLess(install, build)
                self.assertIn('tools/binutils-2.45/bin" >> "$GITHUB_PATH"',
                              steps[install]["run"])

    def test_badges_use_exact_matches_not_fuzzy_or_complete_percent(self):
        with tempfile.TemporaryDirectory() as directory:
            root = pathlib.Path(directory)
            report = root / "report.json"
            badges = root / "badges"
            report.write_text(json.dumps({"measures": {
                "matched_functions": "3", "total_functions": "8",
                "matched_code_percent": 12.345,
                "fuzzy_match_percent": 98.7, "complete_code_percent": 90.0,
            }}))
            with mock.patch.object(progress_report, "REPORT", report), \
                    mock.patch.object(progress_report, "BADGES", badges), \
                    contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(progress_report.main(), 0)
            code = json.loads((badges / "code.json").read_text())
            functions = json.loads((badges / "functions.json").read_text())
            self.assertEqual(code["message"], "12.35%")
            self.assertEqual(functions["message"], "3/8 (37.5%)")

    def test_missing_report_does_not_replace_existing_badges(self):
        with tempfile.TemporaryDirectory() as directory:
            root = pathlib.Path(directory)
            badge = root / "code.json"
            badge.write_text("previous badge")
            with mock.patch.object(progress_report, "REPORT", root / "missing"), \
                    mock.patch.object(progress_report, "BADGES", root):
                with self.assertRaises(SystemExit):
                    progress_report.main()
            self.assertEqual(badge.read_text(), "previous badge")

    def test_ci_checks_badges_before_publishing_report_on_main(self):
        workflow = yaml.load((ROOT / ".github/workflows/ci.yml").read_text(),
                             Loader=yaml.BaseLoader)
        steps = workflow["jobs"]["build-and-report"]["steps"]
        report_index = next(i for i, s in enumerate(steps)
                            if s.get("run") == "make report")
        badge_index = next(i for i, s in enumerate(steps)
                           if "git diff --exit-code -- docs/badges" in s.get("run", ""))
        upload_index = next(i for i, s in enumerate(steps)
                            if s.get("uses", "").startswith("actions/upload-artifact@"))
        self.assertLess(report_index, badge_index)
        self.assertLess(badge_index, upload_index)
        self.assertIn("make progress", steps[badge_index]["run"])
        self.assertIn("make debt", steps[badge_index]["run"])
        upload = steps[upload_index]
        self.assertEqual(upload["with"]["name"], "SLUS_006.62_report")
        self.assertEqual(upload["with"]["path"], "build/USA/report.json")
        self.assertEqual(upload["with"]["if-no-files-found"], "error")
        self.assertEqual(upload["if"],
                         "github.event_name == 'push' && github.ref == 'refs/heads/main'")


if __name__ == "__main__":
    unittest.main()
