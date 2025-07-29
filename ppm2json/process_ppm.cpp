#include "ppm.h"
#include <fstream>
#include <string>

bool LoadPPM(const std::string& filename, mat<vec3b>& img) {
    std::ifstream is(filename, std::ios::binary);
    if (!is) return false;

    std::string magic(2, ' ');
    is.read(reinterpret_cast<char *>(&magic), 2);
    if (magic!="P6") return false;

    is.get();
    if (is.peek() == '#') {
        std::string line;
        std::getline(is, line);
    }
    uint32_t width, height, max_value;
    is >> width;
    is.get();
    is >> height;
    is.get();
    is >> max_value;
    is.get();

    img.resize(height,width);

    for (uint32_t i=0; i<height; ++i) {
        for (uint32_t j=0; j<width; ++j) {
            uint8_t r = is.get(),g = is.get(), b = is.get();
            vec3b v(r,g,b);
            img(i,j) = v;
        }
    }

    return true;
}

void SplitRGB(const mat<vec3b>& img, mat<uint8_t>& img_r, mat<uint8_t>& img_g, mat<uint8_t>& img_b) {
    for (uint32_t i=0; i<img.rows(); ++i) {
        for (uint32_t j=0; j<img.cols(); ++j) {
            img_r(i,j) = img(i,j)[0];
            img_g(i,j) = img(i,j)[1];
            img_b(i,j) = img(i,j)[2];
        }
    }
}
