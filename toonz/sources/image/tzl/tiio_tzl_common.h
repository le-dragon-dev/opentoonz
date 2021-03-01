#pragma once

#include "tlevel_io.h"

const int CURRENT_VERSION = 14;
const int CREATOR_LENGTH  = 40;

namespace {

class Header {
public:
  enum RasType {
    Raster32RGBM,
    Raster64RGBM,
    Raster32CM,
    RasterGR8,
    RasterUnknown
  };

  int m_lx;
  int m_ly;
  RasType m_rasType;
};



class TzlChunk {
public:
  TINT32 m_offs;
  TINT32 m_length;

  TzlChunk(TINT32 offs, TINT32 length) : m_offs(offs), m_length(length) {}
  TzlChunk() : m_offs(0), m_length(0) {}
  bool operator<(const TzlChunk &c) const { return m_offs < c.m_offs; }

private:
};

typedef std::map<TFrameId, TzlChunk> TzlOffsetMap;

bool readVersion(FILE *chan, int &version) {
  char magic[8];
  memset(magic, 0, sizeof(magic));
  fread(&magic, sizeof(char), 8, chan);
  if (memcmp(magic, "TLV10", 5) == 0) {
    version = 10;
  } else if (memcmp(magic, "TLV11", 5) == 0) {
    version = 11;
  } else if (memcmp(magic, "TLV12", 5) == 0) {
    version = 12;
  } else if (memcmp(magic, "TLV13", 5) == 0) {
    version = 13;
  } else if (memcmp(magic, "TLV14", 5) == 0) {
    version = 14;
  } else {
    return false;
  }
  return true;
}

bool readHeaderAndOffsets(FILE *chan, TzlOffsetMap &frameOffsTable,
                          TzlOffsetMap &iconOffsTable, TDimension &res,
                          int &version, QString &creator, TINT32 *_frameCount,
                          TINT32 *_offsetTablePos, TINT32 *_iconOffsetTablePos,
                          TLevelP level) {
  TINT32 hdrSize;
  TINT32 lx = 0, ly = 0, frameCount = 0;
  char codec[4];
  TINT32 offsetTablePos;
  TINT32 iconOffsetTablePos;
  // char magic[8];

  assert(frameOffsTable.empty());
  assert(iconOffsTable.empty());

  if (!readVersion(chan, version)) return false;

  // read creator
  if (version == 14) {
    char buffer[CREATOR_LENGTH + 1];
    memset(buffer, 0, sizeof buffer);
    fread(&buffer, sizeof(char), CREATOR_LENGTH, chan);
    creator = buffer;
  }

  fread(&hdrSize, sizeof(TINT32), 1, chan);
  fread(&lx, sizeof(TINT32), 1, chan);
  fread(&ly, sizeof(TINT32), 1, chan);
  fread(&frameCount, sizeof(TINT32), 1, chan);

  if (version > 10) {
    fread(&offsetTablePos, sizeof(TINT32), 1, chan);
    fread(&iconOffsetTablePos, sizeof(TINT32), 1, chan);
#if !TNZ_LITTLE_ENDIAN
    offsetTablePos     = swapTINT32(offsetTablePos);
    iconOffsetTablePos = swapTINT32(iconOffsetTablePos);
#endif
  }

  fread(&codec, 4, 1, chan);

#if !TNZ_LITTLE_ENDIAN
  hdrSize    = swapTINT32(hdrSize);
  lx         = swapTINT32(lx);
  ly         = swapTINT32(ly);
  frameCount = swapTINT32(frameCount);
#endif
  assert(0 < frameCount && frameCount < 60000);

  if (version > 10 && offsetTablePos != 0 && iconOffsetTablePos != 0) {
    // assert(offsetTablePos>0);
    assert(frameCount > 0);

    fseek(chan, offsetTablePos, SEEK_SET);
    TFrameId oldFid(TFrameId::EMPTY_FRAME);
    for (int i = 0; i < (int)frameCount; i++) {
      TINT32 number, offs, length;
      char letter;
      fread(&number, sizeof(TINT32), 1, chan);
      fread(&letter, sizeof(char), 1, chan);
      fread(&offs, sizeof(TINT32), 1, chan);
      if (version >= 12) fread(&length, sizeof(TINT32), 1, chan);

#if !TNZ_LITTLE_ENDIAN
      number = swapTINT32(number);
      offs   = swapTINT32(offs);
      if (version == 12) length = swapTINT32(length);
#endif
      //		std::cout << "#" << i << std::hex << " n 0x" << number
      //<< " l 0x" << letter << " o 0x" << offs << std::dec << std::endl;

      TFrameId fid(number, letter);
      // assert(i==0 || oldFid<fid);

      if (version >= 12) {
        frameOffsTable[fid] = TzlChunk(offs, length);
      } else {
        frameOffsTable[fid] = TzlChunk(offs, 0);
        if (i > 0) {
          frameOffsTable[oldFid].m_length =
              offs - frameOffsTable[oldFid].m_offs;
          assert(frameOffsTable[oldFid].m_length > 0);
        }
        if (i == frameCount - 1) {
          frameOffsTable[fid].m_length = offsetTablePos - offs;
          assert(frameOffsTable[fid].m_length > 0);
        }
      }
      oldFid = fid;
      if (level) level->setFrame(fid, TImageP());
    }
    if (version >= 13) {
      // Build IconOffsetTable
      fseek(chan, iconOffsetTablePos, SEEK_SET);

      for (int i = 0; i < (int)frameCount; i++) {
        TINT32 number, thumbnailOffs, thumbnailLength;
        char letter;
        fread(&number, sizeof(TINT32), 1, chan);
        fread(&letter, sizeof(char), 1, chan);
        fread(&thumbnailOffs, sizeof(TINT32), 1, chan);
        fread(&thumbnailLength, sizeof(TINT32), 1, chan);

#if !TNZ_LITTLE_ENDIAN
        number          = swapTINT32(number);
        thumbnailOffs   = swapTINT32(thumbnailOffs);
        thumbnailLength = swapTINT32(thumbnailLength);
#endif
        TFrameId fid(number, letter);
        iconOffsTable[fid] = TzlChunk(thumbnailOffs, thumbnailLength);
      }
    }
  } else {
    // m_frameOffsTable.resize(frameCount);
    frameOffsTable[TFrameId(1)] = TzlChunk(ftell(chan), 0);
    iconOffsTable[TFrameId(1)]  = TzlChunk(ftell(chan), 0);
    int i;
    for (i = 2; i <= (int)frameCount; i++) {
      frameOffsTable[TFrameId(i)] = TzlChunk(0, 0);
      iconOffsTable[TFrameId(i)]  = TzlChunk(0, 0);
    }
    if (level)
      for (i = 1; i <= (int)frameCount; i++)
        level->setFrame(TFrameId(i), TImageP());
  }

  res.lx = lx;
  res.ly = ly;
  if (_offsetTablePos) *_offsetTablePos = offsetTablePos;
  if (_iconOffsetTablePos) *_iconOffsetTablePos = iconOffsetTablePos;

  if (_frameCount) *_frameCount = frameCount;

  return true;
}
}  // namespace