#include <vector>
#include "hufstr.h"
#include "table_generator.cpp"

void read_table(std::vector<huffman<uint8_t>::elem>& table) {
    std::ifstream is("table.bin", std::ios::binary);
    table.resize(256);
    for (uint32_t i=0; i<256; ++i) {
        huffman<uint8_t>::elem e;
        e._sym = is.get();
        e._len = is.get();
        uint32_t code, n=0;
        is.read(reinterpret_cast<char*>(&code), 4);
        for (uint32_t k=0; k<e._len; ++k) {
            n = n << 1 | 1;
        }
        e._code = code & n;
        table[i] = e;
    }
}

std::vector<uint8_t> hufstr::compress(const std::string& s) const {
    std::vector<huffman<uint8_t>::elem> table;
    read_table(table);
    std::vector<uint8_t> codes;
    uint8_t e = 0, len = 0;
    uint64_t count = 0, code_length = 0;
    for (uint32_t i=0; i<s.size(); ++i) {
        for (uint32_t j=0; j<256; ++j) {
            if (table[j]._sym == s[i]) {
                code_length = table[j]._len;
                while (code_length > 0) {
                    if (len == 8) {
                        codes.push_back(e);
                        ++count;
                        e = len = 0;
                    }
                    uint8_t bit = (table[j]._code >> (code_length-1)) & 1;
                    e = (e << 1) | (bit);
                    ++len;
                    --code_length;
                }
            }
        }
    }
    while (len < 8) {
        e = (e << 1) | 1;
        ++len;
        ++code_length;
    }
    codes.push_back(e);
    return codes;
}

std::string hufstr::decompress(const std::vector<uint8_t>& v) const {
    std::string output = "";
    std::vector<huffman<uint8_t>::elem> table;
    read_table(table);
    uint32_t buffer = 0, count = 8, len = 0;
    for (uint32_t i=0; i<v.size(); ++i) {
        for (uint8_t k=0; k<8; ++k) {
            if (count == 0) count = 8;
            buffer = (buffer << 1) | ((v[i] >> (count-1))&1);
            --count;
            ++len;
            for (uint32_t j=0; j<256; ++j) {
                if (len == table[j]._len && buffer == table[j]._code) {
                    output += table[j]._sym;
                    len = 0;
                    buffer = 0;
                    break;
                }
            }
        }
    }
    return output;
}