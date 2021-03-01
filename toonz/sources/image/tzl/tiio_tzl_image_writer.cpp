#include "tiio_tzl_level_writer.h"
#include "tiio_tzl_image_writer.h"

void TImageWriterTzl::save(const TImageP &img) {
  m_lwp->save(img, m_fid);
}