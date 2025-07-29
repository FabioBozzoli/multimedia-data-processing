#include <iostream>
#include <fstream>
#include "pcx.h"
#include "mat.h"

int main() {
    mat<uint8_t> img(0,0);
    load_pcx("bunny.pcx", img);
    return 0;
}