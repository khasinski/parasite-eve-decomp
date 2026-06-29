#ifndef PE1_PE_IMAGE_H
#define PE1_PE_IMAGE_H

/* PE.IMG -- the game's main on-disc asset archive (~206 MB).
 *
 * Everything that is not the executable or streamed FMV/XA audio lives in
 * PE.IMG: room geometry, entity models, TIM textures, the image font glyphs,
 * the string/message tables, scene init data, etc. For a PC port this is the
 * single container we must understand byte-for-byte.
 *
 * == HOW IT IS LOCATED (boot) ==
 *   OpenPeImage() (src/main/cdrom/OpenPeImage.c) searches the ISO9660 volume
 *   with DsSearchFile() for these paths (verified from the data bytes at
 *   0x80011330 / 0x80011348 / 0x80011354):
 *     "\FMV1\PEDISC01.IDF;1"   -- CD1 marker
 *     "\PE.IMG;1"              -- the archive itself
 *     "\FMV2\PEDISC02.IDF;1"   -- CD2 marker
 *   The matched directory record's start sector is turned into an integer LBA
 *   by CdPosToInt() and stored in g_PeImageBaseLba. The FMV marker that was
 *   found sets g_PeImageMountFlags (bit0 = CD1, bit1 = CD2) -- this is the
 *   CD1/CD2 disc-detection used elsewhere. So the base LBA is NOT a hardcoded
 *   constant; it is discovered at runtime (retail pressing happens to place
 *   PE.IMG at LBA 1013, but the code never assumes that).
 *
 * == HOW DATA IS ADDRESSED (hybrid: baked offset tables + internal directory) ==
 *
 *   EVERY read is:  lba = g_PeImageBaseLba + <sector offset> ; then
 *   CdRom_ReadSectorsFromLba(lba, dst, sectorCount). The sector offset is
 *   RELATIVE to the start of PE.IMG, so in an extracted PE.IMG file the byte
 *   offset is simply  (sectorOffset * PE_IMAGE_SECTOR_SIZE).  No read ever
 *   uses an absolute disc LBA, and no read chains off a sector number that was
 *   itself read from an earlier sector -- the offsets come from one of:
 *
 *   TIER 1 -- fixed sector-offset tables baked into the EXE (u16 entries,
 *   stride 2, sector units relative to base). A loader picks an index, then
 *     start = baseTable[idx];  end = endTable[idx];  count = end - start;
 *     CdRom_ReadSectorsFromLba(g_PeImageBaseLba + start, dst, count);
 *   Known tables (data lives in Data_0818A0.inc.s region; dump for exact rows):
 *     D_800930D8 / D_800930DA  entity-texture start / end sectors
 *                              (Scene_LoadEntityTexture(s), Render_InitDisplayLists)
 *     D_80093378 / D_8009337C  room data start / offset (Scene_LoadRoom)
 *     D_8009315E / D_80093160  scene init-data ranges (Task_LoadSceneData)
 *     D_80093162               scene asset ranges (Scene_LoadRoomAssets)
 *     D_80093168 / D_8009316A / D_8009316E  overlay/main asset + font/UI table
 *                              ranges (Overlay_LoadTables)
 *     D_80093176               font glyph data (Render_LoadFontGlyph)
 *   These define WHERE each *category* of asset sits inside PE.IMG.
 *
 *   TIER 2 -- a self-describing directory that lives inside PE.IMG. Once a
 *   Tier-1 range is read into RAM (the "base"), individual assets are found by
 *   key with the Asset_FindTable* family (src/main/asset/). The format
 *   (verified from those functions) is:
 *     base + *(u32*)(base + 4)            -> sub-header
 *     descriptor = *(u32*)(subHeader + K) -- K = 0x08, 0x2C or 0x30 selects a table
 *     recordTable = base + (descriptor & 0x3FFFFF)   // low 22 bits = byte offset
 *     recordCount = descriptor >> 22                 // high 10 bits = count
 *   Each record is 0xC bytes:
 *     +0x04  u32  payload offset, masked 0xFFFFFF, relative to base (the asset)
 *     +0x08  u32  key   (Asset_FindTable08ByU32Key)
 *     +0x0A  u16  key   (Asset_FindTable2CByU16Key @0x2C, Asset_FindTable30ByU16Key @0x30)
 *   A lookup linearly scans recordCount records; on key match it returns
 *   base + (record[+4] & 0xFFFFFF). So PE.IMG has NO ISO9660 filesystem of its
 *   own -- it is a raw sector archive whose category chunks each begin with one
 *   of these compact descriptor/record directories.
 *
 * == READ PRIMITIVES ==
 *   CdRom_ReadSectorsFromLba / CdRom_ReadSectors  -- issue a sector read
 *   CdRom_ReadSync / CdRom_PollPendingDsRead      -- wait / poll completion
 *   The async "DsRead" subsystem (CdRom_*DsRead*) lets a load run across frames.
 *
 * == PORTING NOTES ==
 *   - Replace OpenPeImage with: open the extracted PE.IMG; set base = 0.
 *   - sectorOffset * 2048 == byte offset into the extracted file.
 *   - Keep the Tier-1 tables (dump them from the EXE) and the Asset_FindTable*
 *     descriptor logic verbatim; only the read primitive changes (fseek+fread).
 *   - To enumerate "every byte": walk the Tier-1 tables to get every category
 *     range, then parse each range's Tier-2 directory to enumerate its records.
 */

#define PE_IMAGE_SECTOR_SIZE   2048   /* CD-ROM data sector; sectorOffset*this = file byte offset */

/* descriptor field packing (Tier-2 directory sub-header words) */
#define PE_DIR_OFFSET_MASK     0x3FFFFF   /* low 22 bits: byte offset of record table from base */
#define PE_DIR_COUNT_SHIFT     22         /* high 10 bits: record count */
#define PE_DIR_RECORD_STRIDE   0xC        /* bytes per directory record */
#define PE_DIR_PAYLOAD_MASK    0xFFFFFF   /* record +0x04 payload offset mask */

extern unsigned int   g_PeImageBaseLba;    /* =0x800B0DD8: PE.IMG start sector, found at boot */
extern signed char    g_PeImageMountFlags; /* =0x800B0DCD: bit0 CD1 (FMV1), bit1 CD2 (FMV2) */

#endif /* PE1_PE_IMAGE_H */
