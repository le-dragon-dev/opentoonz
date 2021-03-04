#include "tiio_tzl_structure.h"

// ****************************************************************************
// TLVLevelHeader: Constructor
// ****************************************************************************
TLVLevelHeader::TLVLevelHeader(const QString& magicWord, const QString& creator,
                             __int32 hdrSize, __int32 width, __int32 height,
                             __int32 framecount, __int32 frameTableOffset,
                             __int32 iconTableOffset, const QString& codec):
    hdrSize(hdrSize), levelWidth(width), levelHeight(height), framecount(framecount),
    frameTableOffset(frameTableOffset), iconTableOffset(iconTableOffset)
{
  assert(magicWord.length() >= TLV_MAGIC_WORD_SIZE);
  assert(creator.length() <= TLV_CREATOR_SIZE);
  assert(width > 0);
  assert(height > 0);
  assert(framecount > 0 && framecount < 60000);
  assert(frameTableOffset > sizeof(TLVLevelHeader));
  assert(iconTableOffset > sizeof(TLVLevelHeader));
  assert(codec.length() == TLV_CODEC_SIZE);

  memcpy(this->magicWord, magicWord.toStdString().c_str(), TLV_MAGIC_WORD_SIZE);
  memcpy(this->creator, creator.toStdString().c_str(), creator.length());
  memcpy(this->codec, codec.toStdString().c_str(), TLV_CODEC_SIZE);

  // At the moment, only this magic word and codec are accepted
  assert(std::strncmp(this->magicWord, "TLV14", 5ull) == 0);
  assert(std::strncmp(this->codec, "LZ0 ", TLV_CODEC_SIZE) == 0);
}

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

  const bool is_values_valid = levelWidth > 0 && levelHeight > 0        &&
                               framecount > 0 && framecount < 60000     &&
                               frameTableOffset > sizeof(TLVLevelHeader) &&
                               iconTableOffset > sizeof(TLVLevelHeader);

  // Codec should be LZ0 (at the moment)
  const bool is_codec_valid = std::strncmp(codec, "LZ0 ", TLV_CODEC_SIZE) == 0;

  return is_magic_valid && is_values_valid && is_codec_valid;
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
  is >> header.magicWord
     >> header.creator
     >> header.hdrSize
     >> header.levelWidth >> header.levelHeight
     >> header.framecount
     >> header.frameTableOffset >> header.iconTableOffset
     >> header.codec;

  // Asserts
  assert(std::strncmp(header.magicWord, "TLV14", 5ull) == 0);
  assert(header.levelWidth > 0);
  assert(header.levelHeight > 0);
  assert(header.framecount > 0 && header.framecount < 60000);
  assert(header.frameTableOffset > sizeof(TLVLevelHeader));
  assert(header.iconTableOffset > sizeof(TLVLevelHeader));
  assert(std::strncmp(header.codec, "LZ0 ", TLV_CODEC_SIZE) == 0);

  return is;
}

// ****************************************************************************
// TVLOffsetTableRow: Write an offset table row into stream
// ****************************************************************************
std::ostream& operator<<(std::ostream& os, const TVLOffsetTableRow& header) {
  os << header.number << header.letter
     << header.imageHeaderOffset << header.imageDataSize;

  return os;
}

// ****************************************************************************
// TVLOffsetTableRow: Read an offset table row from stream
// ****************************************************************************
std::istream& operator>>(std::istream& is, TVLOffsetTableRow& header) {
  is >> header.number >> header.letter
     >> header.imageHeaderOffset >> header.imageDataSize;

  assert(header.imageHeaderOffset > 0);
  assert(header.imageDataSize > 0);

  return is;
}

// ****************************************************************************
// TVLImageHeader: Write an offset table row into stream
// ****************************************************************************
std::ostream& operator<<(std::ostream& os, const TVLImageHeader& header) {
  return <#initializer #>;
}

// ****************************************************************************
// TVLImageHeader: Read an offset table row from stream
// ****************************************************************************
std::istream& operator>>(std::istream& is, TVLImageHeader& header) {
  return <#initializer #>;
}
