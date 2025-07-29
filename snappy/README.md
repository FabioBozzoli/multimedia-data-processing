## `data_compression` Module: Snappy-like Decompression Tool

This module implements a C++ program for **data decompression** from a compressed stream, utilizing a custom decoding algorithm. Its focus is the efficient reconstruction of original data through specific functionalities for bit reading and handling of the compressed file's preamble.

### Key Features

The `snappy.cpp` program relies on several key components to perform decompression:

- bitreader Class: This class manages bit-level reading from an input stream (`std::istream`). It provides methods to read individual bits (`read_bit()`) or a specified number of bits (`read_n_bits(int num)`).
- preamble Function: This function is responsible for extracting compressed data length information from the input stream's preamble. This allows the program to know the final size of the data to be decompressed.
- decode Function: This is the core function responsible for reconstructing the original data from the compressed input stream. It reads byte by byte from the stream, identifies the type of compressed block (indicated by the 2 least significant bits of the byte), and, based on the type, reconstructs the data into the `data` vector. Four types of blocks (cases 0, 1, 2, 3) are handled, each with different logic for managing length and offset for data reconstruction.
- main Function: Controls the overall program flow. It handles opening and closing input/output files, calls the `preamble` function to determine the final length, the `decode` function to perform decompression, and finally writes the reconstructed data to an output file.

### Usage

The program expects two command-line arguments: the path to the input compressed file and the path to the file where the decompressed data will be written.

./snappy_tool input_compressed_file output_decompressed_file

