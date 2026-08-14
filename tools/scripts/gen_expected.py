#!/usr/bin/env python3
"""Build the target-side objects that objdiff compares this tree against.

objdiff needs two objects per translation unit. The base side is what `make
build` compiles from C. The target side has to come from the game itself - if
it came from our own build the comparison would be a mirror and would prove
nothing. This produces the target side by running splat a second time against
an empty source tree, so every function comes back as disassembly of the
retail binary, then assembling those units with the same assembler flags the
base uses.

Symbol names need care because objdiff pairs symbols and relocations by name.
The committed symbol files know only a fraction of the names the C sources
use, so the names the C defines are read back out of the base objects and
placed at their retail addresses - the address a name gets here is computed
from the committed splat manifest, never from the current build's layout,
which need not match the retail image while decompilation is in progress.
Only the names are borrowed. Every byte in the result is disassembled from
the retail binary.

Every object under expected/build/USA mirrors build/USA, so a unit's path
appears once in objdiff.json and both sides differ only in the leading
directory.
"""

import argparse
import concurrent.futures
import pathlib
import re
import shutil
import subprocess
import sys

import yaml
from elftools.elf.elffile import ELFFile

ROOT = pathlib.Path(__file__).resolve().parents[2]
VERSION = "USA"
# Where the executable is loaded. Anything the disassembler names below this
# cannot be a pointer into the image, so it must have been a plain constant.
VRAM_START = 0x80010000

MAP_PLACEMENT = re.compile(
    r"^\s(\.\w+)\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(build/\S+\.o)$", re.MULTILINE
)

# gcc litters every object with bookkeeping labels that never appear in a
# disassembly; naming them would only teach splat bogus symbols. The
# .NON_MATCHING aliases are asm-differ's, declared by include/macro.inc at the
# same address as the symbol they shadow.
GCC_LOCAL = re.compile(
    r"(?:^(?:LM\d+|\$L|__gnu_compiled|gcc2_compiled|_MACRO_INC_GUARD))"
    r"|(?:\.NON_MATCHING$)"
)

# A name splat would have invented anyway. When the C gives the same address a
# real name, the invented spelling in a committed symbol file must not win.
GENERIC_NAME = re.compile(r"^(?:func|D|jtbl|jpt|B|R|rodata|dtail)_[0-9A-Fa-f]+$")

SYMBOL_LINE = re.compile(r"^\s*([A-Za-z_]\w*)\s*=\s*0x([0-9A-Fa-f]+)\s*;(.*)$")

TEXT_TYPES = ("c", "asm", "hasm")
SECTION_TYPES = {".rodata": ".rodata", ".data": ".data", ".sdata": ".sdata"}


def parse_map_placement(text):
    """Objects the map places, in link order: {object: {section: (addr, size)}}."""
    out = {}
    for section, addr, size, obj in MAP_PLACEMENT.findall(text):
        out.setdefault(obj, {})[section] = (int(addr, 16), int(size, 16))
    return out


class Module:
    """One linked binary: the main executable or one overlay."""

    def __init__(self, name, config, map_path, build_prefix, work):
        self.name = name
        self.config = config
        self.map_path = map_path
        self.build_prefix = build_prefix  # e.g. build/USA/ or build/USA/overlays/X/
        self.work = work  # expected/work/<name>

    @property
    def src_lead(self):
        return "src/main/" if self.name == "main" else "src/overlays/%s/" % self.name


def modules(only, overlays):
    out = []
    if only in (None, "main"):
        out.append(main_module())
    if only in (None, "overlays"):
        for cfg in sorted((ROOT / "configs" / VERSION / "overlays").glob("*.yaml")):
            name = cfg.stem
            if overlays and name not in overlays:
                continue
            out.append(
                Module(
                    name,
                    cfg,
                    ROOT / "build" / VERSION / "overlays" / name / ("%s.map" % name),
                    "build/%s/overlays/%s/" % (VERSION, name),
                    "expected/work/overlays/%s" % name,
                )
            )
    return out


def main_module():
    return Module(
        "main",
        ROOT / "configs" / VERSION / "main.yaml",
        ROOT / "build" / VERSION / "main.map",
        "build/%s/" % VERSION,
        "expected/work/main",
    )


