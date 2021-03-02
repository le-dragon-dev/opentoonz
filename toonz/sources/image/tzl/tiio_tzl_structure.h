#pragma once

#include <QString>
#include "tnztypes.h"

// Constants
const size_t TLV_MAGIC_WORD_SIZE = 8ull;
const size_t TLV_CREATOR_SIZE    = 40ull;
const size_t TLV_CODEC_SIZE      = 4ull;

const char TLV_CODEC[TLV_CODEC_SIZE] {'L', 'Z', '0', ' '};

// Types
using TLVMagicWord = char[TLV_MAGIC_WORD_SIZE];
using TLVCreator = char[TLV_CREATOR_SIZE];

// ****************************************************************************
// Toonz Raster Level file header
// ****************************************************************************
struct TLVFileHeader {
  // Members
  TLVMagicWord magicWord {};
  TLVCreator creator {};

  TINT32 hdrSize {};
  TINT32 levelWidth {};
  TINT32 levelHeight {};
  TINT32 framecount {};

  TINT32 frameTableOffset {};
  TINT32 iconTableOffset {};

  // Constructor
  TLVFileHeader(const QString& magicWord, const QString& creator,
                TINT32 hdrSize, TINT32 width, TINT32 height, TINT32 framecount,
                TINT32 frameTableOffset, TINT32 iconTableOffset):
  hdrSize(hdrSize), levelWidth(width), levelHeight(height), framecount(framecount),
  frameTableOffset(frameTableOffset), iconTableOffset(iconTableOffset)
  {}

  // Control the validity
  bool isValid() const;

  // Flow operators
  friend std::ostream& operator<<(std::ostream& os, const TLVFileHeader& header);
  friend std::istream& operator>>(std::istream& is, TLVFileHeader& header);
};