#include "mat.h"
#include "compress.cpp"
#include "process_ppm.cpp"

std::string JSON(const std::string& filename) {
    mat<vec3b> img(0,0);
    if (!LoadPPM(filename, img)) return "";
    mat<uint8_t> img_r(img.rows(), img.cols()), img_g(img.rows(), img.cols()), img_b(img.rows(), img.cols());
    SplitRGB(img, img_r, img_g, img_b);
    std::vector<uint8_t> encode_r, encode_g, encode_b;
    PackBitsEncode(img_r, encode_r);
    PackBitsEncode(img_g, encode_g);
    PackBitsEncode(img_b, encode_b);
    std::string r = Base64Encode(encode_r);
    std::string g = Base64Encode(encode_g);
    std::string b = Base64Encode(encode_b);
    std::string json = "{\n\t\"width\": " + std::to_string(img.cols());
    json+=",\n\t\"height\": " + std::to_string(img.rows());
    json+=",\n\t\"red\": " + r;
    json+=",\n\t\"green\": " + g;
    json+=",\n\t\"blue\": " + b;
    json+="\n}";
    return json;
}