def subsegment_slices(config):
    """Retail vram for each named subsegment slice: {name: {section: vram}}.

    The manifest's offsets are file offsets; each code segment maps them to
    vram by one affine shift. This is the retail truth the whole pass leans
    on - it never consults the current build's layout, which may have drifted
    while a unit is mid-decompilation.
    """
    slices = {}
    spans = []
    for segment in config.get("segments", []):
        if not isinstance(segment, dict) or "vram" not in segment:
            continue
        base = segment["vram"] - segment["start"]
        rows = [s for s in segment.get("subsegments", []) if isinstance(s, list)]
        for row in rows:
            if len(row) < 3:
                continue
            offset, kind, name = row[0], row[1], row[2]
            if kind in TEXT_TYPES:
                slices.setdefault(name, {})[".text"] = base + offset
            elif kind in SECTION_TYPES:
                slices.setdefault(name, {})[SECTION_TYPES[kind]] = base + offset
        if rows:
            spans.append((segment["vram"], base + rows[-1][0]))
    return slices, spans


class SymbolTable:
    """Names at retail addresses, merged from every source with one rule set.

    Kept as one flat file for splat so that precedence lives here and not in
    however splat happens to merge multiple files.
    """

    def __init__(self):
        self.by_addr = {}  # addr -> (name, attrs)
        self.names = {}  # name -> addr

    def add(self, name, addr, attrs="", replace_generic=False):
        if GCC_LOCAL.search(name):
            return
        holder = self.by_addr.get(addr)
        if holder is not None:
            if replace_generic and GENERIC_NAME.match(holder[0]) and not GENERIC_NAME.match(name):
                del self.names[holder[0]]
            else:
                return
        if name in self.names:
            # The same name at two addresses would attach to whichever one
            # splat reads last; dropping the newcomer keeps the first honest.
            return
        self.by_addr[addr] = (name, attrs)
        self.names[name] = addr

    def add_file(self, path):
        for line in path.read_text().splitlines():
            match = SYMBOL_LINE.match(line)
            if match:
                # splat reads only //-style trailers; a /* */ comment would
                # break its parser, so anything else is dropped.
                attrs = match.group(3).strip()
                if not attrs.startswith("//"):
                    attrs = ""
                self.add(match.group(1), int(match.group(2), 16), attrs)

    def without_range(self, spans):
        """A copy minus every symbol inside the given vram spans.

        An overlay loads over a slice of the main executable's address space,
        so a main symbol inside the overlay's window would mislabel the
        overlay's own code.
        """
        out = SymbolTable()
        for addr, (name, attrs) in self.by_addr.items():
            if any(lo <= addr < hi for lo, hi in spans):
                continue
            out.add(name, addr, attrs)
        return out

    def write(self, path):
        lines = [
            "// Generated by tools/scripts/gen_expected.py. Do not edit; do not commit."
        ]
        for addr in sorted(self.by_addr):
            name, attrs = self.by_addr[addr]
            suffix = " %s" % attrs if attrs else ""
            lines.append("%s = 0x%08X;%s" % (name, addr, suffix))
        path.write_text("\n".join(lines) + "\n")


def defined_symbols(obj_path):
    """(section, name, offset, type, size) for every defined symbol in one object."""
    out = []
    with obj_path.open("rb") as handle:
        elf = ELFFile(handle)
        table = elf.get_section_by_name(".symtab")
        if table is None:
            return out
        sections = {i: elf.get_section(i).name for i in range(elf.num_sections())}
        for symbol in table.iter_symbols():
            info = symbol["st_info"]
            if info["type"] not in ("STT_FUNC", "STT_OBJECT", "STT_NOTYPE"):
                continue
            shndx = symbol["st_shndx"]
            section = sections.get(shndx) if isinstance(shndx, int) else None
            if section and symbol.name:
                out.append(
                    (section, symbol.name, symbol["st_value"], info["type"],
                     symbol["st_size"])
                )
    return out


def harvest(module, config, table):
    """Place every name the base objects define at its retail address.

    The C is the only place most names exist. The address does not come from
    the build - a unit that does not yet match retail may have shifted its
    neighbours - but from the committed manifest slice the unit occupies.
    Within a unit the offsets of a later function are trusted; they are only
    wrong inside a unit that does not match yet, and that unit's report is
    the one place the error can surface.
    """
    slices, _ = subsegment_slices(config)
    counted = 0
    for name, sections in slices.items():
        if ".text" not in sections:
            continue
        obj = ROOT / (module.build_prefix + module.src_lead + name + ".c.o")
        if not obj.exists():
            continue
        for section, sym, offset, kind, size in defined_symbols(obj):
            vram = sections.get(section)
            if vram is None:
                continue
            # A generic name embeds its own address. When the computed retail
            # address disagrees, the unit's internal layout has drifted from
            # retail; placing the name would collide with the identical name
            # splat auto-invents at the real address and break assembly.
            generic = GENERIC_NAME.match(sym)
            if generic and int(sym.rsplit("_", 1)[1], 16) != vram + offset:
                continue
            notes = []
            if kind == "STT_FUNC" or section == ".text":
                notes.append("type:func")
            if size:
                notes.append("size:0x%X" % size)
            attrs = "// %s" % " ".join(notes) if notes else ""
            table.add(sym, vram + offset, attrs, replace_generic=True)
            counted += 1
    return counted


