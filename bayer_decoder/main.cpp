#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <stdlib.h>

template<typename T>
struct mat {
    uint32_t rows_, cols_;
    std::vector<T> data_;

    mat(uint32_t r, uint32_t c) : rows_(r), cols_(c), data_(r*c) {}

    void resize(uint32_t r, uint32_t c) {
        rows_ = r;
        cols_ = c;
        data_.resize(r*c);
    }

    uint32_t getRows() {return rows_;}
    uint32_t getCols() {return cols_;}

    T& operator[](uint32_t pos) {return data_[pos];}
    const T& operator[](uint32_t pos) const {return data_[pos];}

    T& operator()(uint32_t r, uint32_t c) {return data_[r*cols_ + c];}
    const T& operator()(uint32_t r, uint32_t c) const {return data_[r*cols_ +c];}

    uint32_t raw_size() {return data_.size()*sizeof(T);}
    const uint32_t raw_size() const {return data_.size()*sizeof(T);}

    uint32_t size() {return data_.size();}
    const uint32_t size() const {return data_.size();}

    auto raw_data() {return data_.data();}
};

bool read_pgm(std::istream& is, mat<uint16_t>& img) {
    std::string magic(2, ' ');
    is.read(reinterpret_cast<char*>(&magic), 2);
    if (magic!="P5") return false;
    is.get();
    uint32_t width, height, maxval;
    is >> width >> height >> maxval;
    is.get();
    img.resize(height, width);
    uint32_t count = 0;
    while (is.peek()!=EOF) {
        uint8_t MSB = is.get(), LSB = is.get();
        uint16_t val = (MSB << 8) | LSB;
        img[count] = val;
        ++count;
    }
    return true;
}

bool write_pgm(const std::string& filename, mat<uint16_t>& img, mat<uint8_t>& gray) {
    std::ofstream os(filename, std::ios::binary);
    if (!os) return false;
    gray.resize(img.rows_, img.cols_);
    for (uint32_t i=0; i<img.size(); ++i) {
        gray[i] = img[i]/256;
    }
    os << "P5\n" << gray.cols_ <<"\n" << gray.rows_ << "\n255\n";
    os.write(reinterpret_cast<const char*>(gray.raw_data()), gray.raw_size());
    return true;
}

bool bayer(mat<uint8_t>& img, mat<std::array<uint8_t, 3>>& color_img) {
    color_img.resize(img.rows_, img.cols_);
    for (uint32_t r=0; r<img.getRows(); ++r) {
        for (uint32_t c=0; c<img.getCols(); ++c) {
            if (r%2==0) {
                if (c%2==0) {
                    color_img(r,c)[0] = img(r,c);
                    color_img(r,c)[1] = color_img(r,c)[2] = 0;
                }
                else {
                    color_img(r,c)[1] = img(r,c);
                    color_img(r,c)[0] = color_img(r,c)[2] = 0;
                }
            }
            else {
                if (c%2 == 0) {
                    color_img(r,c)[1] = img(r,c);
                    color_img(r,c)[0] = color_img(r,c)[2] = 0;
                }
                else {
                    color_img(r,c)[2] = img(r,c);
                    color_img(r,c)[0] = color_img(r,c)[1] = 0;
                }
            }
        }
    }
    return true;
}

void increase_dim(mat<std::array<uint8_t, 3>>& img, mat<std::array<uint8_t, 3>>& new_img) {
    new_img.resize(img.rows_+4, img.cols_+4);
    for (uint32_t i=0; i<new_img.rows_; ++i) {
        for (uint32_t j=0; j<new_img.cols_; ++j) {
            if (i==0 || j==0 || i==img.rows_+3 || j==img.cols_+3 || i==1 || j==1 || i==img.rows_+2 || j==img.cols_+2) new_img(i,j) = {0,0,0};
            else new_img(i,j) = img(i-2,j-2);
        }
    }
}

bool write_ppm(const std::string& filename, mat<std::array<uint8_t, 3>>& img) {
    std::ofstream os(filename, std::ios::binary);
    if (!os) return false;
    os << "P6\n" << img.cols_ << "\n" << img.rows_ << "\n255\n";
    os.write(reinterpret_cast<const char *>(img.raw_data()), img.raw_size());
    return true;
}

void green(mat<std::array<uint8_t, 3>>& old_color) {
    mat<std::array<uint8_t, 3>> color(0,0);
    increase_dim(old_color, color);
    for (uint32_t r=2; r<color.getRows()-2; ++r) {
        for (uint32_t c=2; c<color.getCols()-2; ++c) {
            uint8_t G2 = color(r-1,c)[1], G4 = color(r,c-1)[1],
            G6 = color(r,c+1)[1], G8 = color(r+1,c)[1];
            std::array<uint8_t, 3> X1v = color(r-2,c), X5v = color(r,c), X9v = color(r+2,c),
            X3v = color(r,c-2), X7v = color(r,c+2);
            int32_t res;
            if ((r%2==0) && (c%2==0)) {
                uint8_t X1 = X1v[0], X5 = X5v[0], X7 = X7v[0], X9 = X9v[0], X3 = X3v[0];
                uint16_t delta_h = abs(G4-G6) + abs(2*X5-X3-X7);
                uint16_t delta_v = abs(G2-G8) + abs(2*X5-X1-X9);
                if (delta_h < delta_v) { res = static_cast<int>(double((G4+G6))/2.0+double((2*X5-X3-X7))/4.0); }
                else if (delta_h > delta_v) { res = static_cast<int>(double((G2+G8))/2.0 + double((2*X5-X1-X9))/4.0); }
                else { res = static_cast<int>(double((G2+G4+G6+G8))/4.0 + double((4*X5-X1-X3-X7-X9))/8.0); }
            }
            else if ((r%2==1) && (c%2==1)) {
                uint8_t X1 = X1v[2], X5 = X5v[2], X7 = X7v[2], X9 = X9v[2], X3 = X3v[2];
                uint16_t delta_h = abs(G4-G6) + abs(2*X5-X3-X7);
                uint16_t delta_v = abs(G2-G8) + abs(2*X5-X1-X9);
                if (delta_h < delta_v) { res = static_cast<int>(double((G4+G6))/2.0+double((2*X5-X3-X7))/4.0); }
                else if (delta_h > delta_v) { res = static_cast<int>(double((G2-G8))/2.0 + double((2*X5-X1-X9))/4.0); }
                else { res = static_cast<int>(double((G2+G4+G6+G8))/4.0 + double((4*X5-X1-X3-X7-X9))/8.0); }
            }
            if (res < 0) res = 0;
            else if (res > 255) res = 255;
            if ((r%2)==(c%2)) color(r,c)[1] = res;
        }
    }
    for (uint32_t r=2; r<color.getRows()-2; ++r) {
        for (uint32_t c=2; c<color.getCols()-2; ++c) {
            old_color(r-2,c-2) = color(r,c);
        }
    }
}

