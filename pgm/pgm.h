#ifndef PGM_H
#define PGM_H
#include "mat.h"

enum class pgm_mode {plain = 2, binary = 5};

void write(const std::string& filename, pgm_mode mode, const mat<uint8_t>& m);

mat<uint8_t> read(const std::string& filename);

#endif //PGM_H
