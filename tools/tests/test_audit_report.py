import unittest

from tools.scripts import audit_report


def configured(name, kind):
    return {"name": name, "metadata": {"source_kind": kind}}


def measured(name, code=0, functions=0):
    return {
        "name": name,
        "measures": {"matched_code": str(code), "matched_functions": functions},
    }


class ReportAuditTests(unittest.TestCase):
    def test_only_semantic_c_contributes_to_totals(self):
        config = {"units": [configured("good", "semantic_c"),
                            configured("asm", "asm_constrained")]}
        report = {
            "units": [measured("good", 12, 1), measured("asm")],
            "measures": {"matched_code": "12", "matched_functions": 1},
        }

        errors, code, functions = audit_report.audit(config, report)

        self.assertEqual(errors, [])
        self.assertEqual((code, functions), (12, 1))

    def test_nonsemantic_credit_is_rejected_even_when_totals_add_up(self):
        config = {"units": [configured("asm", "asm_constrained")]}
        report = {
            "units": [measured("asm", 8, 1)],
            "measures": {"matched_code": "8", "matched_functions": 1},
        }

        errors, _code, _functions = audit_report.audit(config, report)

        self.assertTrue(any("asm_constrained received" in error for error in errors))

    def test_missing_report_unit_is_rejected(self):
        config = {"units": [configured("missing", "semantic_c")]}
        report = {"units": [], "measures": {}}

        errors, _code, _functions = audit_report.audit(config, report)

        self.assertTrue(any("omitted" in error for error in errors))


if __name__ == "__main__":
    unittest.main()
