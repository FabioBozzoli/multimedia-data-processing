#ifndef PCX_H
#define PCX_H

#include "mat.h"
#include "pcx.cpp"

bool load_pcx(const std::string& filename, mat<unsigned char>& img);

#endif //PCX_H
