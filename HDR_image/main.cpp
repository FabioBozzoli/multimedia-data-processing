#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

template<typename T>
class mat {
    uint32_t rows_, cols_;
    std::vector<std::vector<T>> pixels_;

public:
    mat(const uint32_t r, const uint32_t c) : rows_(r), cols_(c) {}

    void set_rows(const uint32_t r) { rows_ = r; }
    void set_cols(const uint32_t c) { cols_ = c; }

    const uint32_t get_rows() const { return rows_; }
    const uint32_t get_cols() const { return cols_; }

    void add_pixel_float(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t e) {
        float r_float = (static_cast<float>(r) + 0.5)/256.0 * static_cast<float>(pow(2, (e-128)));
        float g_float = (static_cast<float>(g) + 0.5)/256.0 * static_cast<float>(pow(2, (e-128)));
        float b_float = (static_cast<float>(b) + 0.5)/256.0 * static_cast<float>(pow(2, (e-128)));
        pixels_.push_back({r_float,g_float,b_float});
    }

    void add_pixel_integer(const uint8_t r, const uint8_t g, const uint8_t b) {
        pixels_.push_back({r,g,b});
    }

    std::vector<T> operator()(const uint32_t r, const uint32_t c){ return pixels_[r*cols_ + c]; }
    std::vector<T> operator()(const uint32_t r, const uint32_t c) const { return pixels_[r*cols_ + c]; }
    std::vector<T> operator[](const uint32_t pos){ return pixels_[pos]; }
    std::vector<T> operator[](const uint32_t pos) const { return pixels_[pos]; }

    //const auto raw_data() const { return pixels_.data(); }
    //auto raw_data() { return pixels_.data(); }

    const auto size() const { return pixels_.size(); }
    auto size() { return pixels_.size(); }
    //const auto raw_size() const { return pixels_.size()*3; }
    //auto raw_size() { return pixels_.size()*3; }
};

mat<float> read_header(std::istream& is) {
    mat<float> img(0,0);
    std::string header(10, ' ');
    std::getline(is, header);
    if (header == "#?RADIANCE") {
        while (is.peek() != '\n') {
            std::string s;
            std::getline(is, s);
        }
        is.get(); is.get(); is.get();
        std::string s;
        uint32_t rows, cols;
        is >> rows;
        is.get(); is.get(); is.get();
        is >> cols;
        is.get();
        img.set_rows(rows);
        img.set_cols(cols);
    }
    return img;
}

void RLE(std::istream& is, mat<float>& img, std::vector<uint8_t>& v) {
    uint32_t counter=0;
    uint8_t L=0;
    while (counter != img.get_cols()) {
        L = is.get();
        if (L <= 127) {
            for (uint8_t i=0; i<L; ++i) {
                v.push_back(is.get());
                ++counter;
            }
        }
        else {
            uint8_t byte = is.get();
            for (uint8_t i=0; i<L-128; ++i) {
                v.push_back(byte);
                ++counter;
            }
        }
    }
}

void read_line(std::istream& is, mat<float>& img) {
    is.get(); is.get(); is.get(); is.get();
    std::vector<uint8_t> reds, greens, blues, e;
    RLE(is, img, reds);
    RLE(is, img, greens);
    RLE(is, img, blues);
    RLE(is, img, e);
    for (uint32_t i=0; i<img.get_cols(); ++i) {
        img.add_pixel_float(reds[i], greens[i], blues[i], e[i]);
    }
}

float find_max(mat<float>& img) {
    float max = -1;
    for (uint32_t i=0; i<img.get_cols()*img.get_rows(); ++i) {
        if (max < img[i][0]) max = img[i][0];
        if (max < img[i][1]) max = img[i][1];
        if (max < img[i][2]) max = img[i][2];
    }
    return max;
}

float find_min(mat<float>& img) {
    float min = 255;
    for (uint32_t i=0; i<img.get_cols()*img.get_rows(); ++i) {
        if (min > img[i][0]) min = img[i][0];
        if (min > img[i][1]) min = img[i][1];
        if (min > img[i][2]) min = img[i][2];
    }
    return min;
}

mat<uint8_t> read_pixels(std::istream& is, mat<float>& img) {
    mat<uint8_t> img_int(img.get_rows(), img.get_cols());
    while (is.peek() != EOF) {
        read_line(is, img);
    }
    float max = find_max(img);
    float min = find_min(img);
    for (uint32_t i=0; i<img.get_cols()*img.get_rows(); ++i) {
        uint8_t r = static_cast<uint8_t>(255*pow(((img[i][0]-min)/(max-min)),(0.45)));
        uint8_t g = static_cast<uint8_t>(255*pow(((img[i][1]-min)/(max-min)),(0.45)));
        uint8_t b = static_cast<uint8_t>(255*pow(((img[i][2]-min)/(max-min)),(0.45)));
        img_int.add_pixel_integer(r,g,b);
    }
    return img_int;
}

void write_pam(std::ostream& os, mat<uint8_t>& img) {
    os << "P7\nWIDTH " << img.get_cols() << "\nHEIGHT " << img.get_rows()
    << "\nDEPTH 3\nMAXVAL 255\nTUPLTYPE RGB\nENDHDR\n";
    for (size_t i=0; i<img.size(); ++i) {
        os.put(img[i][0]);
        os.put(img[i][1]);
        os.put(img[i][2]);
    }
}

int main(int argc, char* argv[]) {
    if (argc!=3) return EXIT_FAILURE;

    std::ifstream is(argv[1], std::ios::binary);
    if (!is) return EXIT_FAILURE;

    mat img = read_header(is);
    if (img.get_cols()==0 | img.get_rows() == 0) return EXIT_FAILURE;
    mat<uint8_t> img_int = read_pixels(is, img);
    std::ofstream os(argv[2], std::ios::binary);
    if (!os) return EXIT_FAILURE;
    write_pam(os, img_int);
    return EXIT_SUCCESS;
}