#pragma once

#include "tnoncopyable.h"

// ****************************************************************************
// Toonz Raster Level Writer
// ****************************************************************************
class TLevelWriterTzl final : public TLevelWriter, private TNonCopyable<TLevelWriterTzl> {
  // Constructor / Destructor
public:
  TLevelWriterTzl(const TFilePath &path, TPropertyGroup *winfo)
      : TLevelWriter(path, winfo) {}
  ~TLevelWriterTzl() override = default;
  TLevelWriterTzl(TLevelWriterTzl &&move);
  TLevelWriterTzl & operator=(TLevelWriterTzl &&move);

public:
  static TLevelWriter *create(const TFilePath &f, TPropertyGroup *winfo) {
    return new TLevelWriterTzl(f, winfo);
  }
};

// ****************************************************************************
// Toonz Raster Image Writer
// ****************************************************************************
class TImageWriterTzl final : public TImageWriter, private TNonCopyable<TImageWriterTzl> {
  // Constructor / Destructor
public:
  TImageWriterTzl(TLevelWriterTzl *lw, TFrameId fid);
  ~TImageWriterTzl() override = default;
  TImageWriterTzl(TImageWriterTzl &&move);
  TImageWriterTzl & operator=(TImageWriterTzl &&move);
};