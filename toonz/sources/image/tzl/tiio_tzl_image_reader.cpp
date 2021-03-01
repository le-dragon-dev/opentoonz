#include <tcodec.h>
#include <tpixelcm.h>
#include <timageinfo.h>
#include <ttoonzimage.h>
#include <trop.h>

#include "tiio_tzl_level_reader.h"
#include "tiio_tzl_image_reader.h"

// Restituisce la regione del raster shrinkata e la relativa savebox.
static TRect applyShrinkAndRegion(TRasterP &ras, int shrink, TRect region,
                                  TRect savebox) {
  // estraggo la regione solo se essa ha coordinate valide.
  if (!region.isEmpty() && region != TRect() && region.getLx() > 0 &&
      region.getLy() > 0)
    ras = ras->extract(region);
  else
    region = TRect(0, 0, ras->getLx() - 1, ras->getLy() - 1);
  if (shrink > 1) {
    ras = TRop::shrink(ras, shrink);
  }
  // calcolo la nuova savebox
  savebox *= region;
  if (savebox == TRect() || savebox.getLx() <= 0 || savebox.getLy() <= 0)
    return TRect();
  int firstColIndexOfLayer = (savebox.x0 - region.x0) - 1 + shrink -
                             (abs(savebox.x0 - region.x0 - 1) % shrink);
  int firstRowIndexOfLayer = (savebox.y0 - region.y0) - 1 + shrink -
                             (abs(savebox.y0 - region.y0 - 1) % shrink);
  savebox.x0 = (firstColIndexOfLayer) / shrink;
  savebox.y0 = (firstRowIndexOfLayer) / shrink;
  savebox.x1 = savebox.x0 + (savebox.getLx()) / shrink - 1;
  savebox.y1 = savebox.y0 + (savebox.getLy()) / shrink - 1;
  return savebox;
}

TImageReaderTzl::TImageReaderTzl(const TFilePath &f, const TFrameId &fid,
                                 TLevelReaderTzl *lr)
    : TImageReader(f)
    , m_fid(fid)
    , m_lrp(lr)
    , m_lx(lr->m_res.lx)
    , m_ly(lr->m_res.ly)
    , m_isIcon(false) {}

//-------------------------------------------------------------------

