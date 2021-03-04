#pragma once

#include <QString>
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
const TLVCodec TLV_CODEC {'L', 'Z', '0', ' '};

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

  // Constructor
  TLVLevelHeader(const QString& magicWord, const QString& creator,
                TINT32 hdrSize, TINT32 width, TINT32 height, TINT32 framecount,
                TINT32 frameTableOffset, TINT32 iconTableOffset,
                const QString& codec = TLV_CODEC);

  // Control the validity
  bool isValid() const;

  // Flow operators
  friend std::ostream& operator<<(std::ostream& os, const TLVLevelHeader& header);
  friend std::istream& operator>>(std::istream& is, TLVLevelHeader& header);
};

// ****************************************************************************
// Toonz Raster Level offset table row
// ****************************************************************************
struct TVLOffsetTableRow {
  TINT32 number {};
  char   letter {};
  TINT32 imageHeaderOffset{};
  TINT32 imageDataSize{};

  // Constructor
  TVLOffsetTableRow(TINT32 number, char letter, TINT32 dataOffset, TINT32 dataSize):
    number(number), letter(letter), imageHeaderOffset(dataOffset), imageDataSize(dataSize) {
    assert(dataOffset > 0);
    assert(dataSize > 0);
  }

  // Flow operators
  friend std::ostream& operator<<(std::ostream& os, const TVLOffsetTableRow& header);
  friend std::istream& operator>>(std::istream& is, TVLOffsetTableRow& header);
};

// ****************************************************************************
// Toonz Raster Image header
// ****************************************************************************
struct TVLImageHeader {
  TINT32 sbx0 {}, sby0 {};
  TINT32 sbWidth {}, sbHeight {};
  TINT32 pixelsDataSize {};
  TINT32 dpix {}, dpiy {};
  TINT32 pixelsDataOffset;

  // Constructor
  TVLImageHeader(TINT32 sbx0, TINT32 sby0, TINT32 sbWidth, TINT32 sbHeight,
                 TINT32 pixelsDataSize, TINT32 dpix, TINT32 dpiy,
                 TINT32 headerOffset):
    sbx0(sbx0), sby0(sbx0), sbWidth(sbWidth), sbHeight(sbHeight),
    pixelsDataSize(pixelsDataSize), dpix(dpix), dpiy(dpiy),
    pixelsDataOffset(headerOffset + static_cast<TINT32>(sizeof(TVLImageHeader) - sizeof(TINT32)))
  {}

  // Flow operators
  friend std::ostream& operator<<(std::ostream& os, const TVLImageHeader& header);
  friend std::istream& operator>>(std::istream& is, TVLImageHeader& header);
};