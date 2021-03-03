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

  TLVCodec codec {};

  // Constructor
  TLVFileHeader(const QString& magicWord, const QString& creator,
                TINT32 hdrSize, TINT32 width, TINT32 height, TINT32 framecount,
                TINT32 frameTableOffset, TINT32 iconTableOffset,
                const QString& codec = TLV_CODEC);

  // Control the validity
  bool isValid() const;

  // Flow operators
  friend std::ostream& operator<<(std::ostream& os, const TLVFileHeader& header);
  friend std::istream& operator>>(std::istream& is, TLVFileHeader& header);
};