TImageP TImageReaderTzl::load() {
  int version   = m_lrp->m_version;
  assert(version == 14); // Accept only version 14 after OpenToonZ 1.5
  TImageP image = TImageP();


  FILE *chan = m_lrp->m_chan;

  if (!chan) return TImageP();
  // SAVEBOX_X0 SAVEBOX_Y0 SAVEBOX_LX SAVEBOX_LY BUFFER_SIZE
  TINT32 sbx0 = 0, sby0 = 0, sblx, sbly;
  TINT32 actualBuffSize;
  double xdpi = 1, ydpi = 1;
  // TINT32 imgBuffSize = 0;
  UCHAR *imgBuff = 0;
  TINT32 iconLx = 0, iconLy = 0;
  assert(!m_lrp->m_frameOffsTable.empty());
  assert(!m_lrp->m_iconOffsTable.empty());
  if (m_lrp->m_frameOffsTable.empty())
    throw TException("Loading tlv: the frames table is empty.");
  if (m_lrp->m_iconOffsTable.empty())
    throw TException("Loading tlv: the frames icons table is empty.");

  TzlOffsetMap::iterator it     = m_lrp->m_frameOffsTable.find(m_fid);
  TzlOffsetMap::iterator iconIt = m_lrp->m_iconOffsTable.find(m_fid);
  if (it == m_lrp->m_frameOffsTable.end() ||
      iconIt == m_lrp->m_iconOffsTable.end())
    throw TException("Loading tlv: frame ID not found.");

  fseek(chan, it->second.m_offs, SEEK_SET);
  fread(&sbx0, sizeof(TINT32), 1, chan);
  fread(&sby0, sizeof(TINT32), 1, chan);
  fread(&sblx, sizeof(TINT32), 1, chan);
  fread(&sbly, sizeof(TINT32), 1, chan);
  fread(&actualBuffSize, sizeof(TINT32), 1, chan);
  fread(&xdpi, sizeof(double), 1, chan);
  fread(&ydpi, sizeof(double), 1, chan);

  if (sbx0 < 0 || sby0 < 0 || sblx < 0 || sbly < 0 || sblx > m_lx ||
      sbly > m_ly)
    throw TException("Loading tlv: savebox dimension error.");

#if !TNZ_LITTLE_ENDIAN
  sbx0           = swapTINT32(sbx0);
  sby0           = swapTINT32(sby0);
  sblx           = swapTINT32(sblx);
  sbly           = swapTINT32(sbly);
  actualBuffSize = swapTINT32(actualBuffSize);
  reverse((char *)&xdpi, sizeof(double));
  reverse((char *)&ydpi, sizeof(double));
#endif

  // Carico l'icona dal file
  if (m_isIcon) {
    fseek(chan, iconIt->second.m_offs, SEEK_SET);
    fread(&iconLx, sizeof(TINT32), 1, chan);
    fread(&iconLy, sizeof(TINT32), 1, chan);
    assert(iconLx > 0 && iconLy > 0);
    if (iconLx < 0 || iconLy < 0 || iconLx > m_lx || iconLy > m_ly)
      throw TException("Loading tlv: bad icon size.");
    fread(&actualBuffSize, sizeof(TINT32), 1, chan);

    if (actualBuffSize <= 0 ||
        actualBuffSize > (int)(iconLx * iconLx * sizeof(TPixelCM32)))
      throw TException("Loading tlv: icon buffer size error.");

    TRasterCM32P raux = TRasterCM32P(iconLx, iconLy);
    if (!raux) return TImageP();
    raux->lock();
    imgBuff = (UCHAR *)raux->getRawData();  // new UCHAR[imgBuffSize];
    fread(imgBuff, actualBuffSize, 1, chan);

#if !TNZ_LITTLE_ENDIAN
    Header *header    = (Header *)imgBuff;
    header->m_lx      = swapTINT32(header->m_lx);
    header->m_ly      = swapTINT32(header->m_ly);
    header->m_rasType = (Header::RasType)swapTINT32(header->m_rasType);
#endif

    TRasterCodecLZO codec("LZO", false);
    TRasterP ras;
    if (!codec.decompress(imgBuff, actualBuffSize, ras, m_safeMode))
      return TImageP();
    assert((TRasterCM32P)ras);
    raux->unlock();

#if !TNZ_LITTLE_ENDIAN
    for (int y = 0; y < ras->getLy(); ++y) {
      ras->lock();
      TINT32 *pix    = ((TINT32 *)ras->getRawData(0, y));
      TINT32 *endPix = pix + ras->getLx();
      while (pix < endPix) {
        *pix = swapTINT32(*pix);
        pix++;
      }
      ras->unlock();
    }
#endif

    if (m_lrp->m_res.lx == 0 || m_lrp->m_res.ly == 0) return TImageP();
    // Compute savebox
    if (m_lrp->m_res.lx < 0 || m_lrp->m_res.ly < 0)
      throw TException("Loading tlv: icon resolution error");

    TRect tmp_savebox;
    TRect savebox;
    TRop::computeBBox(ras, tmp_savebox);
    if (tmp_savebox.isEmpty()) {
      TINT32 iconsbx0 = tround((double)iconLx * sbx0 / m_lrp->m_res.lx);
      TINT32 iconsby0 = tround((double)iconLy * sby0 / m_lrp->m_res.ly);
      TINT32 iconsblx =
          std::max(tround((double)iconLx * sblx / m_lrp->m_res.lx), 1);
      TINT32 iconsbly =
          std::max(tround((double)iconLy * sbly / m_lrp->m_res.ly), 1);
      savebox =
          TRect(TPoint(iconsbx0, iconsby0), TDimension(iconsblx, iconsbly));
    } else {
      TINT32 iconsbx0 = tfloor((double)iconLx * sbx0 / m_lrp->m_res.lx);
      TINT32 iconsby0 = tfloor((double)iconLy * sby0 / m_lrp->m_res.ly);
      savebox         = TRect(TPoint(iconsbx0, iconsby0),
                              TDimension(tmp_savebox.getLx(), tmp_savebox.getLy()));
    }

    TDimension imgSize(iconLx, iconLy);
    if (!TRect(imgSize).contains(
        savebox))  // for this 'if', see comment in createIcon method. vinz
      savebox = savebox * TRect(imgSize);

    if (imgSize != savebox.getSize()) {
      TRasterCM32P fullRas(imgSize);
      TPixelCM32 bgColor;

      fullRas->fillOutside(savebox, bgColor);

      if (savebox.getSize() != ras->getSize())
        throw TException("Loading tlv: bad icon savebox size.");
      fullRas->extractT(savebox)->copy(ras);
      ras = fullRas;
    }

    TToonzImageP ti(ras, savebox);
    ti->setDpi(xdpi, ydpi);
    ti->setPalette(m_lrp->m_level->getPalette());
    return ti;
  }
  if (actualBuffSize <= 0 ||
      actualBuffSize > (int)(m_lx * m_ly * sizeof(TPixelCM32)))
    throw TException("Loading tlv: buffer size error");

  TRasterCM32P raux = TRasterCM32P(m_lx, m_ly);

  raux->lock();
  imgBuff = (UCHAR *)raux->getRawData();

  fread(imgBuff, actualBuffSize, 1, chan);
  Header *header = (Header *)imgBuff;

#if !TNZ_LITTLE_ENDIAN
  header->m_lx      = swapTINT32(header->m_lx);
  header->m_ly      = swapTINT32(header->m_ly);
  header->m_rasType = (Header::RasType)swapTINT32(header->m_rasType);
#endif

  TRasterCodecLZO codec("LZO", false);
  TRasterP ras;
  if (!codec.decompress(imgBuff, actualBuffSize, ras, m_safeMode))
    return TImageP();
  assert((TRasterCM32P)ras);
  assert(ras->getLx() == header->m_lx);
  assert(ras->getLy() == header->m_ly);
  if (ras->getLx() != header->m_lx)
    throw TException("Loading tlv: lx dimension error.");
  if (ras->getLy() != header->m_ly)
    throw TException("Loading tlv: ly dimension error.");
  raux->unlock();

#if !TNZ_LITTLE_ENDIAN
  for (int y = 0; y < ras->getLy(); ++y) {
    ras->lock();
    TINT32 *pix    = ((TINT32 *)ras->getRawData(0, y));
    TINT32 *endPix = pix + ras->getLx();
    while (pix < endPix) {
      *pix = swapTINT32(*pix);
      pix++;
    }
    ras->unlock();
  }
#endif

  TRect savebox(TPoint(sbx0, sby0), TDimension(sblx, sbly));
  TDimension imgSize(m_lrp->m_res.lx, m_lrp->m_res.ly);
  assert(TRect(imgSize).contains(savebox));
  if (!TRect(imgSize).contains(savebox))
    throw TException("Loading tlv: bad savebox size.");
  if (imgSize != savebox.getSize()) {
    TRasterCM32P fullRas(imgSize);
    TPixelCM32 bgColor;
    if (!savebox.isEmpty()) {
      fullRas->fillOutside(savebox, bgColor);
      assert(savebox.getSize() == ras->getSize());
      if (savebox.getSize() != ras->getSize())
        throw TException("Loading tlv: bad icon savebox size.");
      fullRas->extractT(savebox)->copy(ras);
    } else
      fullRas->clear();
    ras = fullRas;
  }

  TToonzImageP ti(ras, savebox);
  ti->setDpi(xdpi, ydpi);
  ti->setPalette(m_lrp->m_level->getPalette());

  image = ti;

  if (image == TImageP()) return TImageP();

  if (!m_isIcon) {
    TToonzImageP ti = image;
    if (!ti) return TImageP();
    TRasterP ras  = ti->getRaster();
    TRect savebox = ti->getSavebox();
    if (m_region != TRect() && m_region.getLx() > 0 && m_region.getLy() > 0) {
      m_region *= TRect(0, 0, ti->getSize().lx, ti->getSize().ly);
      if (m_region.isEmpty() || m_region == TRect() || m_region.getLx() <= 0 ||
          m_region.getLy() <= 0)
        return TImageP();
    }
    savebox = applyShrinkAndRegion(ras, m_shrink, m_region, savebox);
    if (savebox == TRect()) {
      if (m_region != TRect()) {
        ras = ras->create(m_region.getLx(), m_region.getLy());
        ras->clear();
        savebox = m_region;
      } else {
        // se sia la savebox che la regione sono vuote non faccio nulla
      }
    }
    ti->setCMapped(ras);
    ti->setSavebox(savebox);
    return ti;
  }
  return image;
}

