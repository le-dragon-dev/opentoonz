#pragma once

#include "tnoncopyable.h"
#include "tiio_tzl_structure.h"

// ****************************************************************************
// Toonz Raster Level Reader
// ****************************************************************************
class TLevelReaderTzl final : public TLevelReader, private TNonCopyable<TLevelReaderTzl> {
  // Constructor / Destructor
public:
  explicit TLevelReaderTzl(const TFilePath &path);
  ~TLevelReaderTzl() override = default;

  // Virtuals to override
public:
  TLevelP loadInfo() override;
  QString getCreator() override;

  void doReadPalette(bool) override;
  TImageReaderP getFrameReader(TFrameId id) override;

public:
  static TLevelReader *create(const TFilePath &f) {
    return new TLevelReaderTzl(f);
  }

private:
  TLVLevelHeader m_levelHeader;
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
};