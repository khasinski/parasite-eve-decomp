import pathlib
import json
import contextlib
import io
import tempfile
import unittest

from tools.scripts import crutch_debt


class CrutchDebtTests(unittest.TestCase):
    def test_collects_main_and_overlay_scopes_separately(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            main = root / "main" / "battle"
            overlay = root / "overlays" / "room_m001"
            main.mkdir(parents=True)
            overlay.mkdir(parents=True)
            (main / "a.c").write_text('void a(void) { asm(""); }\n')
            (overlay / "b.c").write_text(
                'void b(void) { register int value asm("$2"); }\n'
            )

            _, _, _, scopes = crutch_debt.collect_debt(root)

        self.assertEqual(scopes["main"]["barriers"], 1)
        self.assertEqual(scopes["main"]["pins"], 0)
        self.assertEqual(scopes["overlays"]["pins"], 1)
        self.assertEqual(scopes["overlays"]["barriers"], 0)

    def test_comments_do_not_count_as_debt(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            source = root / "main" / "test.c"
            source.parent.mkdir(parents=True)
            source.write_text('// register int value asm("$2");\n')

            _, totals, _, _ = crutch_debt.collect_debt(root)

        self.assertEqual(totals["pins"], 0)

    def test_collects_semantic_and_typing_scaffolding(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            source = root / "main" / "test.c"
            source.parent.mkdir(parents=True)
            source.write_text("""
void f(char *p, void *q) {
    int a = *((int *)(p + 0x10));
    int b = M2C_FIELD(q, int *, 4);
    int c = (u32)q;
    int d = ({ 1; });
    (void)a; (void)b; (void)c; (void)d;
}
""")

            _, totals, _, _ = crutch_debt.collect_debt(root)

        self.assertEqual(totals["raw_offset_dereferences"], 1)
        self.assertEqual(totals["field_macros"], 1)
        self.assertEqual(totals["pointer_integer_casts"], 1)
        self.assertEqual(totals["statement_expressions"], 1)

    def test_check_rejects_an_increase_in_one_scope(self):
        scopes = {
            "main": {key: 0 for key in crutch_debt.ORDER},
            "overlays": {key: 0 for key in crutch_debt.ORDER},
        }
        scopes["main"]["externs_in_c"] = 2
        baseline = {
            "main": {key: 0 for key in crutch_debt.ORDER},
            "overlays": {key: 0 for key in crutch_debt.ORDER},
        }
        baseline["main"]["externs_in_c"] = 1
        with tempfile.TemporaryDirectory() as tmp:
            path = pathlib.Path(tmp) / "baseline.json"
            path.write_text(json.dumps(baseline))
            with contextlib.redirect_stdout(io.StringIO()):
                result = crutch_debt.check_baseline(scopes, path)

        self.assertEqual(result, 1)

    def test_check_rejects_a_stale_baseline_after_improvement(self):
        scopes = {
            "main": {key: 0 for key in crutch_debt.ORDER},
            "overlays": {key: 0 for key in crutch_debt.ORDER},
        }
        baseline = {
            "main": {key: 0 for key in crutch_debt.ORDER},
            "overlays": {key: 0 for key in crutch_debt.ORDER},
        }
        baseline["main"]["externs_in_c"] = 1
        with tempfile.TemporaryDirectory() as tmp:
            path = pathlib.Path(tmp) / "baseline.json"
            path.write_text(json.dumps(baseline))
            with contextlib.redirect_stdout(io.StringIO()):
                result = crutch_debt.check_baseline(scopes, path)

        self.assertEqual(result, 1)


if __name__ == "__main__":
    unittest.main()
