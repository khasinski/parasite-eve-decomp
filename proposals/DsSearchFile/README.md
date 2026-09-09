# DsSearchFile

The function in [`../dsfile/candidate.c`](../dsfile/candidate.c) reconstructs
the complete 736-byte retail function at
0x80081414 in C using the shared Psy-Q `DslFILE` type. It handles media-cache
refresh, up to eight path components, directory/file-cache failures and a
64-entry file search. The successful return is the cached entry pointer,
while the caller's output receives a copy; this is what the retail code does.

Stock GCC 2.8.1 with unsplit addresses gives **90.46196%**. No pins, barriers
or instruction ASM are used. The candidate is not in the production manifest
and is not credited as matched. Remaining differences include loop-constant
hoisting, independent file-name/index induction variables, structure-copy
register allocation and symbolic-address scheduling.

`verify_behavior.py` compares the candidate and SHA-verified retail MIPS in
Unicorn. All 768 cases agree on return pointers, output records, media state
and external-call traces. Cases cover invalid/root/nested paths, missing
directories, the depth limit, first/last/missing file entries, cache/media
failures and diagnostic output. The CD/cache callees are modeled; this finite
check does not establish hardware behavior or replace exact matching. A
negative control returning the output pointer instead of the cache pointer
is rejected on the first successful file lookup.

From the repository root:

```sh
tools/scripts/cc.sh proposals/dsfile/candidate.c /tmp/ds-search.o
python3 proposals/DsSearchFile/verify_behavior.py /tmp/ds-search.o
```

The interpreter needs Unicorn and pyelftools. The checker also requires
`mipsel-none-elf-nm` and `mipsel-none-elf-ld` on PATH.

The accompanying directory-cache correction comes from retail `DS_newmedia`:
0x800818B4 stores the one-based directory index at entry offset 0, while
0x800818C0 stores the path record's parent ID at offset 4. `DS_searchdir`
compares that second field with the requested parent. Neither field is the
record name length; that byte controls copying and path-record advancement.

## Permuter search

A four-worker search was started on SSH host `darwine`, in
`/home/hasik/sprintf-permuter-20260909/dssearch`, with nice level 10.
Its parent PID at launch was 4054286; inspect the process before treating
it as live. It uses the existing stock GCC 2.8.1/MASPSX environment,
`--better-only --stop-on-zero --stack-diffs`, and logs to the parent
directory's `run-dssearch.log`. The original 88.16304% seed's exported baseline object matches
its locally compiled object at 100%.

The original 88.16304% candidate has permuter score 2365. Output 1946-1
introduced a local -1 sentinel before `DS_searchdir`; this lets the compiler
keep the sentinel across the call and improves objdiff to 90.304344%.
The current source keeps that change as `not_found`, with no pins or barriers.
It passes all 512 behavioral cases. Output 1960-1 was rejected: it reused a
diagnostic-state temporary on paths where it had not been initialized.
Output 1941-1 has a slightly lower permuter score but its extra cache-base
temporary gives a lower objdiff percentage than the selected source.

Manual constant pins, argument pins, word-by-word copies and volatile aggregate
copies did not improve on the selected source. The production ASM remains
unmodified until exact instruction and relocation matching is obtained.

Output 1905-1 also passes behavioral comparison. Its explicit first-character
read and saved signed component pointer improve objdiff to 90.46196%. The
selected implementation is now maintained only in the complete DSFILE TU,
not duplicated here. The checker supports the combined TU's local helper
addresses and now includes the real `DS_cachefile` failure return -1.

Output 1708-1 reports 91.603264% and passes finite behavior cases, but reads
uninitialized `entry` in a duplicated diagnostic branch. Host Clang rejects
that read with `-Werror=uninitialized`. Removing the branch drops the score
to 90.304344%, so the 91.603264% result is not accepted.