//-------------------------------------------------------------------
const TImageInfo *TImageReaderTzl::getImageInfo() const {
  assert(!m_lrp->m_frameOffsTable.empty());
  FILE *chan = m_lrp->m_chan;
  if (!chan) return 0;

  TzlOffsetMap::iterator it = m_lrp->m_frameOffsTable.find(m_fid);

  if (it == m_lrp->m_frameOffsTable.end()) return 0;

  fseek(chan, it->second.m_offs, SEEK_SET);

  // SAVEBOX_X0 SAVEBOX_Y0 SAVEBOX_LX SAVEBOX_LY BUFFER_SIZE
  TINT32 sbx0, sby0, sblx, sbly;
  TINT32 actualBuffSize;
  double xdpi = 1, ydpi = 1;
  //  TINT32 imgBuffSize = 0;

  // int pos = ftell(chan);

  fread(&sbx0, sizeof(TINT32), 1, chan);
  fread(&sby0, sizeof(TINT32), 1, chan);
  fread(&sblx, sizeof(TINT32), 1, chan);
  fread(&sbly, sizeof(TINT32), 1, chan);
  fread(&actualBuffSize, sizeof(TINT32), 1, chan);

  fread(&xdpi, sizeof(double), 1, chan);
  fread(&ydpi, sizeof(double), 1, chan);

#if !TNZ_LITTLE_ENDIAN
  sbx0           = swapTINT32(sbx0);
  sby0           = swapTINT32(sby0);
  sblx           = swapTINT32(sblx);
  sbly           = swapTINT32(sbly);
  actualBuffSize = swapTINT32(actualBuffSize);
  reverse((char *)&xdpi, sizeof(double));
  reverse((char *)&ydpi, sizeof(double));
#endif

  static TImageInfo info;
  info.m_x0   = sbx0;
  info.m_y0   = sby0;
  info.m_x1   = sbx0 + sblx - 1;
  info.m_y1   = sby0 + sbly - 1;
  info.m_lx   = m_lx;
  info.m_ly   = m_ly;
  info.m_dpix = xdpi;
  info.m_dpiy = ydpi;

  return &info;
}

//-------------------------------------------------------------------
TDimension TImageReaderTzl::getSize() const { return TDimension(m_lx, m_ly); }

//-------------------------------------------------------------------
TRect TImageReaderTzl::getBBox() const { return TRect(getSize()); }

