#include "tcodec.h"
#include "tcontenthistory.h"
#include "timageinfo.h"
#include "tpalette.h"
#include "tpersist.h"
#include "tstream.h"

#include "tiio_tzl_level_reader.h"
#include "tiio_tzl_image_reader.h"

void TLevelReaderTzl::readPalette() {
  TFilePath pltfp = m_path.withNoFrame().withType("tpl");
  TPersist *p     = 0;
  TIStream is(pltfp);
  TPalette *palette = 0;

  if (is) {
    std::string tagName;
    if (is.matchTag(tagName) && tagName == "palette") {
      std::string gname;
      is.getTagParam("name", gname);
      palette = new TPalette();
      palette->loadData(is);
      palette->setGlobalName(::to_wstring(gname));
      is.matchEndTag();
    }
  }

  if (!palette) {
    int i;
    palette = new TPalette();
    for (i = 1; i < 128 + 32; i++) palette->addStyle(TPixel32(127, 150, 255));

    for (i = 0; i < 10; i++) palette->getPage(0)->addStyle(i + 1);
    for (i = 0; i < 10; i++) palette->getPage(0)->addStyle(128 + i);
  }
  if (m_level) m_level->setPalette(palette);
}

//---------------------------------------------------

TLevelReaderTzl::TLevelReaderTzl(const TFilePath &path)
    : TLevelReader(path)
    , m_chan(0)
    , m_res(0, 0)
    , m_xDpi(0)
    , m_yDpi(0)
    , m_version(0)
    , m_frameOffsTable()
    , m_iconOffsTable()
    , m_level()
    , m_readPalette(true) {
  m_chan = fopen(path, "rb");

  if (!m_chan) return;

  if (!readHeaderAndOffsets(m_chan, m_frameOffsTable, m_iconOffsTable, m_res,
                            m_version, m_creator, 0, 0, 0, m_level))
    return;

  TFilePath historyFp = path.withNoFrame().withType("hst");
  FILE *historyChan   = fopen(historyFp, "r");
  if (historyChan) {
    // read the whole file into historyData string
    fseek(historyChan, 0, SEEK_END);
    long lSize = ftell(historyChan);
    rewind(historyChan);
    std::string historyData(lSize, '\0');
    fread(&historyData[0], 1, lSize, historyChan);
    fclose(historyChan);

    if (!m_contentHistory) m_contentHistory = new TContentHistory(true);
    m_contentHistory->deserialize(QString::fromStdString(historyData));
  }

  // UCHAR *imgBuff = new UCHAR[imgBuffSize];

  // fclose(m_chan);
  // m_chan = 0;
}

//-------------------------------------------------------------------

void TLevelReaderTzl::doReadPalette(bool doReadIt) { m_readPalette = doReadIt; }

//-------------------------------------------------------------------

TLevelReaderTzl::~TLevelReaderTzl() {
  if (m_chan) fclose(m_chan);
  m_chan = 0;
}

//-------------------------------------------------------------------

TLevelP TLevelReaderTzl::loadInfo() {
  if (m_level && m_level->getPalette() == 0 && m_readPalette) readPalette();
  return m_level;
}

//-------------------------------------------------------------------

TImageReaderP TLevelReaderTzl::getFrameReader(TFrameId fid) {
  if (m_level && m_level->getPalette() == 0 && m_readPalette) readPalette();

  return new TImageReaderTzl(getFilePath(), fid, this);
}

//-------------------------------------------------------------------
QString TLevelReaderTzl::getCreator() {
  if (m_version < 14) return "";
  return m_creator;
}

//-------------------------------------------------------------------
bool TLevelReaderTzl::getIconSize(TDimension &iconSize) {
  if (m_iconOffsTable.empty()) return false;
  if (m_version < 13) return false;
  assert(m_chan);
  TINT32 currentPos         = ftell(m_chan);
  TzlOffsetMap::iterator it = m_iconOffsTable.begin();
  TINT32 offs               = it->second.m_offs;

  fseek(m_chan, offs, SEEK_SET);
  TINT32 iconLx = 0, iconLy = 0;
  // leggo la dimensione delle iconcine nel file
  fread(&iconLx, sizeof(TINT32), 1, m_chan);
  fread(&iconLy, sizeof(TINT32), 1, m_chan);
  assert(iconLx > 0 && iconLy > 0);
  // ritorno alla posizione corrente
  fseek(m_chan, currentPos, SEEK_SET);
  iconSize = TDimension(iconLx, iconLy);
  return true;
}