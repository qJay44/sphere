#include "Tif.hpp"

Tif::Tif(const fspath& path) {
  tif = TIFFOpen(path.string().c_str(), "r");
  if (!tif) {
    printf("Tiff can't open the file (%s)\n", path.string().c_str());
    exit(EXIT_FAILURE);
  }

  u32 tags = TIFFGetTagListCount(tif);
  printf("Tags: %d\n", tags);
  for (u32 i = 0; i < tags; i++) {
    printf("Tag %d: %d\n", i, TIFFGetTagListEntry(tif, i));
  }
}

Tif::~Tif() {
  TIFFClose(tif);
}

u32 Tif::width() const {
  u32 r;
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &r);
  return r;
}

u32 Tif::height() const {
  u32 r;
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &r);
  return r;
}

u32 Tif::tileWidth() const {
  u32 r;
  TIFFGetField(tif, TIFFTAG_TILEWIDTH, &r);
  return r;
}

u32 Tif::tileHeight() const {
  u32 r;
  TIFFGetField(tif, TIFFTAG_TILELENGTH, &r);
  return r;
}

u32 Tif::samples() const {
  u32 r;
  TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &r);
  return r;
}

u32 Tif::strips() const {
  return TIFFNumberOfStrips(tif);
}

u32 Tif::stripSize() const {
  return TIFFStripSize(tif);
}

void Tif::printInfo() const {
  const char* msg = \
    "w: %d, h: %d, tile width: %d, tile height: %d\n" \
    "samples: %d, strips: %d, srip size: %d\n";
  printf(msg, width(), height(), tileWidth(), tileHeight(), samples(), strips(), stripSize());
}
