#include <iostream>
#include <unordered_map>
#include <vector>
#include <array>
#include <fstream>
#include <memory>

struct bitreader {
    uint8_t buffer_;
    uint32_t n_;
    std::istream& is_;

    bitreader(std::istream& is) : is_(is), buffer_(0), n_(0) {}

    uint8_t read_bit() {
        if (n_ == 0) {
            is_.read(reinterpret_cast<char*>(&buffer_), 1);
            n_ = 8;
        }
        --n_;
        uint8_t bit = (buffer_ >> n_) & 1;
        return bit;
    }

    uint32_t read_n_bits(int n) {
        uint32_t out = 0;
        while (n-->0) {
            out = (out << 1) | read_bit();
        }
        return out;
    }
};

struct bitwriter {
    uint8_t buffer_;
    uint32_t n_;
    std::ostream& os_;

    bitwriter(std::ostream& os) : os_(os), buffer_(0), n_(0) {}

    void write_bit(const uint32_t num) {
        if (n_ == 8) {
            os_.write(reinterpret_cast<char*>(&buffer_), 1);
            n_ = 0;
            buffer_ = 0;
        }
        ++n_;
        buffer_ = (buffer_ << 1) | (num & 1);
    }

    void write_n_bits(uint32_t val, int n) {
        while (n-->0) {
            write_bit((val >> n) & 1);
        }
    }

    ~bitwriter() {
        if (n_ > 0) {
            buffer_ <<= (8 - n_);
            os_.write(reinterpret_cast<char*>(&buffer_), 1);
        }
    }
};

struct node {
    int16_t sym_;
    uint32_t freq_, len_, code_;
    node* l_;
    node* r_;

    node(int16_t sym, uint32_t freq) : sym_(sym), freq_(freq) {}
    node(node* l, node* r) : l_(l), r_(r), freq_(l->freq_+r->freq_) {}
};

void recursive_length(node* n, uint32_t len, std::vector<node*>& leaves) {
    if (n->l_ != nullptr) recursive_length(n->l_, len+1, leaves);
    if (n->r_ != nullptr) recursive_length(n->r_, len+1, leaves);

    if (n->l_ == nullptr && n->r_ == nullptr) {
        n->len_ = len;
        leaves.push_back(n);
    }
}

std::vector<std::pair<int32_t, int32_t>> create_table(std::vector<int16_t>& img) {
    std::unordered_map<int16_t, uint32_t> f(512);
    for (uint32_t i=0; i<img.size(); ++i) {
        ++f[img[i]+255];
    }

    std::vector<node*> nodes;
    std::vector<std::unique_ptr<node>> mem;

    for (auto [sym, freq] : f) {
        mem.push_back(std::make_unique<node>(sym-255, freq));
        nodes.push_back(mem.back().get());
    }

    std::sort(nodes.begin(), nodes.end(), [](node* n1, node* n2) {
        return n1->freq_ > n2->freq_;
    });

    while (nodes.size()!=1) {
        node* n1 = nodes.back();
        nodes.pop_back();
        node* n2 = nodes.back();
        nodes.pop_back();
        mem.push_back(std::make_unique<node>(n1, n2));
        auto n = mem.back().get();

        auto it = std::lower_bound(nodes.begin(), nodes.end(), n,
            [](node* n1, node* n2) {
                return n1->freq_ > n2->freq_;
            });

        nodes.insert(it, n);
    }

    std::vector<node*> leaves;
    recursive_length(nodes[0], 0, leaves);

    std::sort(leaves.begin(), leaves.end(), [](node* n1, node* n2) {
        return n1->len_ < n2->len_;
    });

    std::vector<std::pair<int32_t, int32_t>> res;
    for (auto n : leaves) {
        std::pair<int32_t, int32_t> p;
        p.first = n->sym_;
        p.second = n->len_;
        res.push_back(p);
    }
    return res;
}

std::vector<std::array<int32_t, 3>> get_table_code(std::vector<std::pair<int32_t, int32_t>>& leaves) {
    std::vector<std::array<int32_t, 3>> res(leaves.size());
    res[0] = {leaves[0].first, leaves[0].second, 0};
    uint32_t prev_len = leaves[0].second;
    int32_t code = 0;
    for (uint32_t i=1; i<leaves.size(); ++i) {
        ++code;

        if (leaves[i].second > prev_len) {
            code = code << (leaves[i].second - prev_len);
        }

        res[i] = {leaves[i].first, leaves[i].second, code};
        prev_len = leaves[i].second;
    }
    return res;
}

template<typename T>
class mat {
    uint32_t rows_, cols_;
    std::vector<T> data_;

public:
    mat(uint32_t r, uint32_t c) : rows_(r), cols_(c), data_(r*c) {}

    void resize(uint32_t r, uint32_t c) {
        rows_ = r;
        cols_ = c;
        data_.resize(r*c);
    }

    uint32_t size() { return data_.size(); }
    const uint32_t size() const { return data_.size(); }

    uint32_t getRows() { return rows_; }
    uint32_t getCols() { return cols_; }
    const uint32_t getRows() const { return rows_; }
    const uint32_t getCols() const { return cols_; }

    T& operator[](uint32_t pos) { return data_[pos]; }
    const T& operator[](uint32_t pos) const { return data_[pos]; }

    T& operator()(uint32_t r, uint32_t c) { return data_[r*cols_+c]; }
    const T& operator()(uint32_t r, uint32_t c) const { return data_[r*cols_+c]; }

