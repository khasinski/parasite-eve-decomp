# Contiguous LIBCD command and wait functions

`candidate.c` compiles CD_sync, CD_ready and CD_cw together, in their retail
order, using one stock GCC 2.8.1 configuration:
`-mno-split-addresses -fno-expensive-optimizations`. It includes their current
candidate sources, so standalone and combined experiments share definitions.
This is a reconstructed portion of BIOS_1, not a complete translation unit;
getintr, other driver routines and data ownership remain outside this file.
Production remains assembly for these three functions.

| Function | Retail bytes | Combined objdiff match |
| --- | ---: | ---: |
| CD_sync | 640 | 91.79375% |
| CD_ready | 712 | 89.15169% |
| CD_cw | 1036 | 92.22394% |

The common configuration improves CD_cw from 91.64865% and retains the other
two matches. Without `-fno-expensive-optimizations`, the combined scores are
88.58125%, 86.146065% and 91.64865%. Neither configuration uses pins, barriers
or instruction ASM. The shared timeout, dispatcher and copy helpers remain
inline C.

```sh
tools/scripts/cc.sh proposals/libcd_commands/candidate.c /tmp/libcd_commands.o
python proposals/CD_sync/verify_behavior.py /tmp/libcd_commands.o
python proposals/CD_ready/verify_behavior.py /tmp/libcd_commands.o
python proposals/CD_cw/verify_behavior.py /tmp/libcd_commands.o
python proposals/CD_cw/verify_behavior.py /tmp/libcd_commands.o --real-sync
```

The verifiers link and locate functions using ELF symbols, allowing the same
suites to check a combined object. The first three commands run 840, 1008 and
3072 finite cases, with external calls modeled as documented in each suite.
The final command runs another 3072 cases in which CD_cw really calls CD_sync:
retail runs the original two functions and the candidate runs both C bodies.
It begins with a pending error status, which CD_sync consumes before CD_cw
issues a new command; CD_cw deliberately ignores that synchronization result.
The check asserts that the initial synchronization's two VSync calls and
callback-context check occur before issuing the command. Hardware, getintr,
callbacks and remaining external functions are still modeled. This is not
whole-driver or physical-hardware verification.

A combined negative control that omits CD_cw's CD_sync call is rejected.
All three separate-function suites pass on the combined object, and the
standalone CD_cw suite also passes with its updated common compiler flags.
The SDK provenance verifier in `../CD_cw/verify_sdk.py` establishes the
contiguous retail ranges independently of these imperfect C matches.
