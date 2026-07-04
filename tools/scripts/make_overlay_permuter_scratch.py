#!/usr/bin/env python3
"""Create a decomp-permuter scratch from overlay function queues."""

from __future__ import annotations

import argparse
import csv
import json
import shutil
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
DEFAULT_STATUS = Path("docs/assets/overlay-function-status.csv")
DEFAULT_C_TARGET_QUEUE = Path("docs/assets/overlay-c-target-queue.csv")
SCRATCH_ROOT = Path("tools/decomp-permuter")


def read_rows(path: Path) -> list[dict[str, str]]:
    with path.open(newline="") as f:
        return list(csv.DictReader(f))


def select_row(
    rows: list[dict[str, str]],
    func: str | None,
    rank: str | None,
    c_rank: str | None = None,
    c_target_rows: list[dict[str, str]] | None = None,
) -> dict[str, str]:
    if func:
        matches = [row for row in rows if row["symbol"] == func]
    elif c_rank:
        if c_target_rows is None:
            raise SystemExit("C_RANK selection requires overlay-c-target-queue rows")
        c_matches = [row for row in c_target_rows if row["rank"] == str(c_rank)]
        if not c_matches:
            raise SystemExit(f"no overlay c-target row matches C_RANK={c_rank}")
        if len(c_matches) > 1:
            raise SystemExit(
                "C_RANK selector is ambiguous: "
                + ", ".join(row["symbol"] for row in c_matches)
            )
        c_row = c_matches[0]
        matches = [
            row
            for row in rows
            if row["overlay_id"] == c_row["overlay_id"] and row["symbol"] == c_row["symbol"]
        ]
    elif rank:
        matches = [row for row in rows if row["asm_function_rank"] == str(rank)]
    else:
        if c_target_rows is not None:
            return select_row(rows, func=None, rank=None, c_rank="1", c_target_rows=c_target_rows)
        matches = [row for row in rows if row["asm_function_rank"] == "1"]

    if not matches:
        if func:
            selector = f"FUNC={func}"
        elif c_rank:
            selector = f"C_RANK={c_rank}"
        else:
            selector = f"RANK={rank or 1}"
        raise SystemExit(f"no overlay function-status row matches {selector}")
    if len(matches) > 1:
        raise SystemExit("selector is ambiguous: " + ", ".join(row["symbol"] for row in matches))
    row = matches[0]
    if row["status"] != "asm" or row["kind"] != "function":
        raise SystemExit(f"{row['symbol']} is not an ASM-backed overlay function")
    return row


def source_for(row: dict[str, str]) -> Path | None:
    path = ROOT / "src" / "overlays" / row["overlay_id"] / f"{row['symbol']}.c"
    return path if path.exists() else None


def asm_source_for(row: dict[str, str]) -> Path | None:
    object_path = row["object"]
    marker = "/asm/USA/overlays/"
    if marker not in object_path or not object_path.endswith(".s.o"):
        return None
    rel = "asm/USA/overlays/" + object_path.split(marker, 1)[1][:-2]
    path = ROOT / rel
    return path if path.exists() else None


def write_base_c(path: Path, row: dict[str, str], source: Path | None) -> None:
    if source is not None:
        text = source.read_text()
        path.write_text(text)
        return

    symbol = row["symbol"]
    path.write_text(
        "\n".join(
            [
                "typedef signed char s8;",
                "typedef unsigned char u8;",
                "typedef short s16;",
                "typedef unsigned short u16;",
                "typedef int s32;",
                "typedef unsigned int u32;",
                "",
                f"/* Overlay: {row['overlay_id']}, VRAM {row['vram']}, ROM offset {row['rom_offset']}, {row['size_bytes']} bytes. */",
                "/* Replace this stub with a candidate decompilation, then run the permuter. */",
                f"s32 {symbol}(void) {{",
                "    return 0;",
                "}",
                "",
            ]
        )
    )


def write_compile_sh(path: Path) -> None:
    cc = ROOT / "tools" / "scripts" / "cc.sh"
    path.write_text(
        "\n".join(
            [
                "#!/usr/bin/env bash",
                f'exec "{cc}" "$1" "$3"',
                "",
            ]
        )
    )
    path.chmod(0o755)


def write_metadata(
    path: Path,
    row: dict[str, str],
    source: Path | None,
    asm_source: Path | None,
    c_rank: str | None,
) -> None:
    def rel(path: Path | None) -> str | None:
        if path is None:
            return None
        return str(path.relative_to(ROOT))

    data = {
        "overlay_id": row["overlay_id"],
        "function": row["symbol"],
        "asm_function_rank": row["asm_function_rank"],
        "c_target_rank": c_rank,
        "vram": row["vram"],
        "rom_offset": row["rom_offset"],
        "size_bytes": int(row["size_bytes"]),
        "status_row_object": row["object"],
        "target_asm": rel(asm_source),
        "existing_source": rel(source),
        "check_command": row["check_command"],
        "scratch_command": (
            f"make overlay-permuter-scratch C_RANK={c_rank}"
            if c_rank
            else f"make overlay-permuter-scratch RANK={row['asm_function_rank']}"
        ),
    }
    path.write_text(json.dumps(data, indent=2, sort_keys=True) + "\n")


def sanity_compile(scratch: Path) -> bool:
    proc = subprocess.run(
        [str(scratch / "compile.sh"), str(scratch / "base.c"), "-o", "/tmp/_overlay_perm_sanity.o"],
        cwd=ROOT,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        check=False,
    )
    return proc.returncode == 0


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--status", type=Path, default=DEFAULT_STATUS)
    parser.add_argument("--c-target-queue", type=Path, default=DEFAULT_C_TARGET_QUEUE)
    parser.add_argument("--func")
    parser.add_argument("--rank")
    parser.add_argument("--c-rank")
    parser.add_argument("--out-dir", type=Path)
    args = parser.parse_args()

    status_path = args.status if args.status.is_absolute() else ROOT / args.status
    c_target_path = (
        args.c_target_queue
        if args.c_target_queue.is_absolute()
        else ROOT / args.c_target_queue
    )
    rows = read_rows(status_path)
    c_target_rows = read_rows(c_target_path) if c_target_path.exists() else None
    row = select_row(rows, args.func, args.rank, args.c_rank, c_target_rows)
    symbol = row["symbol"]
    scratch = args.out_dir or SCRATCH_ROOT / f"scratch_{symbol}"
    scratch = scratch if scratch.is_absolute() else ROOT / scratch
    scratch.mkdir(parents=True, exist_ok=True)

    obj = ROOT / row["object"]
    if not obj.exists():
        raise SystemExit(f"missing target object; run make overlay-verify first: {obj}")
    shutil.copy2(obj, scratch / "target.o")

    source = source_for(row)
    asm_source = asm_source_for(row)
    if asm_source is not None:
        shutil.copy2(asm_source, scratch / "target.s")
    write_base_c(scratch / "base.c", row, source)
    write_compile_sh(scratch / "compile.sh")
    write_metadata(scratch / "target.meta.json", row, source, asm_source, args.c_rank)
    (scratch / "settings.toml").write_text(
        "\n".join(
            [
                'compiler_type = "gcc"',
                f'func_name = "{symbol}"',
                "",
            ]
        )
    )

    ok = sanity_compile(scratch)
    rel = scratch.relative_to(ROOT)
    if ok:
        print(f"scratch ready: {rel} (func={symbol}, overlay={row['overlay_id']})")
    else:
        print(f"scratch created but base.c does not compile: {rel}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
