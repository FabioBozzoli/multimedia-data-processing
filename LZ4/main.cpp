#include <iostream>
#include <fstream>
#include <string>
bool read_header(std::istream& is, uint32_t& unc_file_size) {
    uint32_t magic_number = 0, cost = 0;
    for (uint8_t i=0; i<4; ++i) {
        magic_number = magic_number << 8 | is.get();
    }
    if (magic_number != 0x03214C18) { return false; }
    is.read(reinterpret_cast<char*>(&unc_file_size), 4);
    for (uint8_t i=0; i<4; ++i) {
        cost = cost << 8 | is.get();
    }
    if (cost != 0x0000004D) { return false; }
    return true;
}

bool read_stream(std::istream& is, std::string& output) {
    size_t pos = 0;
    uint32_t n_blocks=0;
    while (is.peek() != EOF) {
        uint32_t block_length = 0;
        ++n_blocks;
        is.read(reinterpret_cast<char*>(&block_length), 4);
        block_length += pos;
        for (uint32_t k=0; k<block_length;) {
            uint8_t token = is.get();
            ++k;
            uint32_t literal_length = token >> 4;
            uint32_t counter=0;
            while (literal_length == 255*counter+15) {
                literal_length += is.get();
                ++counter;
                ++k;
            }

            std::string literals(literal_length, ' ');
            for (int i=0; i<literal_length; ++i) {
                literals[i] = static_cast<char>(is.get());
                ++k;
            }
            for (uint32_t i=pos; i<literal_length+pos; ++i) {
                output[i] = literals[i-pos];
            }
            pos+=literal_length;
            if (k<block_length-1 ) {
                uint16_t offset;
                is.read(reinterpret_cast<char*>(&offset), 2);
                k+=2;
                uint32_t match_length = token & 0x0F;
                counter=0;
                while (match_length == 255*counter+15) {
                    match_length += is.get();
                    ++counter;
                    ++k;
                }
                match_length += 4;

                if (is.peek()==EOF) { return true; }
                for (uint32_t i=pos; i<match_length+pos; ++i) {
                    output[i] = output[i-offset];
                }
                pos+=match_length;
            }

        }

    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc!=3) { return EXIT_FAILURE; }
    std::ifstream is(argv[1], std::ios::binary);
    if (!is) { return EXIT_FAILURE; }
    uint32_t unc_file_size;
    if (!read_header(is, unc_file_size)) { return EXIT_FAILURE; }
    std::string output(unc_file_size, ' ');
    read_stream(is, output);
    std::ofstream os(argv[2]);
    if (!os) { return EXIT_FAILURE; }
    os << output;
    return EXIT_SUCCESS;
}