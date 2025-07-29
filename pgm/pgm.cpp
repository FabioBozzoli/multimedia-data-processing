#include <string>
#include <fstream>

#include "pgm.h"
#include "mat.h"

void write(const std::string& filename, pgm_mode mode, const mat<uint8_t>& m) {
    std::ofstream os(filename, std::ios::binary);

    os << "P" << int(mode) << "\n#Creato da Fabio Bozzoli\n" <<
        m.cols() << " " << m.rows() << "\n255\n";

    if (mode == pgm_mode::plain) {
        for (auto it = m.begin(); it != m.end(); ++it) {
            os << int(*it) << " ";
        }
    }
    else {
        os.write(m.rawdata(), m.rawsize());
    }
}


mat<uint8_t> read(const std::string& filename) {
    mat<uint8_t> img;

    std::ifstream is(filename, std::ios::binary);
    if (!is) {
        return img;
    }

    std::string magic_number;
    int width, height, nlevels;
    std::getline(is, magic_number);

    if (is.peek()=='#') {
        std::string comment;
        std::getline(is, comment);
    }
    is >> width >> height >> nlevels;
    is.get();
    img.resize(height, width);
    if (magic_number == "P2") {
        for (auto& x : img) {
            int value;
            is >> value;
            x = value;
        }
    }
    else if (magic_number=="P5") {
        is.read(img.rawdata(), img.rawsize());
    }
    return img;
}