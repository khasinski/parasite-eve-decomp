# Contributing

This is a matching decompilation intended to remain workable for years. A
retail byte match is required, but it is only one part of accepting a change.

## Daily loop

1. Keep experiments and non-matching reconstructions under `candidates/`.
2. Promote C into `src/` only together with its manifest entry.
3. Use subsystem headers for cross-unit declarations; do not add file-local
   `extern` debt.
4. Run the narrow object/function diff while iterating.
5. Run `make verify` before handing work off.

`make verify` runs the source/manifest contract, debt ratchets, organization
ratchet, tool tests, full main build, and retail SHA-1 check. It needs the local
retail-derived files and compiler setup. `make ci` is its source-only subset and
runs in GitHub Actions.

## Definition of done

A normal main-executable change is done when:

- the intended function is semantic C under `docs/SOURCE_QUALITY.md`;
- its stock-compiler object diff is exact;
- `make verify` passes;
- any debt reduction is captured with `make debt-baseline` or
  `make organization-baseline` and the lowered JSON is included;
- naming, typing, or source-boundary claims have evidence, not only a byte
  match.

For a shared header, compiler wrapper, Makefile, manifest, source move, or
translation-unit boundary change, use `make verify-clean`. This proves the
result without stale objects. Header dependencies protect later incremental
builds, but they do not replace the clean acceptance check for structural work.

Do not add splat's complete auto-undefined files back to the linker command.
The generated selective alias file is what lets real object definitions take
ownership while retaining absolute pins only for unresolved layout debt.

For an overlay change, also run:

```sh
make overlay-check OVERLAY=<name>
```

The main executable checksum does not verify overlay bytes.

## Ratchet discipline

Debt and opaque-name baselines must equal the current tree. CI fails when debt
increases and also when an improvement leaves a stale higher baseline. This
makes every cleanup permanent: later work cannot silently spend it. Baseline
updates that raise a count are policy changes and require explicit review; they
are not a normal way to make CI green.

The detailed organization and source-quality rules live in
`docs/CODE_ORGANIZATION.md` and `docs/SOURCE_QUALITY.md`.
