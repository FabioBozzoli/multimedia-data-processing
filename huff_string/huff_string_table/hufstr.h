#include <vector>
#ifndef HUFSTR_H
#define HUFSTR_H

class hufstr {
public:
    std::vector<uint8_t> compress(const std::string& s) const;
    std::string decompress(const std::vector<uint8_t>& v) const;
};

#endif //HUFSTR_H