def linker_symbols(table):
    """Names the linker binds to absolute retail addresses.

    undefined_syms_manual.txt is the project's own registry of extern data,
    and undefined_addr_aliases is generated from splat's auto files; both
    carry retail addresses by construction, so both sides of a data reference
    can call the address by the same name.
    """
    for name in ("undefined_syms_manual.txt", "undefined_addr_aliases.main.txt"):
        path = ROOT / "linkers" / VERSION / name
        if path.exists():
            table.add_file(path)


def target_config(module, symbol_file):
    """The module's split config again, pointed at an empty source tree.

    src_path decides whether splat finds a C file for a subsegment. Aiming it
    somewhere empty is what makes splat emit every function as assembly
    instead of only the ones this tree has not decompiled yet.
    migrate_rodata_to_functions is on so a unit's constants travel with it the
    way they sit inside the compiled C object; left off they would land in no
    unit at all.
    """
    config = yaml.safe_load(module.config.read_text())
    options = config["options"]
    work = module.work
    options["base_path"] = str(ROOT)
    options["asm_path"] = "%s/%s" % (work, options["asm_path"])
    options["src_path"] = "%s/src" % work
    options["build_path"] = "%s/build" % work
    options["ld_script_path"] = "%s/generated.ld" % work
    for key in ("undefined_syms_auto_path", "undefined_funcs_auto_path"):
        if key in options:
            options[key] = "%s/%s" % (work, pathlib.Path(options[key]).name)
    options["migrate_rodata_to_functions"] = True
    options["make_full_disasm_for_code"] = True
    options["disassemble_all"] = True
    options["symbol_addrs_path"] = [str(symbol_file.relative_to(ROOT))]
    # A hasm unit is hand-written assembly this tree keeps in src/, and splat
    # leaves those alone. The target side wants every unit disassembled out
    # of the retail binary, so they go back to being ordinary code here.
    for segment in config.get("segments", []):
        if not isinstance(segment, dict):
            continue
        for sub in segment.get("subsegments", []):
            if isinstance(sub, list) and len(sub) > 1 and sub[1] == "hasm":
                sub[1] = "c"
    return yaml.safe_dump(config, sort_keys=False)


DIFFER_ALIAS = re.compile(r"^nonmatching\s.*$\n?", re.MULTILINE)
CODE_LABEL = re.compile(r"^(glabel|endlabel|dlabel|enddlabel) (\w+)$", re.MULTILINE)


def strip_differ_aliases(text):
    """Remove the `nonmatching` lines splat writes above each function.

    The macro declares a second symbol at the function's own address for
    asm-differ's benefit. A compiled C object never has one, so leaving them
    in gives objdiff two target symbols competing for one base symbol, and it
    refuses to score the unit at all.
    """
    return DIFFER_ALIAS.sub("", text)


def retype_data_in_text(text, kinds):
    """Relabel blocks the base does not type as functions but splat does.

    A hand-written word table can sit inside .text. gcc leaves it untyped or
    types it as an object; splat writes glabel, which types it as a function.
    objdiff keeps functions and data in separate ledgers and cannot pair one
    against the other, so a single such symbol makes the unit unscorable.
    Only the symbol's type changes - the words assemble byte for byte the same.
    """

    def relabel(match):
        macro, name = match.groups()
        kind = kinds.get(name)
        if kind is None:
            return match.group(0)
        opener = macro in ("glabel", "dlabel")
        if kind == "STT_OBJECT":
            return "%s %s" % ("dlabel" if opener else "enddlabel", name)
        # Untyped in the base, so emit the label with no .type and - because
        # gcc leaves these at size zero while enddlabel would measure them -
        # no .size either; a sized symbol pairing a sizeless one is the one
        # shape objdiff refuses to diff at all.
        return ".global %s\n%s:" % (name, name) if opener else ""

    return CODE_LABEL.sub(relabel, text)


AUTO_SYMBOL = re.compile(r"^(D_[0-9A-Fa-f]+) = 0x([0-9A-Fa-f]+);", re.MULTILINE)
HILO = re.compile(r"%(hi|lo)\((D_[0-9A-Fa-f]+)\)")


