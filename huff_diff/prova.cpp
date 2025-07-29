#include <fstream>
#include <vector>
#include <array>
#include <unordered_map>

struct bitreader {
    uint8_t buffer_;
    uint32_t n_;
    std::istream& is_;

    bitreader(std::istream& is) : is_(is), buffer_(0), n_(0) {}

    uint8_t read_bit() {
        if (n_==0) {
            is_.read(reinterpret_cast<char*>(&buffer_), 1);
            n_=8;
        }
        --n_;
        uint8_t bit = (buffer_ >> n_) & 1;
        return bit;
    }

    uint32_t read_n_bits(int n) {
        uint32_t res = 0;
        while (n-->0) {
            res = (res << 1) | read_bit();
        }
        return res;
    }
};

struct bitwriter {
    uint8_t buffer_;
    uint32_t n_;
    std::ostream& os_;

    bitwriter(std::ostream& os) : os_(os), buffer_(0), n_(0) {}

    void write_bit(const uint32_t val) {
        if (n_==8) {
            os_.write(reinterpret_cast<char*>(&buffer_), 1);
            n_ = 0;
            buffer_ = 0;
        }
        ++n_;
        buffer_ = (buffer_ << 1) | (val & 1);
    }

    void write_n_bits(int n, uint3_t val) {
        while (n-->0) {
            write_bit((val >> n) & 1);
        }
    }

    ~bitwriter() {
        while (n_>0) {
            write_bit(0);
        }
    }
};

struct node {
    uint8_t sym_;
    uint32_t len_, code_, freq_;
    node* l_;
    node* r_;

    node(uint8_t sym, uint32_t freq) : sym_(sym), freq_(freq) {}
    node(node* l, node* r) : l_(l), r_(r), freq_(l->freq_ + r_->freq_) {}
};

void recursive_length(node* n, uint32_t len, std::vector<node*> leaves) {
    if (n->l_ != nullptr) recursive_length(n->l_, len+1, leaves);
    if (n->r_ != nullptr) recursive_length(n->r_, len+1, leaves);

    if (n->l_ == nullptr && n->r_ == nullptr) {
        n->len_ = len;
        leaves.push_back(n);
    }
}

std::vector<std::pair<uint32_t, uint32_t>> create_table(std::vector<uint32_t>& text) {
    std::unordered_map<uint8_t, uint32_t> f(256);
    for (const auto c:text) {
        ++f[c];
    }

    std::vector<node*> nodes;
    std::vector<std::unique_ptr<node>> mem;

    for (auto [sym, freq] : f) {
        mem.push_back(std::make_unique<node>(sym, freq));
        auto n = mem.back().get();
        nodes.push_back(n);
    }

    std::sort(nodes.begin(), nodes.end(), [](node* n1, node* n2) {
        return n1->freq_ > n2->freq_;
    });

    while (nodes.size() > 1) {
        node* n1 = nodes.back().get();
        nodes.pop_back();
        node* n2 = nodes.back().get();
        nodes.pop_back();

        mem.push_back(std::make_unique<node>(n1,n2));
        auto new_node = mem.back().get();

        auto it = std::lower_bound(nodes.begin(), nodes.end(), new_node,
            [](node* n1, node* n2) {
                return n1->freq_ > n2->freq_;
            });

        nodes.insert(it, new_node);
    }

    std::vector<node*> leaves;
    recursive_length(nodes[0], 0, leaves);

    std::sort(leaves.begin(), leaves.end(), [](node* n1, node* n2) {
        return n1->len_ < n2->len_;
    });

    std::vector<std::pair<uint32_t, uint32_t>> res;
    for (auto n : leaves) {
        std::pair<uint32_t, uint32_t> p;
        p.first = n->sym_;
        p.second = n->len_;
        res.push_back(p);
    }
    return res;
}

std::vector<std::array<uint32_t, 3>> get_table_code(std::vector<std::pair<uint32_t, uint32_t>> leaves) {
    std::vector<std::array<uint32_t, 3>> res;
    res[0] = {leaves[0].first, leaves[0].second, 0};
    uint32_t prev_len = leaves[0].second;

    uint32_t code = 0;
    for (uin32_t i=0; i<leaves.size(); ++i) {
        ++code;

        if (leaves[i].second > prev_len)
            code = code << (leaves[i].second - prev_len);

        res[i] = {leaves[i].first, leaves[i].second, code};
        prev_len = leaves[i].second;
    }

    return res;
}
