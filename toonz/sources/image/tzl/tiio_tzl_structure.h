#pragma once

#include <QString>
#include <vector>
#include "tnztypes.h"

// Size of types
const size_t TLV_MAGIC_WORD_SIZE { 8 };
const size_t TLV_CREATOR_SIZE    { 40 };
const size_t TLV_CODEC_SIZE      { 4 };

// Types
using TLVMagicWord = char[TLV_MAGIC_WORD_SIZE];
using TLVCreator   = char[TLV_CREATOR_SIZE];
using TLVCodec     = char[TLV_CODEC_SIZE];

// Constants
const TLVCodec TLV_CODEC {'L', 'Z', 'O', ' '};

// Forward declaration
class TLevelP;

// ****************************************************************************
// Toonz Raster Level offset table row
// ****************************************************************************
struct TLVOffsetTableRow {
  enum class Type {
    FRAME,
    ICON
  };

  TINT32 number {};
  char   letter {};
  TINT32 imageHeaderOffset{};
  TINT32 imageDataSize{};

  // Constructor
  TLVOffsetTableRow() = default;

  // Flow operators
  friend std::ostream& operator<<(std::ostream& os, const TLVOffsetTableRow& header);
  friend std::istream& operator>>(std::istream& is, TLVOffsetTableRow& header);
};

using TLVOffsetTable = std::vector<TLVOffsetTableRow>;

// ****************************************************************************
// Toonz Raster Level header
// ****************************************************************************
struct TLVLevelHeader {
  // Members
  TLVMagicWord magicWord {};
  TLVCreator creator {};

  TINT32 hdrSize {};
  TINT32 levelWidth {};
  TINT32 levelHeight {};
  TINT32 framecount {};

  TINT32 frameTableOffset {};
  TINT32 iconTableOffset {};

  TLVCodec codec {};

  // Constructors
  TLVLevelHeader() = default;
  TLVLevelHeader(const TLevelP& level);

  // Control the validity
  bool isValid() const;

  // Get offset tables
  TLVOffsetTable getOffsetTable(std::istream& is, TLVOffsetTableRow::Type type) const;

  // Flow operators
  friend std::ostream& operator<<(std::ostream& os, const TLVLevelHeader& header);
  friend std::istream& operator>>(std::istream& is, TLVLevelHeader& header);
};

// ****************************************************************************
// Toonz Raster Image header
// ****************************************************************************
struct TLVImageHeader {
  TINT32 sbx0 {}, sby0 {};
  TINT32 sbWidth {}, sbHeight {};
  TINT32 pixelsDataSize {};
  TINT32 dpix {}, dpiy {};
  TINT32 pixelsDataOffset;

  // Constructor
  TLVImageHeader(TINT32 sbx0, TINT32 sby0, TINT32 sbWidth, TINT32 sbHeight,
                 TINT32 pixelsDataSize, TINT32 dpix, TINT32 dpiy,
                 TINT32 headerOffset):
    sbx0(sbx0), sby0(sbx0), sbWidth(sbWidth), sbHeight(sbHeight),
    pixelsDataSize(pixelsDataSize), dpix(dpix), dpiy(dpiy),
    pixelsDataOffset(headerOffset + static_cast<TINT32>(sizeof(TLVImageHeader) - sizeof(TINT32)))
  {}

  // Flow operators
  friend std::ostream& operator<<(std::ostream& os, const TLVImageHeader& header);
  friend std::istream& operator>>(std::istream& is, TLVImageHeader& header);
};