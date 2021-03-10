#include "tlevel_io.h"

#include "tiio_tzl_reader.h"

// ****************************************************************************
// ****************************************************************************
TLevelReaderTzl::TLevelReaderTzl(const TFilePath& path) : TLevelReader(path)
{
  // Open the file
  {
    std::ifstream in(path.getWideString(), std::ios::in | std::ios::binary);
    if (!in.is_open())
      return;

    // Load the level header
    in >> m_levelHeader;
  }

  // Check if the level header is valid
  if (!m_levelHeader.isValid())
    return;
}

// ****************************************************************************
// ****************************************************************************
TLevelP TLevelReaderTzl::loadInfo() {
  // Create a level and get its table
  TLevelP level(new TLevel);

  std::ifstream in(m_path.getWideString(), std::ios::in | std::ios::binary);
  if (!in.is_open())
    return {};

  // Fill the level with empty frames a.t.m
  TLVOffsetTable tlvTable = m_levelHeader.getOffsetTable(in, TLVOffsetTableRow::Type::FRAME);
  for(const auto& row: tlvTable) {
    level->setFrame(TFrameId(row.number, row.letter), TImageP());
  }

  return level;
}

// ****************************************************************************
// ****************************************************************************
QString TLevelReaderTzl::getCreator() { return m_levelHeader.creator; }

// ****************************************************************************
// ****************************************************************************
void TLevelReaderTzl::doReadPalette(bool) {}

// ****************************************************************************
// ****************************************************************************
TImageReaderP TLevelReaderTzl::getFrameReader(TFrameId id) {
  return TLevelReader::getFrameReader(id);
}
