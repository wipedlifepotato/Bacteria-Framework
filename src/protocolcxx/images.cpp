#include "images.h"
namespace bacteria {
namespace images {
RGBa GD::getRandColor(void) { return gdRandColor(); }
void GD::initColor(std::string &name, RGBa &color) {
  mColors[name] = gdInitColor(&mImage, &color);
}
void GD::initColor(std::string &name, ui r, ui g, ui b, ui a) {
  RGBa color{r, g, b, a};
  initColor(name, color);
}
int GD::initColor(ui r, ui g, ui b, ui a) {
  RGBa color{r, g, b, a};
  return gdInitColor(&mImage, &color);
}
int GD::initColor(RGBa &color) { return gdInitColor(&mImage, &color); }
int GD::getColor(std::string n) { return mColors[n]; }
std::map<std::string, int> GD::getColorsMap(void) { return mColors; }

GD::GD(std::string filename, ui width, ui height) {
  mImage = gdInitImageFromFile(filename.c_str(), width, height);
}
GD::GD(ui width, ui height, RGBa *bgcolor) {
  if (bgcolor == nullptr) {
    RGBa white{255, 255, 255, 255};
    mImage = gdInitImage(width, height, &white);
  } else
    mImage = gdInitImage(width, height, bgcolor);
}
GD::~GD(void) { gdImageClear(&mImage); }
GD::rawImgdata GD::getRawData(unsigned char quality) {
  // void * getImageData(struct image *im, ui quality, int * size);
  int size;
  if (quality > 100)
    quality = 100;
  void *pRaw = getImageData(&mImage, (ui)quality, &size);
  return {reinterpret_cast<unsigned char *>(pRaw), size};
}
void GD::saveToFile(std::string filepath, int quality) {
  writeToFile(gdImageJpeg, filepath.c_str(), &mImage, quality);
}
/*
        Draws
    */
void GD::DrawRect(ui &x, ui &y, ui &x1, ui &y1, int color) {
  if (color == 0)
    color = mBlackColor;
  gdDrawRect(&mImage, x, y, x1, y1, color);
}
void GD::DrawLine(ui &x, ui &y, ui &x1, ui &y1, int color) {
  if (color == 0)
    color = mBlackColor;
  gdDrawLine(&mImage, x, y, x1, y1, color);
}
void GD::DrawPixel(ui &x, ui &y, int color) {
  if (color == 0)
    color = mBlackColor;
  gdSetPixel(&mImage, x, y, color);
}
void GD::DrawText(std::string text, double ptSize, double angle, int x, int y,
                  std::string fontPath, int color) {
  gdImageStringTTF(mImage.im, NULL, color, const_cast<char *>(fontPath.c_str()),
                   ptSize, angle, x, y, const_cast<char *>(text.c_str()));
}
/*randoms*/
std::string GD::getRandStr(size_t len) {
  std::unique_ptr<char> p{(new char(len + 1))};
  gdGetRandStr(reinterpret_cast<unsigned char *>(p.get()), len);
  return std::string{p.get()};
}
void GD::drawRandLines(ui count) { gdDrawRandomLines(&mImage, count); }
void GD::drawRandPixels(ui min, ui max) {
  gdDrawRandomPixels(&mImage, min, max);
}
} // namespace images

} // namespace bacteria