    std::vector<T>& getData() { return data_; }
    const std::vector<T>& getData() const { return data_; }

    auto rawdata(){ return data_.data(); }
    const auto rawdata() const{ return data_.data(); }

    auto rawsize() { return data_.size()*sizeof(T); }
    const auto rawsize() const { return data_.size()*sizeof(T); }
};

bool read_pam(std::istream& is, mat<uint8_t>& img) {
    std::string magic(2, ' ');
    is.read(reinterpret_cast<char*>(&magic), 2);
    if (magic != "P7") return false;
    is.get();
    uint32_t rows, cols;
    std::string dummy;
    is >> dummy;
    is.get();
    is >> cols >> dummy >> rows >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;
    is.get();
    img.resize(rows, cols);
    uint32_t count = 0;
    while (is.peek() != EOF) {
        img[count] = is.get();
        ++count;
    }
    return true;
}

void difference(const mat<uint8_t>& img, mat<int16_t>& diff) {
    diff.resize(img.getRows(), img.getCols());
    for (uint32_t r=0; r<img.getRows(); ++r) {
        for (uint32_t c=0; c<img.getCols(); ++c) {
            if (r==0 && c==0) diff(r,c) = img(r,c);
            else if (c==0) diff(r,c) = img(r,c) - img(r-1,c);
            else diff(r,c) = img(r,c) - img(r,c-1);
        }
    }
}

void compress(const mat<uint8_t>& img, mat<int16_t>& diff, std::ostream& os) {
    difference(img, diff);
    std::vector<std::pair<int32_t, int32_t>> leaves = create_table(diff.getData());
    std::vector<std::array<int32_t, 3>> table = get_table_code(leaves);
    uint32_t width = img.getCols(), height = img.getRows(), len_table = table.size();
    std::string s = "HUFFDIFF";
    os.write(reinterpret_cast<char*>(s.data()), 8);
    os.write(reinterpret_cast<char*>(&width), 4);
    os.write(reinterpret_cast<char*>(&height), 4);
    bitwriter bw(os);
    bw.write_n_bits(len_table, 9);
    for (uint32_t i=0; i<len_table; ++i) {
        bw.write_n_bits(table[i][0]+255, 9);
        bw.write_n_bits(table[i][1], 5);
    }
    for (uint32_t i=0; i<diff.size(); ++i) {
        for (uint32_t k=0; k<len_table; ++k) {
            if (diff[i] == table[k][0]) {
                bw.write_n_bits(table[k][2], table[k][1]);
                break;
            }
        }
    }
}

void write_pam(std::ostream& os, const mat<uint8_t>& img) {
    os << "P7\nWIDTH " << std::to_string(img.getCols()) <<
        "\nHEIGHT " << std::to_string(img.getRows()) <<
            "\nDEPTH 1\nMAXVAL 255\nTUPLTYPE GRAYSCALE\nENDHDR\n";
    for (size_t i=0; i<img.size(); ++i) {
        uint8_t c = img[i];
        os.write(reinterpret_cast<const char*>(&c), 1);
    }
}

void decompress(std::istream& is, mat<uint8_t>& img) {
    mat<int16_t> diff(0,0);
    std::string magic(8, ' ');
    is.read(reinterpret_cast<char*>(&magic), 8);
    uint32_t width, height;
    is.read(reinterpret_cast<char*>(&width), 4);
    is.read(reinterpret_cast<char*>(&height), 4);
    diff.resize(height, width);
    bitreader br(is);
    uint16_t len_table = br.read_n_bits(9);
    std::vector<std::pair<int32_t, int32_t>> leaves;
    for (uint32_t i=0; i<len_table; ++i) {
        int32_t sym = br.read_n_bits(9) - 255;
        int32_t len = br.read_n_bits(5);
        leaves.push_back({sym, len});
    }
    auto table = get_table_code(leaves);
    size_t code = 0, len = 0, count=0;
    while (count != width*height) {
        code = (code << 1) | br.read_bit();
        ++len;
        for (uint32_t i=0; i<len_table; ++i) {
            if (table[i][2] == code && table[i][1] == len) {
                len = 0;
                code = 0;
                diff[count] = table[i][0];
                ++count;
                break;
            }
        }
    }
    img.resize(diff.getRows(), diff.getCols());
    for (uint32_t r=0; r<diff.getRows(); ++r) {
        for (uint32_t c=0; c<diff.getCols(); ++c) {
            if (r==0 && c==0) img(r,c) = diff(r,c);
            else if (c==0) img(r,c) = diff(r,c) + img(r-1,c);
            else img(r,c) = diff(r,c) + img(r,c-1);
        }
    }
}

int main(int argc, char * argv[]) {
    if (argc != 4) return EXIT_FAILURE;
    std::string infile = argv[2];
    std::string outfile = argv[3];
    std::ifstream is(infile, std::ios::binary);
    if (!is) return EXIT_FAILURE;
    std::ofstream os(outfile, std::ios::binary);
    if (!os) return EXIT_FAILURE;
    if (argv[1][0] == 'c') {
        mat<uint8_t> img(0,0);
        mat<int16_t> diff(0,0);
        if (!read_pam(is, img)) return EXIT_FAILURE;
        compress(img, diff, os);
    }
    else if (argv[1][0] == 'd') {
        mat<uint8_t> img(0,0);
        decompress(is, img);
        write_pam(os, img);
    }

    return 0;
}