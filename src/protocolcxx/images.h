#include "images/images.h"

#include <map>
#include <memory>
#include <string>


namespace bacteria {
  namespace images {
  class GD {
  protected:
    using rawImgdata = std::pair<const unsigned char *, size_t>;
    struct image mImage;
    std::map<std::string, int> mColors;
    int mBlackColor = initColor(0, 0, 0, 255);

  public:
    RGBa getRandColor(void) ;
    void initColor(std::string &name, RGBa &color);
    void initColor(std::string &name, ui r, ui g, ui b, ui a);
    int initColor(ui r, ui g, ui b, ui a);
    int initColor(RGBa &color) ;
    int getColor(std::string n) ;
    std::map<std::string, int> getColorsMap(void) ;

    GD(std::string filename, ui width, ui height) ;
    GD(ui width, ui height, RGBa *bgcolor = nullptr) ;
    ~GD(void) ;
    GD(void) = delete;
    rawImgdata getRawData(unsigned char quality=75) ;
    void saveToFile(std::string filepath, int quality=75) ;
    /*
            Draws
    */
    void DrawRect(ui &x, ui &y, ui &x1, ui &y1, int color=0 ) ;
    void DrawLine(ui &x, ui &y, ui &x1, ui &y1, int color=0 ) ;
    void DrawPixel(ui &x, ui &y, int color=0 ) ;
    void DrawText(std::string text, double ptSize, double angle, int x, int y,
                  std::string fontPath = "./fonts/dummy.ttf", int color = 0);
    /*randoms*/
    std::string getRandStr(size_t len = 12) ;
    void drawRandLines(ui count) ;
    void drawRandPixels(ui min, ui max) ;
  };
  }; // namespace images
};