def invented_constants(auto_syms):
    """{name: value} for addresses splat named that are not addresses at all.

    A lui/addiu pair loading a plain number looks exactly like one loading an
    address, and the disassembler has to guess. Anything below where the
    executables are loaded cannot be a pointer, so the guess was wrong: the
    compiler wrote a constant and the target has to say so too, or the pair
    reads as a relocation the base does not have.
    """
    return {
        name: int(value, 16)
        for name, value in AUTO_SYMBOL.findall(auto_syms)
        if int(value, 16) < VRAM_START
    }


def halves(value):
    """The lui/addiu pair for a constant, the way the assembler splits one."""
    low = value & 0xFFFF
    if low >= 0x8000:
        low -= 0x10000
    return (value - low) >> 16, low


def inline_constant_pairs(text, constants):
    def replace(match):
        part, name = match.groups()
        if name not in constants:
            return match.group(0)
        high, low = halves(constants[name])
        return "0x%X" % high if part == "hi" else "%d" % low

    return HILO.sub(replace, text)


def base_text_types(placement):
    """{symbol: ELF type} for base .text symbols that are not plain functions."""
    kinds = {}
    for obj in placement:
        path = ROOT / obj
        if not path.exists():
            continue
        for section, sym, _offset, kind, _size in defined_symbols(path):
            if section == ".text" and kind != "STT_FUNC":
                kinds[sym] = kind
    return kinds


def run(cmd, **kwargs):
    done = subprocess.run(
        cmd, cwd=ROOT, capture_output=True, text=True, errors="replace", **kwargs
    )
    if done.returncode != 0:
        sys.stderr.write(done.stdout + done.stderr)
        raise SystemExit("failed: %s" % " ".join(str(c) for c in cmd))
    return done.stdout


def object_section_bytes(path):
    """Total allocated non-bss bytes in one object."""
    total = 0
    with path.open("rb") as handle:
        elf = ELFFile(handle)
        for section in elf.iter_sections():
            if section.name in (".text", ".data", ".rodata", ".sdata"):
                total += section["sh_size"]
    return total


def check_coverage(jobs, binary):
    """The units together must account for every loaded byte of the binary.

    objdiff happily reports on a unit list that quietly lost a function - it
    only scores what it is given. The retail file's size is ground truth: the
    bytes of every unit, taken together, must equal the file exactly (the
    PS-EXE header is itself a unit, and bss carries no bytes). A shortfall
    means a unit's disassembly is missing something; an excess means something
    was counted twice.
    """
    total = sum(object_section_bytes(obj) for _, obj in jobs)
    want = binary.stat().st_size
    return total, want


def committed_symbol_files(config):
    """symbol_addrs_path as a list; splat accepts a bare string too."""
    paths = config["options"].get("symbol_addrs_path", [])
    return [paths] if isinstance(paths, str) else list(paths)


def module_symbols(module, config, shared):
    """The symbol file this module's disassembly runs with."""
    table = SymbolTable()
    for path in committed_symbol_files(config):
        table.add_file(ROOT / path)
    harvested = harvest(module, config, table)
    if module.name == "main":
        linker_symbols(table)
        _slices, spans = subsegment_slices(config)
        shared["main"] = table
        shared["main_spans"] = spans
    else:
        _slices, spans = subsegment_slices(config)
        outside = shared["main"].without_range(spans)
        for addr, (name, attrs) in outside.by_addr.items():
            table.add(name, addr, attrs)
    return table, harvested


