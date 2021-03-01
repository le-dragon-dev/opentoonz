#pragma once

#include <tfilepath.h>
#include <timage_io.h>

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

// Forward declaration
class TLevelReaderTzl;

class TImageReaderTzl final : public TImageReader {
public:
  TImageReaderTzl(const TFilePath &f, const TFrameId &fid, TLevelReaderTzl *);
  ~TImageReaderTzl() {}

private:
  // not implemented
  TImageReaderTzl(const TImageReaderTzl &);
  TImageReaderTzl &operator=(const TImageReaderTzl &src);
  TImageP load10();
  TImageP load11();
  TImageP load13();  // Aggiunta iconcine
  TImageP load14();  //	Aggiunto creator con lunghezza fissa
  const TImageInfo *getImageInfo10() const;
  const TImageInfo *getImageInfo11() const;  // vale anche le versioni > di 11

public:
  //! Indice del frame del livello
  TFrameId m_fid;
  TImageP load() override;
  TImageP loadIcon() override {
    m_isIcon = true;
    return load();
  }
  const TImageInfo *getImageInfo() const override;

  // TImageP doLoad();

  TDimension getSize() const;
  TRect getBBox() const;

private:
  //! Size of image
  int m_lx, m_ly;
  bool m_isIcon;
  //! Reference to level reader
  TLevelReaderTzl *m_lrp;
};