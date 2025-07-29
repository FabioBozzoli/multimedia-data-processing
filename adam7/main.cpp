#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <utility>

struct mat {
    uint32_t rows_, cols_;
    std::vector<uint8_t> pix_;

    mat(uint32_t r, uint32_t c) : rows_(r), cols_(c) {}

    uint8_t& operator()(uint32_t r, uint32_t c) {
        return pix_[r*cols_+c];
    }
    const uint8_t operator()(uint32_t r, uint32_t c) const {
        return pix_[r*cols_+c];
    }

    void pix(const std::vector<uint8_t>& p) { pix_ = p; }

    void add_pixel(uint8_t p) {
        pix_.push_back(p);
    }

    void set_rows(const uint32_t& r) { rows_ = r; }
    void set_cols(const uint32_t& c) { cols_ = c; }

    void set_pix_zero() {
        for (uint32_t i=0; i<rows_; ++i) {
            for (uint32_t j=0; j<cols_; ++j) {
                pix_.push_back(0);
            }
        }
    }
};

mat mat_adam7(uint32_t r, uint32_t c) {
    std::vector<uint8_t> square{1,6,4,6,2,6,4,6,7,7,7,7,7,7,7,7,
    5,6,5,6,5,6,5,6,7,7,7,7,7,7,7,7,3,6,4,6,3,6,4,6,7,7,7,7,7,
    7,7,7,5,6,5,6,5,6,5,6,7,7,7,7,7,7,7,7};
    mat mask8(8,8);
    mask8.pix(square);
    mat mask(r,c);
    for (uint32_t i=0; i<r; ++i) {
        for (uint32_t j=0; j<c; ++j) {
            mask.add_pixel(mask8(i%8, j%8));
        }
    }
    return mask;
}

void read_pgm(std::istream& is, mat& img) {
    std::string magic_number(2, ' ');
    is >> magic_number; is.get();
    if (is.peek() == '#') {
        std::string line;
        std::getline(is, line);
    }
    uint32_t cols, rows, nlevels;
    is >> cols >> rows >> nlevels;
    std::cout << cols << " " << rows << " " << nlevels;
    is.get();
    img.set_rows(rows);
    img.set_cols(cols);
    while (is.peek() != EOF) {
        img.add_pixel(is.get());
    }
}

void write_MLT(std::ostream& os, const mat& img, const mat& mask) {
    os << "MULTIRES";
    os.write(reinterpret_cast<const char *>(&img.cols_), sizeof(uint32_t));
    os.write(reinterpret_cast<const char *>(&img.rows_), sizeof(uint32_t));
    for (uint8_t l=1; l<8; ++l) {
        for (uint32_t i=0; i<img.rows_; ++i) {
            for (uint32_t j=0; j<img.cols_; ++j) {
                if (mask(i,j) == l) {
                    os.put(img(i,j));
                }
            }
        }
    }

}

void read_mlt(std::istream& is, mat& img, uint8_t& level, mat& mask) {
    for (uint8_t i=0; i<8; ++i) is.get();
    uint32_t cols, rows;
    is.read(reinterpret_cast<char *>(&cols), sizeof(uint32_t));
    is.read(reinterpret_cast<char *>(&rows), sizeof(uint32_t));
    mask = mat_adam7(rows, cols);
    img.set_cols(cols); img.set_rows(rows);
    img.set_pix_zero();
    for (uint8_t l=1; l<8; ++l) {
        if (is.peek() == EOF) {
            level = l;
            break;
        }
        for (uint32_t i=0; i<rows; ++i) {
            for (uint32_t j=0; j<cols; ++j) {
                if (mask(i,j)==l)
                    img(i,j) = static_cast<uint8_t>(is.get());
            }
        }
    }
    level = 7;
}

void sub_mat(mat img, uint32_t r, uint32_t c, uint32_t dim_r, uint32_t dim_c,
    mat& m) {
    uint8_t pix = img(r,c);
    for (uint32_t i=r; i<img.rows_ && i<dim_r+r; ++i) {
        for (uint32_t j=c; j<img.cols_ && j<dim_c+c; ++j) {
            m(i,j) = pix;
        }
    }
}

void write_pgm_level_i(const std::string& filename, const mat& mask,
    const uint8_t level, mat& img) {
    std::unordered_map<uint8_t, std::pair<uint8_t, uint8_t>> sub_mat_map
    {{1,{8,8}},{2,{4, 8}}, {3,{4,4}}, {4,{2,4}}, {5,{2,2}},{6,{1,2}}, {7,{1,1}}};
    for (uint8_t l=1; l<=level; ++l) {
        std::string f = filename.substr(0, filename.find('.'))+"_"+std::to_string(l)+".pgm";
        std::ofstream os(f, std::ios::binary);
        os << "P5 " << img.cols_ << " " << img.rows_ << " 255\n";
        uint8_t offset_cols = sub_mat_map[l].first;
        uint8_t offset_rows = sub_mat_map[l].second;
        mat img_level_l(img.rows_, img.cols_);
        img_level_l.set_pix_zero();
        for (uint32_t i=0; i<img.rows_; i+=offset_rows) {
            for (uint32_t j=0; j<img.cols_; j+=offset_cols) {
                sub_mat(img, i, j, offset_rows, offset_cols, img_level_l);
            }
        }
        for (uint32_t i=0; i<img.rows_; ++i) {
            for (uint32_t j=0; j<img.cols_; ++j) {
                os.put(img_level_l(i,j));
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) return EXIT_FAILURE;

    std::ifstream is(argv[2], std::ios::binary);
    if (!is) return EXIT_FAILURE;
    if (argv[1][0]=='c') {
        std::ofstream os(argv[3], std::ios::binary);
        if (!os) return EXIT_FAILURE;
        mat img(0,0);
        read_pgm(is,img);
        mat mask = mat_adam7(img.rows_, img.cols_);
        write_MLT(os, img, mask);
    }
    else if (argv[1][0] == 'd') {
        mat img(0,0);
        uint8_t level = 0;
        mat mask(0,0);
        read_mlt(is, img, level, mask);
        write_pgm_level_i(argv[3], mask, level, img);
    }
    return EXIT_SUCCESS;
}