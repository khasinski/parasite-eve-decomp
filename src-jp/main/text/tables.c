#include "common.h"
#include "pe1/aya.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/menu_widget.h"
#include "pe1/resident_tables.h"
#include "pe1/text.h"

/* JP: selects which run of four table offsets (after the reserved word) the
 * text lookups use. */

MenuWidgetSimpleDescriptor *MenuWidget_LookupSimpleDescriptor(unsigned int index) {
    if (index >= 0x41) {
        return 0;
    }
    return &g_MenuWidgetSimpleDescriptors[index];
}

MenuWidgetGridDescriptor *MenuWidget_LookupGridDescriptor(unsigned int index) {
    if (index >= 0x41) {
        return 0;
    }
    return &g_MenuWidgetGridDescriptors[index];
}

DrawGlyphDescriptor *Draw_LookupGlyphDescriptor(int index) {
    s32 *slot = &g_ResidentDataPack.directory.glyphDescriptors;
    s32 offset = *slot;
    DrawGlyphDescriptor *table = (DrawGlyphDescriptor *)&g_ResidentDataPack.bytes[offset];

    return &table[index];
}

void *Text_LookupTable0(unsigned int index) {
    TextOffsetTable *table;
    s16 offset;

    table = RESIDENT_TABLE(TextOffsetTable, text);
    if (index < (s16)table->header.count) {
        offset = table->header.offsets[index];
        return &table->bytes[offset];
    }

    return 0;
}

ItemDataRecord *Item_LookupBaseData(unsigned int index) {
    s32 *endSlot = &g_ResidentDataPack.directory.statGrowth;
    s32 end = *endSlot;
    s32 begin = g_ResidentDataPack.directory.itemBaseData;

    if (index >= (unsigned int)(end - begin) / sizeof(ItemDataRecord)) {
        return 0;
    }
    return (ItemDataRecord *)&g_ResidentDataPack.bytes[begin] + index;
}

void *Aya_GetStatGrowthTable(int category) {
    return &RESIDENT_TABLE(u8, statGrowth)[category << 9];
}

AyaLevelStats *Aya_LookupLevelStats(int level) {
    if (level < 0) {
        level = 0;
    } else if (level >= 0x63) {
        level = 0x62;
    }

    return &RESIDENT_TABLE(AyaLevelStats, levelStats)[level];
}

void *Aya_GetLevelExpTable(void) {
    return RESIDENT_TABLE(s32, levelExp);
}

void *Aya_GetParasiteSpellUnlockTable(void) {
    return RESIDENT_TABLE(ParasiteSpellEntry, parasiteSpells);
}

int Draw_LookupGlyphMetrics(int glyph) {
    return g_ResidentDataPack.bytes[glyph + g_ResidentDataPack.directory.glyphMetrics];
}

void *Text_LookupTable4(unsigned int index) {
    int slot = g_TextTableSet * 4 + 1;
    ResidentDataPack *pack = &g_ResidentDataPack;
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)&pack->bytes[pack->directory.text];
    table = (TextOffsetTable *)&directory->bytes[((s32 *)directory)[slot]];
    if (index >= (s16)table->header.count) {
        return 0;
    }

    offset = table->header.offsets[index];
    return &table->bytes[offset];
}

void *Text_LookupTable8(unsigned int index) {
    int slot = g_TextTableSet * 4 + 2;
    ResidentDataPack *pack = &g_ResidentDataPack;
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)&pack->bytes[pack->directory.text];
    table = (TextOffsetTable *)&directory->bytes[((s32 *)directory)[slot]];
    if (index >= (s16)table->header.count) {
        return 0;
    }

    offset = table->header.offsets[index];
    return &table->bytes[offset];
}

void *Text_LookupTableC(unsigned int index) {
    int slot = g_TextTableSet * 4 + 3;
    ResidentDataPack *pack = &g_ResidentDataPack;
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)&pack->bytes[pack->directory.text];
    table = (TextOffsetTable *)&directory->bytes[((s32 *)directory)[slot]];
    if (index >= (s16)table->header.count) {
        return 0;
    }

    offset = table->header.offsets[index];
    return &table->bytes[offset];
}

void *Text_LookupTable10(unsigned int index) {
    int slot = g_TextTableSet * 4 + 4;
    ResidentDataPack *pack = &g_ResidentDataPack;
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)&pack->bytes[pack->directory.text];
    table = (TextOffsetTable *)&directory->bytes[((s32 *)directory)[slot]];
    if (index >= (s16)table->header.count) {
        return 0;
    }

    offset = table->header.offsets[index];
    return &table->bytes[offset];
}

/* Text-table-3 entry naming the location of map `map`. */
void *Text_LookupMapLocationText(int map) {
    TextTableDirectory *directory;
    TextOffsetTable *table;
    int index;
    s16 offset;
    int slot;

    index = g_ResidentDataPack.bytes[map + g_ResidentDataPack.directory.mapLocationIds];
    if (index == 0) {
        return 0;
    }

    slot = g_TextTableSet * 4 + 4;
    directory = RESIDENT_TABLE(TextTableDirectory, text);
    table = (TextOffsetTable *)&directory->bytes[((s32 *)directory)[slot]];
    index += 0x7F;
    if ((unsigned int)index >= (s16)table->header.count) {
        return 0;
    }

    offset = table->header.offsets[index];
    return &table->bytes[offset];
}

/* Location name of map `map` (-1 selects name 15); names are NUL-separated. */
u8 *Text_GetMapLocationName(int map) {
    u8 *name;

    name = RESIDENT_TABLE(u8, locationNames);
    if (map >= 0) {
        map = g_ResidentDataPack.bytes[map + g_ResidentDataPack.directory.mapLocationIds];
    } else {
        map = 0xF;
    }

    if (map == 0) {
        return 0;
    }

    do {
        if (*name++ == 0) {
            map--;
        }
    } while (map > 0);

    return name;
}

/* Memory-card icon block copied into each save file header. */
u8 *Text_GetSaveIconData(void) {
    return RESIDENT_TABLE(u8, saveIcon);
}
