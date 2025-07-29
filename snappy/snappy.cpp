#include <iostream>
#include <fstream>
#include <vector>

struct bitreader {
    uint8_t buffer_;
    std::istream& is_;
    uint32_t n_;

    bitreader(std::istream& is) : buffer_(0), is_(is), n_(0) {}

    uint8_t read_bit() {
        if (n_==0) {
            is_.read(reinterpret_cast<char *>(&buffer_), 1);
            n_ = 8;
        }
        --n_;
        return (buffer_ >> n_) & 1;
    }

    uint32_t read_n_bits(int num) {
        uint32_t out;
        while (num-->0) {
            out = (out << 1) | read_bit();
        }
        return out;
    }
};

size_t preamble(std::istream& is) {
    uint8_t byte;
    size_t length = 0, count = 0;
    do {
        byte = is.get();
        length = length + ((byte & 0b01111111) << (7*count));
        ++count;
    } while (((byte >> 7) & 1) == 1);
    return length;
}

void decode(std::istream& is, std::vector<uint8_t>& data) {
    while (is.peek()!=EOF) {
        uint8_t byte = is.get();
        uint8_t type = byte & 0b00000011;
        uint64_t length=0, offset=0, new_length=0;
        switch (type){
            case 0:
                length = ((byte >> 2) & 0b00111111);
                new_length=length;
                if (length >= 60) {
                    is.read(reinterpret_cast<char*>(&new_length), (length-59));
                }
                length = new_length + 1;
                for (uint64_t i=0; i<length; ++i) {
                    data.push_back(is.get());
                }
                break;

            case 1:
                length = ((byte >> 2) & 0b00000111) + 4;
                offset = ((byte >> 5) & 0b00000111);
                offset = offset << 8 | is.get();
                for (uint64_t i=0; i<length; ++i) {
                    data.push_back(data[data.size()-offset]);
                }
                break;

            case 2:
                length = ((byte >> 2) & 0b00111111) + 1;
                is.read(reinterpret_cast<char*>(&offset), 2);
                for (uint64_t i=0; i<length; ++i) {
                    data.push_back(data[data.size()-offset]);
                }
                break;

            case 3:
                length = ((byte >> 2) & 0b00111111) + 1;
                is.read(reinterpret_cast<char*>(&offset), 4);
                for (uint64_t i=0; i<length; ++i) {
                    data.push_back(data[data.size()-offset]);
                }
                break;
        }
    }
}

bool write(const std::string& filename, const std::vector<uint8_t>& data) {
    std::ofstream os(filename, std::ios::binary);
    if (!os) return false;
    for (uint64_t i=0; i<data.size(); ++i) {
        os << data[i];
    }
    return true;
}

int main(int argc, char * argv[]) {
    if (argc!=3) return EXIT_FAILURE;

    std::ifstream is(argv[1], std::ios::binary);
    if (!is) return EXIT_FAILURE;

    size_t final_length = preamble(is);
    std::vector<uint8_t> data;
    decode(is, data);
    if (!write(argv[2], data)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}