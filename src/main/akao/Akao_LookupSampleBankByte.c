#include "pe1/akao/track.h"
extern AkaoSequencerBank *volatile D_8009D2C8;

extern AkaoU8 D_8009B7BC[];
extern AkaoU8 D_8009B8BC[];

int Akao_LookupSampleBankByte(AkaoTrack *track)
{
    AkaoU8 *cursor = track->pc;
    register unsigned repeat __asm__("$6") = track->call_stack_index;
    unsigned code;
    int argument;
    int displacement;
    unsigned bank_byte;
    unsigned skip;

    for (;;) {
        code = *cursor;
check_code:
        if (code < 0x9A) {
            if (code >= 0x8F) {
                track->tremolo_duration = 0;
                track->tremolo_phase &= ~5;
            }
            return *cursor;
        }
        if (code < 0xA0) {
            return 0xA0;
        }
        skip = D_8009B7BC[code];
        if (skip) goto advance_by_table;
        switch (code) {
        case 0xFC:
            cursor++;
            argument = *cursor;
            skip = D_8009B8BC[argument];
            if (skip) goto advance_by_table;
            if (argument == 7) {
                goto check_bank;
            }
            if (argument < 8) {
                if (argument != 6) {
                    code = argument;
                    goto check_code;
                }
                goto handle_6;
            }
            if (argument < 10) {
                cursor++;
                if (*cursor == track->repeat_counters[repeat] + 1) {
                    cursor++;
                    argument = *cursor++;
                    displacement = *cursor++;
                    repeat = (repeat - 1) & 3;
                    goto apply_displacement;
                } else {
                    cursor += 3;
                }
                break;
            }
            break;
handle_6:
            asm volatile("" :: "r"(cursor));
            cursor++;
            goto read_displacement;
check_bank:
            cursor++;
            bank_byte = *cursor;
            cursor++;
            if (D_8009D2C8->field_56 < bank_byte) goto skip_two;
            goto read_displacement;
read_displacement:
            argument = *cursor++;
            displacement = *cursor++;
apply_displacement:
            argument += displacement << 8;
            displacement = (short)argument;
            cursor += displacement;
            break;
advance_by_table:
            cursor += skip;
            break;
skip_two:
            cursor += 2;
            break;
        case 0xC9:
            cursor++;
            if (*cursor == track->repeat_counters[repeat] + 1) {
                cursor++;
                repeat = (repeat - 1) & 3;
            } else {
                goto load_stack;
            }
            break;
        case 0xCB:
        case 0xCD:
        case 0xD1:
        case 0xDB:
            cursor++;
            track->tremolo_duration = 0;
            track->tremolo_phase &= ~5;
            break;
        case 0xCC:
        case 0xD0:
            track->tremolo_phase &= ~5;
            return 0xA0;
        case 0xCA:
            if (track->flags & 0x200000) {
                goto stop;
            }
load_stack:
            cursor = track->call_stack[repeat];
            break;
        default:
stop:
            track->tremolo_duration = 0;
            track->tremolo_phase &= ~5;
            return 0xA0;
        }
        continue;
    }
}
