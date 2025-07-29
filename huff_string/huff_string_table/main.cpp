#include "hufstr.h"
#include "table_generator.cpp"
#include <fstream>
#include <vector>

int main()
{
    /*
    std::ifstream is("bibbia.txt", std::ios::binary);
    if (!is) {
        return EXIT_FAILURE;
    }

    // Calcolo le frequenze dei byte nel file - Compute byte frequencies in the provided file
    frequency<uint8_t> f;
    f = std::for_each(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), f);
    // Aggiungo 1 a ogni possibile byte per essere sicuro di avere un codice di Huffman per ogni possibile simbolo
    // Add 1 to every byte ensuring that every symbol gets a Huffman code
    for (int i = 0; i < 256; ++i)
        f(i);
    // Calcolo i codici canonici di Huffman - Compute canonical Huffman codes
    huffman<uint8_t> huff(f);

    // Qui bisogna salvare la tabella di Huffman nel file table.bin
    // Here you need to save the Huffman table onto table.bin
    std::ofstream os("table.bin", std::ios::binary);
    std::vector<huffman<uint8_t>::elem> table = huff.table();
    for (uint32_t i=0; i<table.size(); ++i) {
        os.write(reinterpret_cast<char*>(&table[i]._sym), 1);
        os.write(reinterpret_cast<char*>(&table[i]._len), 1);
        os.write(reinterpret_cast<char*>(&table[i]._code), 4);
    }
    // Una chiamata a huff.table() restituisce un std::vector<elem> che è la
    // tabella dei codici di Huffman. Ogni elem ha un uint8_t _sym (il byte/carattere),
    // un uint8_t _len (la lunghezza del codice) e un uint32_t _code. I _len
    // bit meno significativi di _code sono il codice di Huffman.
    // A call to huff.table() returns a std::vector<elem> which is the
    // Huffman table. Each elem has a uint8_t _sym (the byte/character),
    // a uint8_t _len (the code length), and a uint32_t _code. The _len
    // least significant bits of _code are the Huffman code.
    */

    hufstr h;
    std::vector<uint8_t> v = h.compress("aeio");
    for (uint32_t i=0; i<v.size(); ++i) {
        std::cout << std::to_string(v[i]) << " ";
    }

    std::string out = h.decompress(v);
    std::cout << out;
    return EXIT_SUCCESS;
}