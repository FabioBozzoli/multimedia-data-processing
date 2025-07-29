Of course, here is a description for your README.md file, written in English and without using bold formatting, for the two new programs.

---

# C++ Huffman Coding and Image Compression

This repository contains two C++ programs that demonstrate data compression techniques. The first program, `prova.cpp`, is a general-purpose implementation of the Huffman coding algorithm. The second, `huffdiff.cpp`, applies Huffman coding along with a differential encoding scheme to compress and decompress images in the PAM (Portable Arbitrary Map) format.

## `prova.cpp`: Huffman Coding Implementation

This program provides the core logic for building a Huffman tree and generating a coding table from a given text.

### Features

- `bitreader` and `bitwriter`: Utility classes for reading and writing data at the bit level, essential for handling compressed data streams.
- `node` Structure: Defines the nodes of the Huffman tree, storing symbol frequencies and links to child nodes.
- Frequency Analysis: Calculates the frequency of each symbol in the input data.
- Huffman Tree Construction: Builds the Huffman tree by iteratively combining nodes with the lowest frequencies.
- Code Generation: Traverses the Huffman tree to assign a unique binary code to each symbol based on its path from the root. The length of the code is inversely proportional to the symbol's frequency.

### Workflow

1.  Calculates symbol frequencies from the input.
2.  Builds a priority queue of nodes, ordered by frequency.
3.  Constructs the Huffman tree by merging nodes.
4.  Recursively traverses the tree to determine the length of each symbol's code.
5.  Generates the final coding table.

## `huffdiff.cpp`: Image Compression with Huffman and Differential Encoding

This program implements a complete compression and decompression pipeline for grayscale images, inspired by techniques used in formats like PNG.

### Features

- PAM Image Support: Includes functions to read and write grayscale images in the PAM (`P7`) format.
- `mat` Class: A simple matrix class to store image data efficiently.
- Differential Encoding: Before compression, the image is transformed by storing the difference between adjacent pixel values. This "difference image" typically has lower entropy and is more compressible.
- Huffman Compression: The difference image is then compressed using the Huffman coding algorithm.
- Decompression: The process is reversed to decompress the file. The Huffman-coded data is decoded to reconstruct the difference image, and then the original image is restored from the differences.

### Usage

The program can be used for both compression and decompression via command-line arguments.

To compress an image:

```bash
g++ huffdiff.cpp -o huffdiff_compress
./huffdiff_compress c input_image.pam compressed_output.huff
```

To decompress an image:

```bash
g++ huffdiff.cpp -o huffdiff_decompress
./huffdiff_decompress d compressed_output.huff decompressed_image.pam
```
