#include <iostream>
#include <fstream>
#include <vector>
#include <array>

struct mat {
    uint32_t rows_, cols_;
    std::vector<std::array<uint8_t, 3>> pixels_;

    mat(const uint32_t r, const uint32_t c) : rows_(r), cols_(c) {}

    std::array<uint8_t, 3> operator()(const uint32_t r, const uint32_t c) {
        return pixels_[r*cols_+c];
    }

    std::array<uint8_t, 3> operator[](const uint32_t pos) { return pixels_[pos]; }
    const std::array<uint8_t, 3> operator[](const uint32_t pos) const { return pixels_[pos]; }

    void set_rows(const uint32_t r) { rows_ = r; }
    void set_cols(const uint32_t c) { cols_ = c; }

    uint32_t get_rows() { return rows_; }
    const uint32_t get_rows() const { return rows_; }

    uint32_t get_cols() { return cols_; }
    const uint32_t get_cols() const { return cols_; }

    uint32_t size() { return rows_*cols_; }
    const uint32_t size() const { return rows_*cols_; }

    auto raw_data() { return pixels_.data(); }
    const auto raw_data() const { return pixels_.data(); }

    auto raw_size() { return pixels_.size()*3; }
    const auto raw_size() const { return pixels_.size()*3; }

    void add_pix(const uint8_t r,const uint8_t g,const uint8_t b) { pixels_.push_back({r,g,b}); }
};

mat read_header(std::istream& is, uint32_t& starting_point) {
    uint16_t header_field;
    uint32_t rows, cols;
    mat img(0,0);
    is.read(reinterpret_cast<char *>(&header_field), sizeof(header_field));
    if (header_field != 0x4D42) return img;
    for (uint8_t i=0; i<8; ++i) is.get();
    is.read(reinterpret_cast<char *>(&starting_point), sizeof(starting_point));
    for (uint8_t i=0; i<4; ++i) is.get();
    is.read(reinterpret_cast<char *>(&cols), sizeof(cols));
    is.read(reinterpret_cast<char *>(&rows), sizeof(rows));
    for (uint8_t i=0; i<28; ++i) is.get();
    img.set_rows(rows);
    img.set_cols(cols);
    return img;
}

void read_pixels(std::istream& is, mat& img) {
    uint8_t pad = (32-(img.get_cols()*8*3)%32)/8;
    while (is.peek() != EOF){
        for (uint32_t i=0; i<img.get_cols(); ++i) {
            uint8_t b = is.get();
            uint8_t g = is.get();
            uint8_t r = is.get();
            img.add_pix(r,g,b);
        }
        for (uint8_t i=0; i<pad; ++i) is.get();
    }
}

void write_pam(std::ostream& os, mat& img) {
    os << "P7\nWIDTH " << img.cols_ << "\nHEIGHT " << img.rows_
    << "\nDEPTH 3\nMAXVAL 255\nTUPLTYPE RGB\nENDHDR\n";
    for (int64_t r=img.get_rows()-1; r>=0; --r) {
        for (uint32_t c=0; c<img.get_cols(); ++c) {
            os.put(img(r,c)[0]);
            os.put(img(r,c)[1]);
            os.put(img(r,c)[2]);
        }
    }
}

int main(int argc, char * argv[]) {
    if (argc!=3) return EXIT_FAILURE;
    std::ifstream is(argv[1], std::ios::binary);
    if (!is) return EXIT_FAILURE;
    uint32_t starting_point;
    mat img = read_header(is, starting_point);
    std::vector<std::array<uint8_t, 4>> color_table;
    read_pixels(is, img);
    std::ofstream os(argv[2], std::ios::binary);
    write_pam(os, img);
    return EXIT_SUCCESS;
}