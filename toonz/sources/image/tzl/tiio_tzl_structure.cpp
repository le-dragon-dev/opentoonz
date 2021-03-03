#include "tiio_tzl_structure.h"

// ****************************************************************************
// TLVFileHeader: Constructor
// ****************************************************************************
TLVFileHeader::TLVFileHeader(const QString& magicWord, const QString& creator,
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
  assert(frameTableOffset > sizeof(TLVFileHeader));
  assert(iconTableOffset > sizeof(TLVFileHeader));
  assert(codec.length() == TLV_CODEC_SIZE);

  memcpy(this->magicWord, magicWord.toStdString().c_str(), TLV_MAGIC_WORD_SIZE);
  memcpy(this->creator, creator.toStdString().c_str(), creator.length());
  memcpy(this->codec, codec.toStdString().c_str(), TLV_CODEC_SIZE);

  // At the moment, only this magic word and codec are accepted
  assert(std::strncmp(this->magicWord, "TLV14", 5ull) == 0);
  assert(std::strncmp(this->codec, "LZ0 ", TLV_CODEC_SIZE) == 0);
}

// ****************************************************************************
// TLVFileHeader: Control the validity of:
// - Magic word & codec
// - Level width and height
// - Frame count
// - Offset tables
// ****************************************************************************
bool TLVFileHeader::isValid() const {
  // Magic word should be TLV14 (at the moment)
  const bool is_magic_valid = std::strncmp(magicWord, "TLV14", 5) == 0;

  const bool is_values_valid = levelWidth > 0 && levelHeight > 0        &&
                               framecount > 0 && framecount < 60000     &&
                               frameTableOffset > sizeof(TLVFileHeader) &&
                               iconTableOffset > sizeof(TLVFileHeader);

  // Codec should be LZ0 (at the moment)
  const bool is_codec_valid = std::strncmp(codec, "LZ0 ", TLV_CODEC_SIZE) == 0;

  return is_magic_valid && is_values_valid && is_codec_valid;
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
     >> header.frameTableOffset >> header.iconTableOffset
     >> header.codec;

  // Asserts
  assert(std::strncmp(header.magicWord, "TLV14", 5ull) == 0);
  assert(header.levelWidth > 0);
  assert(header.levelHeight > 0);
  assert(header.framecount > 0 && header.framecount < 60000);
  assert(header.frameTableOffset > sizeof(TLVFileHeader));
  assert(header.iconTableOffset > sizeof(TLVFileHeader));
  assert(std::strncmp(header.codec, "LZ0 ", TLV_CODEC_SIZE) == 0);

  return is;
}

// ****************************************************************************
// TVLOffsetTableRow: Write an offset table row into stream
// ****************************************************************************
std::ostream& operator<<(std::ostream& os, const TVLOffsetTableRow& header) {
  os << header.number << header.letter
     << header.dataOffset << header.dataSize;

  return os;
}

// ****************************************************************************
// TVLOffsetTableRow: Read an offset table row from stream
// ****************************************************************************
std::istream& operator>>(std::istream& is, TVLOffsetTableRow& header) {
  is >> header.number >> header.letter
     >> header.dataOffset >> header.dataSize;

  assert(header.dataOffset > 0);
  assert(header.dataSize > 0);

  return is;
}
