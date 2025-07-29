#include "pcx.h"
#include "mat.h"
#include <fstream>
#include <string>

bool load_pcx(const std::string& filename, mat<unsigned char>& img) {
    std::ifstream is(filename, std::ios::binary);
    if (!is) return false;
    uint8_t header = is.get();
    if (header != 0x0A) return false;
    is.get();
    is.get();
    is.get();
    uint16_t x_min, y_min, x_max, y_max;
    is.read(reinterpret_cast<char*>(&x_min), 2);
    is.read(reinterpret_cast<char*>(&y_min), 2);
    is.read(reinterpret_cast<char*>(&x_max), 2);
    is.read(reinterpret_cast<char*>(&y_max), 2);
    uint32_t cols = x_max - x_min + 1;
    uint32_t rows = y_max - y_min + 1;
    std::string dummy(53, ' ');
    is.read(reinterpret_cast<char *>(dummy.data()), 53);
    uint8_t color_plane = is.get();
    uint16_t bytes_per_plane_line;
    is.read(reinterpret_cast<char *>(&bytes_per_plane_line), 2);
    std::string dummy2(60, ' ');
    is.read(reinterpret_cast<char *>(dummy2.data()), 60);
    img.resize(rows, cols);
    uint32_t index_c=0;
    uint64_t total_bytes = color_plane * bytes_per_plane_line;
    for (uint32_t r=0; r < rows; ++r) {
        for (uint32_t c=0; c < total_bytes;) {
            if (c==total_bytes-1 && total_bytes%2==1) is.get();
            uint8_t pix = is.get();
            ++c;
            if (((pix >> 7) & 1) && ((pix >> 6) & 1)) {
                uint8_t count = pix &0b00111111;
                ++c;
                uint8_t new_pix = is.get();
                for (uint8_t i=0; i<count; ++i) {
                    for (int16_t j=7; j>=0; --j) {
                        if (index_c >= cols) index_c = 0;
                        uint8_t bit = (new_pix >> j)&1;
                        if (bit == 0) img(r,index_c) = 0;
                        else img(r,index_c) = 255;
                        ++index_c;
                    }
                }
            }
            else {
                for (int16_t j=7; j>=0; --j) {
                    if (index_c >= cols) index_c = 0;
                    uint8_t bit = (pix >> j)&1;
                    if (bit == 0) img(r,index_c) = 0;
                    else img(r,index_c) = 255;
                    ++index_c;
                }
            }
        }
    }
    return true;
}
