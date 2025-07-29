#include "mat.h"
#include "ppm.h"
#include <cmath>

void PackBitsEncode(const mat<uint8_t>& img, std::vector<uint8_t>& encoded){
    std::vector<uint8_t> img_vec;
    for (uint32_t i=0; i<img.rows(); ++i) {
        for (uint32_t j=0; j<img.cols(); ++j) {
            img_vec.push_back(img(i,j));
        }
    }

    for (uint32_t i=0; i<img_vec.size();) {
        uint32_t count = 1;
        while (i+count < img_vec.size() && img_vec[i+count] == img_vec[i]) {
            ++count;
        }
        if (count != 1) {
            encoded.push_back(257-count);
            encoded.push_back(img_vec[i]);
            i+=count;
        }
        else {
            count = 0;
            while (img_vec[i+count] != img_vec[i+count+1]) {
                ++count;
            }
            encoded.push_back(count-1);
            for (uint32_t j=0; j<count; ++j) {
                encoded.push_back(img_vec[i+j]);
            }
            i += count;
        }
    }
    encoded.push_back(128);
}

std::string Base64Encode(const std::vector<uint8_t>& v) {
    const std::string encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::vector<uint8_t> pad_v = v;
    if (v.size() % 3 != 0) {
        for (uint8_t i=0; i < 3-(v.size() % 3); ++i) {
            pad_v.push_back(128);
        }
    }
    std::vector<uint32_t> grouped_v;
    for (uint32_t i=0; i<pad_v.size(); i+=3) {
        uint32_t num = 0;
        num += pad_v[i] << 16;
        num += pad_v[i+1] << 8;
        num += pad_v[i+2];
        grouped_v.push_back(num);
    }
    std::string new_v;
    for (uint32_t i=0; i<grouped_v.size(); ++i) {
        for (int j=3; j>=0; --j) {
            uint8_t num=0;
            num = (grouped_v[i] >> j*6) & 0b00111111;
            new_v += encoding[num];
        }
    }
    return new_v;
}