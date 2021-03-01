#pragma once

#include <set>

#include "tcodec.h"
#include "tlevel_io.h"
#include "tiio_tzl_common.h"

// Forward declaration
class TRasterCodecLZO;

class TLevelWriterTzl final : public TLevelWriter {
  // bool m_paletteWritten;
  bool m_headerWritten;
  bool m_creatorWritten;
  FILE *m_chan;
  TLevelP m_level;
  TFilePath m_path, m_palettePath;
  long m_frameCountPos;
  int m_frameCount;
  TzlOffsetMap m_frameOffsTable;
  TzlOffsetMap m_iconOffsTable;
  std::set<TzlChunk> m_freeChunks;
  bool m_exists;
  TPalette *m_palette;
  TDimension m_res;
  TINT32 m_offsetTablePos;
  TINT32 m_iconOffsetTablePos;
  std::map<TFrameId, TFrameId> m_renumberTable;
  const char *m_magic;
  int m_version;
  bool m_updatedIconsSize;
  TDimension m_userIconSize;  // IconSize settata dall'utente
  TDimension
      m_iconSize;  // IconSize in the file according to image aspect ratio.
  TDimension m_currentIconSize;  // If file exists this is the current IconSize
  // in the file
  TRasterCodecLZO *m_codec;

  bool m_overwritePaletteFlag;

public:
  TLevelWriterTzl(const TFilePath &path, TPropertyGroup *winfo);
  ~TLevelWriterTzl();

  void setPalette(TPalette *palette) override;

  void setOverwritePaletteFlag(bool overwrite) override {
    m_overwritePaletteFlag = overwrite;
  }

  void renumberFids(const std::map<TFrameId, TFrameId> &renumberTable) override;
  /*!
                  Setta le dimensioni dell'iconcina nel file.
                  ATTENZIONE: va necessariamente richiamata prima di
     renumberFids()!
          */
  void setIconSize(TDimension iconSize) override;
  TDimension getIconSize() const override { return m_iconSize; }

  TImageWriterP getFrameWriter(TFrameId fid) override;

  // friend class TImageWriterTzl;
  // Save Image and icon
  void save(const TImageP &img);
  void save(const TImageP &img, const TFrameId &fid);
  // save only icon
  void saveIcon(const TImageP &img, const TFrameId &fid);
  // check icon size. Return True if newSize is not equal to currentIconSize
  // (the iconSize in File)
  bool checkIconSize(const TDimension &newSize);
  // Update all icons with new size. Return true if success.
  bool resizeIcons(const TDimension &newSize);
  void remove(const TFrameId &fid);

  /*!
           Da informazioni relativamente allo spazio libero presente su file.
           Ritorna un numero compreso tra 0 e 1: 0 nessuno spazio libero, 1
     tutto lo spazio è libero.
   */
  float getFreeSpace();
  /*!
     Save the file without freeSpace.
     Salva tutti i frame in maniera continua, senza buchi.
     Return TRUE if successfully.
   */
  bool optimize();

public:
  static TLevelWriter *create(const TFilePath &f, TPropertyGroup *winfo) {
    return new TLevelWriterTzl(f, winfo);
  }

private:
  bool m_adjustRatio;
  void doSave(const TImageP &img, const TFrameId &fid);
  // Save image on disk. If isIcon is true save image as icon.
  void saveImage(const TImageP &img, const TFrameId &fid, bool isIcon = false);
  void createIcon(const TImageP &imgIn, TImageP &imgOut);
  bool convertToLatestVersion();
  void writeHeader(const TDimension &size);
  void buildFreeChunksTable();
  void addFreeChunk(TINT32 offs, TINT32 length);
  TINT32 findSavingChunk(const TFrameId &fid, TINT32 length,
                         bool isIcon = false);
  // not implemented
  TLevelWriterTzl(const TLevelWriterTzl &);
  TLevelWriterTzl &operator=(const TLevelWriterTzl &);
};