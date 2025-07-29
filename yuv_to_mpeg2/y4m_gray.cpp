#include "mat.h"
#include "pgm.h"
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>

std::unordered_map<char, int> read_header(std::istream& is) {
    char c;
    std::unordered_map<char, int> map;
    while (is.get(c) && c!='\n') {
        if (c=='W') {
            uint64_t w;
            is >> w;
            map['W'] = w;
        }
        if (c=='H') {
            uint64_t h;
            is >> h;
            map['H'] = h;
        }
    }
    is.get(c);
    while (is.get(c) && c!='\n') {
        ;
    }
    return map;
}

bool y4m_extract_gray(const std::string& filename, std::vector<mat<uint8_t>>& frames) {
    std::ifstream is(filename, std::ios::binary);
    if (!is){ return false; }
    std::unordered_map<char, int> dim = read_header(is);
    mat<uint8_t> m(dim['H'], dim['W']);
    char c = is.peek();
    while (c!=EOF) {
        for (int i=0; i<dim['H']; ++i) {
            for (int j=0; j<dim['W']; ++j) {
                is.get(c);
                m(i,j) = static_cast<uint8_t>(c);
            }
        }
        for (int i=0; i<dim['H']/2; ++i) {
            for (int j=0; j<dim['W']/2; ++j) {
                is.get(c);
            }
        }
        for (int i=0; i<dim['H']/2; ++i) {
            for (int j=0; j<dim['W']/2; ++j) {
                is.get(c);
            }
        }
        while (is.get(c) && c!='\n') {
            ;
        }
        frames.push_back(m);
        c = is.peek();
    }
    return true;
}

/*
int main(int argc, char* argv[]) {
    std::vector<mat<uint8_t>> frames;
    y4m_extract_gray(argv[1], frames);
    std::string s;
    for (char i=0; i<16; ++i) {
        char sd = 'a'+i;
        s+=sd;
        save_pgm(s+".pgm", frames[i], true);
    }
    std::cout << frames.size();
    return 0;
}
*/