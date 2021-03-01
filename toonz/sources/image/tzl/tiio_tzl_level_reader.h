#pragma once

#include "tiio_tzl_common.h"

class TLevelReaderTzl final : public TLevelReader {
public:
  TLevelReaderTzl(const TFilePath &path);
  ~TLevelReaderTzl();
  void doReadPalette(bool doReadIt) override;
  /*!
Return info about current tzl
*/
  TLevelP loadInfo() override;

  /*!
Return an image with Reader information
*/
  TImageReaderP getFrameReader(TFrameId fid) override;

  QString getCreator() override;
  friend class TImageReaderTzl;

  /*!
                  Return TLV version
          */
  int getVersion() { return m_version; }
  /*!
                  Get the iconSize in the file. Return TRUE if icon exists,
     return FALSE if it not exists.
          */
  bool getIconSize(TDimension &iconSize);

private:
  FILE *m_chan;
  TLevelP m_level;
  TDimension m_res;
  double m_xDpi, m_yDpi;
  // int m_frameIndex;
  // TzlOffsetMap m_frameOffset;//per le vecchie tzl
  TzlOffsetMap m_frameOffsTable;
  TzlOffsetMap m_iconOffsTable;
  int m_version;
  QString m_creator;
  bool m_readPalette;

public:
  static TLevelReader *create(const TFilePath &f) {
    return new TLevelReaderTzl(f);
  }

private:
  void readPalette();
  // not implemented
  TLevelReaderTzl(const TLevelReaderTzl &);
  TLevelReaderTzl &operator=(const TLevelReaderTzl &);
};
