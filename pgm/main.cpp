#include <fstream>
#include "pgm.h"

auto make_example(int rows, int cols) {
    mat<uint8_t> m(rows,cols);

    for (int r = 0; r < m.rows(); ++r) {
        for (int c = 0; c < m.cols(); ++c) {
            m(r,c) = r;
        }
    }

    return m;
}

void flip(mat<uint8_t>& m) {
    for (int r=0; r<m.rows()/2; ++r) {
        for (int c=0; c<m.cols(); ++c) {
            std::swap(m(r,c), m(m.rows()-1-r, c));
        }
    }
}

int main() {
    mat<uint8_t> m = read("frog_bin.pgm");
    flip(m);
    write("reverse.pgm", pgm_mode::binary, m);
    return 0;
}