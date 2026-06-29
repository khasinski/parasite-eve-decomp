.set noat
.set noreorder
.globl func_8007E3B4
func_8007E3B4:
lui     $2,%hi(D_A000DFAC)
addiu   $2,$2,%lo(D_A000DFAC)
jr      $2
nop
nop
lui     $8,%hi(D_A000DF80)
addiu   $8,$8,%lo(D_A000DF80)
jalr    $8
nop
nop

.globl func_8007E3DC
func_8007E3DC:
lui     $1,%hi(D_800A34E0)
sw      $31,%lo(D_800A34E0)($1)
jal     EnterCriticalSection
nop
addiu   $9,$0,0x56
addiu   $10,$0,0xB0
jalr    $10
nop
lw      $2,0x18($2)
nop
lw      $3,0x70($2)
nop
andi    $9,$3,0xFFFF
sll     $9,$9,16
lw      $3,0x74($2)
nop
andi    $10,$3,0xFFFF
addu    $3,$9,$10
addiu   $2,$3,0x28
lui     $10,%hi(func_8007E3B4)
addiu   $10,$10,%lo(func_8007E3B4)
lui     $9,%hi(func_8007E3B4 + 0x14)
addiu   $9,$9,%lo(func_8007E3B4 + 0x14)
3:
lw      $3,0($10)
nop
sw      $3,0($2)
addiu   $10,$10,4
bne     $10,$9,3b
addiu   $2,$2,4
lui     $1,0x1
jal     FlushCache
sw      $2,-0x2004($1)
lui     $31,%hi(D_800A34E0)
lw      $31,%lo(D_800A34E0)($31)
nop
jr      $31
nop

.globl func_8007E470
func_8007E470:
lui     $1,%hi(D_800A34E0)
sw      $31,%lo(D_800A34E0)($1)
jal     EnterCriticalSection
nop
addiu   $9,$0,0x57
addiu   $10,$0,0xB0
jalr    $10
nop
lw      $2,0x16C($2)
nop
lw      $3,0x9C8($2)
lui     $10,%hi(func_8007E3B4 + 0x14)
addiu   $10,$10,%lo(func_8007E3B4 + 0x14)
lui     $9,%hi(func_8007E3DC)
addiu   $9,$9,%lo(func_8007E3DC)
4:
lw      $8,0($10)
nop
sw      $8,0x9C8($2)
addiu   $10,$10,4
bne     $10,$9,4b
addiu   $2,$2,4
jal     FlushCache
nop
lui     $31,%hi(D_800A34E0)
lw      $31,%lo(D_800A34E0)($31)
nop
jr      $31
nop

.globl func_8007E4E0
func_8007E4E0:
ori     $2,$0,0xDF80
lui     $10,%hi(StopCARD2 + 0x10)
addiu   $10,$10,%lo(StopCARD2 + 0x10)
lui     $9,%hi(func_8007E3B4)
addiu   $9,$9,%lo(func_8007E3B4)
5:
lw      $3,0($10)
nop
sw      $3,0($2)
addiu   $10,$10,4
bne     $10,$9,5b
addiu   $2,$2,4
jr      $31
nop

.globl _ExitCard
_ExitCard:
lui     $1,%hi(D_800A34F0)
sw      $31,%lo(D_800A34F0)($1)
jal     EnterCriticalSection
nop
addiu   $9,$0,0x56
addiu   $10,$0,0xB0
jalr    $10
nop
lw      $2,0x18($2)
lui     $10,%hi(D_8007E584)
addiu   $10,$10,%lo(D_8007E584)
lui     $9,%hi(D_8007E590)
addiu   $9,$9,%lo(D_8007E590)
6:
lw      $3,0($10)
nop
sw      $3,0x70($2)
addiu   $10,$10,4
bne     $10,$9,6b
addiu   $2,$2,4
jal     FlushCache
nop
jal     ExitCriticalSection
nop
lui     $31,%hi(D_800A34F0)
lw      $31,%lo(D_800A34F0)($31)
nop
jr      $31
nop

.globl D_8007E584
D_8007E584:
nop
nop
nop

.globl D_8007E590
D_8007E590:
nop