def process_module(module, shared, assembler, workers):
    map_text = module.map_path.read_text()
    placement = parse_map_placement(map_text)
    prefix = module.build_prefix
    relatives = []
    seen = set()
    for obj in placement:
        if obj.startswith(prefix) and obj not in seen:
            seen.add(obj)
            relatives.append(obj[len(prefix):])

    work = ROOT / module.work
    shutil.rmtree(work, ignore_errors=True)
    work.mkdir(parents=True)
    (work / "src").mkdir()

    base_config = yaml.safe_load(module.config.read_text())
    table, harvested = module_symbols(module, base_config, shared)
    symbol_file = work / "symbols.txt"
    table.write(symbol_file)

    config_path = work / "splat.yaml"
    config_path.write_text(target_config(module, symbol_file))
    run([sys.executable, "-m", "splat", "split", str(config_path.relative_to(ROOT))])

    asm_lead = base_config["options"]["asm_path"]  # e.g. asm/USA/main
    asm_root = work / asm_lead

    # The data blobs hold no code and were never decompiled: both sides
    # assemble the same bytes from the same file. `make split` post-processes
    # the tree's copy (collapse_zero_data) and this pass does not, so leaving
    # them separate makes two spellings of one thing and nothing pairs.
    blobs = 0
    for source in (work / "asm").rglob("*.s"):
        original = ROOT / source.relative_to(work)
        if original.exists():
            shutil.copyfile(original, source)
            blobs += 1

    kinds = base_text_types(placement)
    constants = {}
    for auto in work.glob("undefined_syms_auto*.txt"):
        constants.update(invented_constants(auto.read_text()))

    src_lead = module.src_lead
    jobs = []
    missing = []
    for relative in relatives:
        if relative.startswith("src/"):
            # A unit built from C is <name>.c.o; the disassembly is <name>.s.
            unit = relative[len(src_lead):].removesuffix(".o")
            unit = unit.removesuffix(".c").removesuffix(".s")
            source = asm_root / ("%s.s" % unit)
            fresh = True
        else:
            source = work / relative.removesuffix(".o")
            # Copied from the tree above; splat regenerated the rest.
            fresh = not (ROOT / relative.removesuffix(".o")).exists()
        obj = ROOT / "expected" / prefix.rstrip("/") / relative
        if not source.exists():
            missing.append(str(source.relative_to(ROOT)))
            continue
        if fresh and source.suffix == ".s":
            text = strip_differ_aliases(source.read_text())
            text = retype_data_in_text(text, kinds)
            source.write_text(inline_constant_pairs(text, constants))
        jobs.append((source, obj))

    if missing:
        for name in missing:
            sys.stderr.write("no disassembly for %s\n" % name)
        raise SystemExit(
            "%s: %d unit(s) have no disassembly" % (module.name, len(missing))
        )

    def assemble(job):
        source, obj = job
        obj.parent.mkdir(parents=True, exist_ok=True)
        return run(
            [
                assembler,
                "-EL",
                "-G0",
                "-mips4",
                "-32",
                "-no-pad-sections",
                "-Iinclude",
                "-I%s" % asm_root.relative_to(ROOT),
                "-o",
                str(obj.relative_to(ROOT)),
                str(source.relative_to(ROOT)),
            ]
        )

    with concurrent.futures.ThreadPoolExecutor(max_workers=workers) as pool:
        list(pool.map(assemble, jobs))

    binary = ROOT / base_config["options"]["target_path"]
    total, want = check_coverage(jobs, binary)
    status = "ok" if total == want else "off by 0x%X bytes" % abs(want - total)
    print(
        "%s: %d units (%d from the tree, %d names from the base), 0x%X of 0x%X bytes [%s]"
        % (module.name, len(jobs), blobs, harvested, total, want, status)
    )
    return total == want


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--only", choices=["main", "overlays"])
    parser.add_argument("--overlay", action="append", default=[],
                        help="limit to named overlay(s); repeatable")
    parser.add_argument("--as", dest="assembler", default="mipsel-none-elf-as")
    parser.add_argument("--jobs", type=int, default=8)
    parser.add_argument("--keep-work", action="store_true",
                        help="keep expected/work for debugging")
    args = parser.parse_args(argv)

    wanted = modules(args.only, set(args.overlay))
    ready = [m for m in wanted if m.map_path.exists()]
    for module in wanted:
        if module not in ready:
            sys.stderr.write(
                "%s: no map at %s - build it first\n" % (module.name, module.map_path)
            )
    if not ready:
        raise SystemExit("nothing to do: no module has been built")

    # The overlays' symbol windows are carved out of the main table, so the
    # main harvest always runs even when only overlays are processed.
    shared = {}
    if not any(m.name == "main" for m in ready):
        module = main_module()
        config = yaml.safe_load(module.config.read_text())
        table = SymbolTable()
        for path in committed_symbol_files(config):
            table.add_file(ROOT / path)
        harvest(module, config, table)
        linker_symbols(table)
        shared["main"] = table

    ok = True
    for module in ready:
        ok = process_module(module, shared, args.assembler, args.jobs) and ok
        if not args.keep_work:
            shutil.rmtree(ROOT / module.work, ignore_errors=True)

    if len(ready) != len(wanted):
        raise SystemExit(
            "%d module(s) skipped for lack of a build" % (len(wanted) - len(ready))
        )
    if not ok:
        raise SystemExit(
            "coverage: at least one module does not account for every byte "
            "of its binary"
        )
    print("coverage: every module accounts for every byte of its binary")
    return 0


if __name__ == "__main__":
    sys.exit(main())
