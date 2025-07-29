#include <iostream>
#include <fstream>
#include <vector>

struct bitreader {
    uint8_t buffer_;
    uint32_t n_;
    std::istream& is_;

    bitreader(std::istream& is) : is_(is), buffer_(0), n_(0){}

    uint8_t read_bit() {
        if (n_==0) {
            is_.read(reinterpret_cast<char*>(&buffer_),1);
            n_=8;
        }
        --n_;
        uint8_t bit =( buffer_ >> n_) & 1;
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

void PDB_header(std::istream& is, std::string& output, uint32_t& number_record) {
    bitreader br(is);
    std::string name(32, ' ');
    is.read(reinterpret_cast<char *>(name.data()), 32);
    std::string dummy(4, ' ');
    is.read(reinterpret_cast<char *>(dummy.data()), 4);
    uint32_t creation_date = br.read_n_bits(4*8);
    std::string dummy2(20, ' ');
    is.read(reinterpret_cast<char *>(dummy2.data()), 20);
    std::string type(4, ' '), creator(4, ' ');
    is.read(reinterpret_cast<char *>(type.data()), 4);
    is.read(reinterpret_cast<char *>(creator.data()), 4);
    is.read(reinterpret_cast<char *>(dummy.data()), 4);
    is.read(reinterpret_cast<char *>(dummy.data()), 4);
    number_record = br.read_n_bits(16);
    output = "PDB name: " + name + "\nCreation date (s): " + std::to_string(creation_date) +
        "\nType: " + type + "\nCreator: " + creator + "\nRecords: " +
            std::to_string(number_record) + "\n\n";
}

void info_entries(std::istream& is, std::string& output, const uint32_t records, std::vector<uint32_t>& offsets) {
    bitreader br(is);
    for (uint32_t i=0; i<records; ++i) {
        uint32_t offset = br.read_n_bits(32);
        uint8_t stuff = br.read_n_bits(8);
        uint32_t id = br.read_n_bits(24);
        offsets.push_back(offset);
        output = output + std::to_string(i) + " - offset: " + std::to_string(offset) +
            " - id: " + std::to_string(id) + "\n";
    }
    output = output + "\n";
}

void palm_DOC(std::istream& is, std::string& output, uint32_t& records, uint32_t& length) {
    bitreader br(is);
    uint16_t compression = br.read_n_bits(16);
    br.read_n_bits(16);
    length = br.read_n_bits(32);
    records = br.read_n_bits(16);
    uint16_t record_size = br.read_n_bits(16);
    uint16_t encryption = br.read_n_bits(16);
    br.read_n_bits(16);
    output = "Compression: " + std::to_string(compression) + "\nTextLength: " +
        std::to_string(length) + "\nRecordCount: " + std::to_string(records) +
            "\nRecordSize: " + std::to_string(record_size) + "\nEncryptionType: " +
                std::to_string(encryption) + "\n\n";
}

void decode_records(std::istream& is, const std::vector<uint32_t>& offsets,
    uint32_t length, uint32_t records, std::ostream& os) {
    bitreader br(is);
    uint32_t c_length=0;
    std::vector<uint8_t> output;
    for (uint32_t i=1; i<records+1; ++i) {
        uint32_t count = 0;
        is.seekg(offsets[i]);
        while (count < 4096 && output.size() < length) {
            uint8_t byte = br.read_n_bits(8);
            if (byte == 0) break;
            else if (byte >= 1 and byte <= 8) {
                for (uint8_t j=0; j<byte && count < 4096 && c_length < length; ++j) {
                    output.push_back(is.get());
                    ++count;
                    ++c_length;
                }
            }
            else if (byte >= 9 and byte <= 0x7f) {
                output.push_back(byte);
                ++count;
                ++c_length;
            }
            else if (byte >= 0x80 and byte <= 0xbf) {
                uint16_t b = (byte << 8) | is.get();
                uint16_t distance = (b >> 3) & 0b0011111111111;
                uint8_t l = (b & 0b111) + 3;
                for (uint32_t k=0; k<l && count < 4096 && c_length < length; ++k) {
                    output.push_back(output[c_length-distance]);
                    ++count;
                    ++c_length;
                }
            }
            else{
                output.push_back(' ');
                ++count;
                ++c_length;
                if (count < 4096 and c_length < length) {
                    output.push_back(byte & 0b01111111);
                    ++count;
                    ++c_length;
                }
            }
        }
    }
    for (uint32_t i=0; i<output.size(); ++i) {
        os.write(reinterpret_cast<char*>(&output[i]), 1);
    }
}

int main(int argc, char * argv[]) {
    if (argc!=3) return EXIT_FAILURE;
    std::ifstream is(argv[1], std::ios::binary);
    if (!is) return EXIT_FAILURE;
    std::ofstream os(argv[2], std::ios::binary);
    if (!os) return EXIT_FAILURE;

    uint32_t BOM = 0xBFBBEF;
    os.write(reinterpret_cast<const char *>(&BOM), 3);

    std::string output, info, palm;
    uint32_t records, length;
    std::vector<uint32_t> offsets;
    PDB_header(is, output, records);
    std::cout << output;
    info_entries(is, info, records, offsets);
    std::cout << info;
    is.seekg(offsets[0]);
    palm_DOC(is, palm, records, length);
    std::cout << palm;
    decode_records(is, offsets, length, records, os);
    return 0;
}