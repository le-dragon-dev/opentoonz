#include "tiio_tzl_structure.h"

// ****************************************************************************
// TLVFileHeader: Control the validity of:
// - Magic word
// - Level width and height
// - Frame count
// - Offset tables
// ****************************************************************************
bool TLVFileHeader::isValid() const {
  return std::strncmp(magicWord, "TLV14", 5) == 0 && // Magic word TLV14
         levelWidth > 0 && levelHeight > 0        && // Width & height > 0
         framecount > 0 && framecount < 60000     && // Framecount [1;59999]
         frameTableOffset > sizeof(TLVFileHeader) && // Offset > header size
         iconTableOffset > sizeof(TLVFileHeader);    // Offset > header size
}

// ****************************************************************************
// TLVFileHeader: Write the header into stream
// ****************************************************************************
std::ostream& operator<<(std::ostream& os, const TLVFileHeader& header) {
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
// TLVFileHeader: Read a header from stream
// ****************************************************************************
std::istream& operator>>(std::istream& is, TLVFileHeader& header) {
  is >> header.magicWord
     >> header.creator
     >> header.hdrSize
     >> header.levelWidth >> header.levelHeight
     >> header.framecount
     >> header.frameTableOffset >> header.iconTableOffset;

  // Asserts
  assert(std::strncmp(header.magicWord, "TLV14", 5ull) == 0);
  assert(header.levelWidth > 0);
  assert(header.levelHeight > 0);
  assert(header.framecount > 0 && header.framecount < 60000);
  assert(header.frameTableOffset > sizeof(TLVFileHeader));
  assert(header.iconTableOffset > sizeof(TLVFileHeader));

  return is;
}

