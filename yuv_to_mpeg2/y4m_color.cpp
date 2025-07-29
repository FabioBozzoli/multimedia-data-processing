#include <vector>
#include "mat.h"
#include "types.h"
#include <fstream>
#include <unordered_map>
#include <iostream>

#include "ppm.h"

std::unordered_map<char, int> read_header(std::istream& is) {
    char c;
    std::unordered_map<char, int> map;
    while (is.get(c) && c!='\n') {
        if (c=='W') {
            uint64_t w;
            is >> w;
            map['W'] = w;
        }
        if (c=='H') {
            uint64_t h;
            is >> h;
            map['H'] = h;
        }
    }
    is.get(c);
    std::string line;
    std::getline(is, line);
    return map;
}

vec3b& saturation(vec3b& val) {
    if (val[0] < 16){ val[0] = 16; }
    else if (val[0] > 235) { val[0] = 235; }
    if (val[1] < 16){ val[1] = 16; }
    else if (val[1] > 240) { val[1] = 240; }
    if (val[2] < 16){ val[2] = 16; }
    else if (val[2] > 240) { val[2] = 240; }
    return val;
}

bool y4m_extract_color(const std::string& filename, std::vector<mat<vec3b>>& frames) {
    std::ifstream is(filename, std::ios::binary);
    if (!is){ return false; }
    std::unordered_map<char, int> dim = read_header(is);
    char c = is.peek();
    while (c!=EOF) {
        mat<uint8_t> y(dim['H'], dim['W']);
        for (int i=0; i<dim['H']; ++i) {
            for (int j=0; j<dim['W']; ++j) {
                is.get(c);
                y(i,j) = static_cast<uint8_t>(c);
            }
        }
        mat<uint8_t> cb(dim['H']/2, dim['W']/2);
        for (int i=0; i<dim['H']/2; ++i) {
            for (int j=0; j<dim['W']/2; ++j) {
                is.get(c);
                cb(i,j) = static_cast<uint8_t>(c);
            }
        }
        mat<uint8_t> cr(dim['H']/2, dim['W']/2);
        for (int i=0; i<dim['H']/2; ++i) {
            for (int j=0; j<dim['W']/2; ++j) {
                is.get(c);
                cr(i,j) = static_cast<uint8_t>(c);
            }
        }

        mat<vec3b> img_rgb(y.rows(), y.cols());
        for (int i=0; i<y.rows(); ++i) {
            for (int j=0; j<y.cols(); ++j) {
                vec3b ycbcr {y(i,j),cb(i/2,j/2),cr(i/2,j/2)};
                saturation(ycbcr);
                uint8_t r = static_cast<uint8_t>(std::clamp((1.164*(ycbcr[0]-16) +
                                                            1.596*(ycbcr[2]-128)), 0.0,
                                                            255.0));
                uint8_t g = static_cast<uint8_t>(std::clamp((1.164*(ycbcr[0]-16) -
                                                            0.392*(ycbcr[1]-128) -
                                                            0.813*(ycbcr[2]-128)), 0.0,
                                                            255.0));
                uint8_t b = static_cast<uint8_t>(std::clamp((1.164*(ycbcr[0]-16) +
                                                            2.017*(ycbcr[1]-128)), 0.0,
                                                            255.0));
                vec3b a {r,g,b};
                img_rgb(i,j) = a;
            }
        }
        frames.push_back(img_rgb);
        std::string line;
        std::getline(is, line);
        c = is.peek();
    }
    return true;
}
/*
int main(int argc, char* argv[]) {
    std::vector<mat<vec3b>> frames;
    y4m_extract_color(argv[1], frames);
    std::string s;
    for (char i=0; i<30; ++i) {
        char ss = 'a'+i;
        s += ss;
        save_ppm(s+".ppm", frames[i], true);
    }
    return 0;
}
*/