void r_b_green(mat<std::array<uint8_t, 3>>& old_color) {
    mat<std::array<uint8_t, 3>> color(0,0);
    increase_dim(old_color, color);
    for (uint32_t r=2; r<color.getRows()-2; ++r) {
        for (uint32_t c=2; c<color.getCols()-2; ++c) {
            std::array<uint8_t, 3> X1v = color(r-1,c), X9v = color(r+1,c),
            X3v = color(r,c-1), X7v = color(r,c+1);
            int32_t red, blue;
            if ((r%2==0) && (c%2==1)) {
                uint8_t X1 = X1v[2], X9 = X9v[2], X3 = X3v[0], X7 = X7v[0];
                color(r,c)[0] = static_cast<uint8_t>(double((X3+X7))/2.0);
                color(r,c)[2] = static_cast<uint8_t>(double((X1+X9))/2.0);
            }
            else if ((r%2==1) && (c%2==0)) {
                uint8_t X1 = X1v[0], X9 = X9v[0], X3 = X3v[2], X7 = X7v[2];
                color(r,c)[2] = static_cast<uint8_t>(double((X3+X7))/2.0);
                color(r,c)[0] = static_cast<uint8_t>(double((X1+X9))/2.0);
            }
        }
    }
    for (uint32_t r=2; r<color.getRows()-2; ++r) {
        for (uint32_t c=2; c<color.getCols()-2; ++c) {
            old_color(r-2,c-2) = color(r,c);
        }
    }
}

void red_blue(mat<std::array<uint8_t, 3>>& old_color) {
    mat<std::array<uint8_t, 3>> color(0,0);
    increase_dim(old_color, color);
    for (uint32_t r=2; r<color.getRows()-2; ++r) {
        for (uint32_t c=2; c<color.getCols()-2; ++c) {
            uint8_t G1 = color(r-1,c-1)[1], G3 = color(r-1,c+1)[1],
            G7 = color(r+1,c-1)[1], G9 = color(r+1,c+1)[1], G5 = color(r,c)[1];
            std::array<uint8_t, 3> X1v = color(r-1,c-1), X9v = color(r+1,c+1),
            X3v = color(r-1,c+1), X7v = color(r+1,c-1);
            uint8_t X1,X9,X7,X3;
            int32_t res=0;
            if ((r%2==0) && (c%2==0)) {
                X1 = X1v[2], X7 = X7v[2], X9 = X9v[2], X3 = X3v[2];
            }
            else if((r%2==1) && (c%2==1)) {
                X1 = X1v[0], X7 = X7v[0], X9 = X9v[0], X3 = X3v[0];
            }
            if ((r%2) == (c%2)) {
                uint16_t delta_n = abs(G1-G9) + abs(2*G5-G1-G9);
                uint16_t delta_p = abs(G3-G7) + abs(2*G5-G3-G7);
                if (delta_n < delta_p) { res = static_cast<int>(double((X1+X9))/2.0+double((2*G5-G1-G9))/4.0); }
                else if (delta_n > delta_p) { res = static_cast<int>(double((X3+X7))/2.0 + double((2*G5-G3-G7))/4.0); }
                else { res = static_cast<int>(double((X1+X3+X7+X9))/4.0 + double((4*G5-G1-G3-G7-G9))/8.0); }

                if (res < 0) res = 0;
                else if (res > 255) res = 255;
            }
            if ((r%2==0) && (c%2==0)) color(r,c)[2] = res;
            else if ((r%2==1) && (c%2==1)) color(r,c)[0] = res;
        }
    }
    for (uint32_t r=2; r<color.getRows()-2; ++r) {
        for (uint32_t c=2; c<color.getCols()-2; ++c) {
            old_color(r-2,c-2) = color(r,c);
        }
    }
}

int main(int argc, char * argv[]) {
    std::ifstream is(argv[1]);
    if (!is) return EXIT_FAILURE;
    mat<uint16_t> img(0,0);
    mat<uint8_t> gray(0,0);
    mat<std::array<uint8_t, 3>> color(0,0);
    read_pgm(is, img);
    std::string s = argv[2];
    write_pgm(s+"_gray.pgm", img, gray);
    bayer(gray, color);
    write_ppm(s+"_bayer.ppm", color);
    green(color);
    write_ppm(s+"_green.ppm", color);
    r_b_green(color);
    red_blue(color);
    write_ppm(s+"_interp.ppm", color);
    return 0;
}