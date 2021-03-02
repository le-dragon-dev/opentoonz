#pragma once

#include "tnoncopyable.h"

// ****************************************************************************
// Toonz Raster Level Reader
// ****************************************************************************
class TLevelReaderTzl final : public TLevelReader, private TNonCopyable<TLevelReaderTzl> {
  // Constructor / Destructor
public:
  explicit TLevelReaderTzl(const TFilePath &path)
      : TLevelReader(path) {}
  ~TLevelReaderTzl() override = default;
  TLevelReaderTzl(TLevelReaderTzl &&move);
  TLevelReaderTzl & operator=(TLevelReaderTzl &&move);

public:
  static TLevelReader *create(const TFilePath &f) {
    return new TLevelReaderTzl(f);
  }
};

// ****************************************************************************
// Toonz Raster Image Reader
// ****************************************************************************
class TImageReaderTzl final : public TImageReader, private TNonCopyable<TImageReaderTzl> {
  // Constructor / Destructor
public:
  TImageReaderTzl(const TFilePath &path, const TFilePath &f,
                  const TFrameId &fid, TLevelReaderTzl *);
  ~TImageReaderTzl() override = default;
  TImageReaderTzl(TImageReaderTzl &&move);
  TImageReaderTzl & operator=(TImageReaderTzl &&move);
};