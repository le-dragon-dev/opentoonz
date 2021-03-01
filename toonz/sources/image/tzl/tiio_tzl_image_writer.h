#pragma once

#include "timage_io.h"

// Constants
const int CURRENT_VERSION = 14;

// Forward declaration
class TLevelWriterTzl;

class TImageWriterTzl final : public TImageWriter {
  //! Reference to level writer
  TLevelWriterTzl *m_lwp;
  TFrameId m_fid;
  TDimension
      m_iconSize;  // Dimensioni dell'iconcina salvata all'interno del file tlv
  //	In genere questo parametro viene settato come quello impostato
  // dall'utente
  // nelle preferenze.
public:
  TImageWriterTzl(TLevelWriterTzl *lw, TFrameId fid)
      : TImageWriter(TFilePath())
      , m_lwp(lw)
      , m_fid(fid)
      , m_iconSize(TDimension(80, 60)) {}
  ~TImageWriterTzl() {}

private:
  // not implemented
  TImageWriterTzl(const TImageWriterTzl &);
  TImageWriterTzl &operator=(const TImageWriterTzl &src);

public:
  void save(const TImageP &img) override; // { m_lwp->save(img, m_fid); }
};