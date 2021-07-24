#ifndef __clang__
module;
#endif
extern "C" {
//#include "lua/luaserv.h"
//#include <lauxlib.h>
//#include <lua.h>
//#include <lualib.h>
//#include"cryptocoins.h"
//#include"json_rpc.h"
//#include"encdec/AES.h"
//#include"encdec/base64.h"
//#include"encdec/hashes.h"
//#include"encdec/rsa_ed25519.h"
//#include"encdec/x25519.h"
#include "images/images.h"
};
#include <map>
#include <memory>
#include <string>

export module images;
export namespace bacteria {
  namespace images {
  class GD {
  protected:
    using rawImgdata = std::pair<const unsigned char *, size_t>;
    struct image mImage;
    std::map<std::string, int> mColors;
    int mBlackColor = initColor(0, 0, 0, 255);

  public:
    RGBa getRandColor(void) { return gdRandColor(); }
    void initColor(std::string &name, RGBa &color) {
      mColors[name] = gdInitColor(&mImage, &color);
    }
    void initColor(std::string &name, ui r, ui g, ui b, ui a) {
      RGBa color{r, g, b, a};
      initColor(name, color);
    }
    int initColor(ui r, ui g, ui b, ui a) {
      RGBa color{r, g, b, a};
      return gdInitColor(&mImage, &color);
    }
    int initColor(RGBa &color) { return gdInitColor(&mImage, &color); }
    int getColor(std::string n) { return mColors[n]; }
    std::map<std::string, int> getColorsMap(void) { return mColors; }
    GD(void) = delete;
    GD(std::string filename, ui width, ui height) {
      mImage = gdInitImageFromFile(filename.c_str(), width, height);
    }
    GD(ui width, ui height, RGBa *bgcolor = nullptr) {
      if (bgcolor == nullptr) {
        RGBa white{255, 255, 255, 255};
        mImage = gdInitImage(width, height, &white);
      } else
        mImage = gdInitImage(width, height, bgcolor);
    }
    ~GD(void) { gdImageClear(&mImage); }
    rawImgdata getRawData(unsigned char quality) {
      // void * getImageData(struct image *im, ui quality, int * size);
      int size;
      if (quality > 100)
        quality = 100;
      void *pRaw = getImageData(&mImage, (ui)quality, &size);
      return {reinterpret_cast<unsigned char *>(pRaw), size};
    }
    void saveToFile(std::string filepath, int quality) {
      writeToFile(gdImageJpeg, filepath.c_str(), &mImage, quality);
    }
    /*
            Draws
    */
    void DrawRect(ui &x, ui &y, ui &x1, ui &y1, int color = 0) {
      if (color == 0)
        color = mBlackColor;
      gdDrawRect(&mImage, x, y, x1, y1, color);
    }
    void DrawLine(ui &x, ui &y, ui &x1, ui &y1, int color = 0) {
      if (color == 0)
        color = mBlackColor;
      gdDrawLine(&mImage, x, y, x1, y1, color);
    }
    void DrawPixel(ui &x, ui &y, int color = 0) {
      if (color == 0)
        color = mBlackColor;
      gdSetPixel(&mImage, x, y, color);
    }
    void DrawText(std::string text, double ptSize, double angle, int x, int y,
                  std::string fontPath = "./fonts/dummy.ttf", int color = 0) {
      gdImageStringTTF(mImage.im, NULL, color,
                       const_cast<char *>(fontPath.c_str()), ptSize, angle, x,
                       y, const_cast<char *>(text.c_str()));
    }
    /*randoms*/
    std::string getRandStr(size_t len = 12) {
      std::unique_ptr<char> p{new char(len) + 1};
      gdGetRandStr(reinterpret_cast<unsigned char *>(p.get()), len);
      return std::string{p.get()};
    }
    void drawRandLines(ui count) { gdDrawRandomLines(&mImage, count); }
    void drawRandPixels(ui min, ui max) {
      gdDrawRandomPixels(&mImage, min, max);
    }
  };
  }; // namespace images
};
