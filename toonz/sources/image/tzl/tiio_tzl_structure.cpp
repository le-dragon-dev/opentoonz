#include <ostream>
#include <istream>

#include "tiio_tzl_structure.h"

// ****************************************************************************
// TLVLevelHeader: Constructor with TLevel
// ****************************************************************************
TLVLevelHeader::TLVLevelHeader(const TLevelP& level) {}

// ****************************************************************************
// TLVLevelHeader: Control the validity of:
// - Magic word & codec
// - Level width and height
// - Frame count
// - Offset tables
// ****************************************************************************
bool TLVLevelHeader::isValid() const {
  // Magic word should be TLV14 (at the moment)
  const bool is_magic_valid = std::strncmp(magicWord, "TLV14", 5) == 0;

  const bool is_values_valid = levelWidth > 0 && levelHeight > 0         &&
                               framecount > 0 && framecount < 60000      &&
                               frameTableOffset > sizeof(TLVLevelHeader) &&
                               iconTableOffset > sizeof(TLVLevelHeader);

  // Codec should be LZ0 (at the moment)
  const bool is_codec_valid = std::strncmp(codec, "LZO ", TLV_CODEC_SIZE) == 0;

  return is_magic_valid && is_values_valid && is_codec_valid;
}

// ****************************************************************************
// ****************************************************************************
TLVOffsetTable TLVLevelHeader::getOffsetTable(std::istream& is, TLVOffsetTableRow::Type type) const {
  TLVOffsetTable table;

  // Go to the right place in the file
  is.seekg(type == TLVOffsetTableRow::Type::FRAME ? frameTableOffset : iconTableOffset);

  // Get table rows
  for (TINT32 i = 0; i < framecount; ++i) {
    TLVOffsetTableRow row {0, 0, 0, 0};
    is >> row;
    table.push_back(row);
  }

  return std::move(table);
}

// ****************************************************************************
// TLVLevelHeader: Write the header into stream
// ****************************************************************************
std::ostream& operator<<(std::ostream& os, const TLVLevelHeader& header) {
  os << header.magicWord
     << header.creator
     << header.hdrSize
     << header.levelWidth << header.levelHeight
     << header.framecount
     << header.frameTableOffset << header.iconTableOffset
     << TLV_CODEC;

  return os;
}

// ****************************************************************************
// TLVLevelHeader: Read a header from stream
// ****************************************************************************
std::istream& operator>>(std::istream& is, TLVLevelHeader& header) {
  is.read(header.magicWord, TLV_MAGIC_WORD_SIZE);
  is.read(header.creator, TLV_CREATOR_SIZE);
  is.read(reinterpret_cast<char*>(&header.hdrSize), sizeof(TINT32));
  is.read(reinterpret_cast<char*>(&header.levelWidth), sizeof(TINT32));
  is.read(reinterpret_cast<char*>(&header.levelHeight), sizeof(TINT32));
  is.read(reinterpret_cast<char*>(&header.framecount), sizeof(TINT32));
  is.read(reinterpret_cast<char*>(&header.frameTableOffset), sizeof(TINT32));
  is.read(reinterpret_cast<char*>(&header.iconTableOffset), sizeof(TINT32));
  is.read(header.codec, TLV_CODEC_SIZE);

  // Asserts
  assert(std::strncmp(header.magicWord, "TLV14", 5ull) == 0);
  assert(header.levelWidth > 0);
  assert(header.levelHeight > 0);
  assert(header.framecount > 0 && header.framecount < 60000);
  assert(header.frameTableOffset > sizeof(TLVLevelHeader));
  assert(header.iconTableOffset > sizeof(TLVLevelHeader));
  assert(std::strncmp(header.codec, "LZO ", TLV_CODEC_SIZE) == 0);

  return is;
}

// ****************************************************************************
// TLVOffsetTableRow: Write an offset table row into stream
// ****************************************************************************
std::ostream& operator<<(std::ostream& os, const TLVOffsetTableRow& tableRow) {
  os << tableRow.number << tableRow.letter
     << tableRow.imageHeaderOffset << tableRow.imageDataSize;

  return os;
}

// ****************************************************************************
// TLVOffsetTableRow: Read an offset table row from stream
// ****************************************************************************
std::istream& operator>>(std::istream& is, TLVOffsetTableRow& tableRow) {
  is.read(reinterpret_cast<char*>(&tableRow.number), sizeof(TINT32));
  is.get(tableRow.letter);
  is.read(reinterpret_cast<char*>(&tableRow.imageHeaderOffset), sizeof(TINT32));
  is.read(reinterpret_cast<char*>(&tableRow.imageDataSize), sizeof(TINT32));

  assert(tableRow.imageHeaderOffset > 0);
  assert(tableRow.imageDataSize > 0);

  return is;
}

// ****************************************************************************
// TLVImageHeader: Write an offset table row into stream
// ****************************************************************************
std::ostream& operator<<(std::ostream& os, const TLVImageHeader& header) {
  os << header.sbx0 << header.sby0
     << header.sbWidth << header.sbHeight
     << header.pixelsDataSize
     << header.dpix << header.dpiy;

  return os;
}

// ****************************************************************************
// TLVImageHeader: Read an offset table row from stream
// ****************************************************************************
std::istream& operator>>(std::istream& is, TLVImageHeader& header) {
  is >> header.sbx0 >> header.sby0
     >> header.sbWidth >> header.sbHeight
     >> header.pixelsDataSize
     >> header.dpix >> header.dpiy;

  header.pixelsDataOffset = static_cast<TINT32>(is.tellg());

  return is;
}
