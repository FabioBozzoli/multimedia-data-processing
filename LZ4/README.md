# C++ LZ4 Compression Decoder

This repository contains a C++ program for decoding files compressed with the LZ4 algorithm. This implementation focuses on the core decompression logic, including parsing LZ4-specific block structures, handling literals and match sequences, and reconstructing the original data.

## Overview

LZ4 is a fast lossless compression algorithm. This program provides a practical example of how to implement an LZ4 decoder in C++, demonstrating the low-level byte manipulation and algorithmic steps required to decompress LZ4 data streams.

## LZ4 Format Specifications (as implemented in this code)

The LZ4 format, as processed by this decoder, appears to follow a structure typical of LZ4 streams, characterized by tokens, literal data, and match references.

-   **File Header Validation:**
    -   The decoder expects a specific 4-byte magic number `0x03214C18` at the beginning of the file to identify it as a valid LZ4 compressed stream.
    -   It also checks for a constant value `0x0000004D` after the file size, which might indicate a format version or additional metadata.
-   **Block Structure:**
    -   The compressed data is processed in blocks. Each block is prefixed by a 4-byte `block_length` field.
    -   The decoder processes data within each block until the `block_length` is exhausted.
-   **Tokens:**
    -   Within each block, data is structured using 1-byte "tokens."
    -   The token's upper 4 bits (`token >> 4`) indicate the `literal_length`.
    -   The token's lower 4 bits (`token & 0x0F`) indicate the `match_length`.
-   **Literal Length Encoding:**
    -   If the `literal_length` (from the token's upper nibble) is `0xF` (15), it signifies that additional bytes follow to extend the literal length.
    -   Subsequent bytes are added to the literal length. If an additional byte is `0xFF` (255), another byte follows, and so on. This mechanism allows for arbitrarily long literal sequences.
-   **Literal Data:**
    -   Following the token and any additional bytes for literal length, the actual `literal_length` bytes of raw, uncompressed data (literals) are present and are appended directly to the output.
-   **Match Length Encoding:**
    -   If the `match_length` (from the token's lower nibble) is `0xF` (15), it signifies that additional bytes follow to extend the match length.
    -   Similar to literal length, subsequent bytes are added to the match length, with `0xFF` indicating continuation.
    -   The final `match_length` value is implicitly increased by 4 (i.e., `accumulated_value + 4`). This implies a minimum match length of 4.
-   **Offset:**
    -   After the literal data (and if a match is indicated), a 2-byte `offset` is read. This `offset` indicates how many bytes backward from the current position the match sequence starts.
-   **Match Copying:**
    -   The decoder copies `match_length` bytes from the `output_string` at `output_string.size() - offset_`. This "back-reference" mechanism is central to LZ77-based compression, where repeated sequences are replaced by pointers to their previous occurrences.
    -   The `read_copy` function handles cases where the `match_length` exceeds the `offset`, effectively copying from the newly appended data (self-referencing copy).

## Code Structure and Components

-   `Lz4` Struct: This structure encapsulates the state and logic for processing a single LZ4 block.
    -   `literal_length_`, `match_length_`: Store the decoded lengths.
    -   `v_literal_length_`, `v_match_length_`: Vectors to accumulate additional bytes for lengths.
    -   `literals_`: Stores the raw literal data.
    -   `offset_`: Stores the match offset.
    -   `input_`: Reference to the input file stream.
    -   `read_additional()`: Helper function to read continuation bytes for lengths.
    -   `read_match_copy()`: Reads the offset and computes the full match length.
    -   `read_copy()`: Performs the byte copying for matches.
-   `decode` Function:
    -   Manages the overall decoding process.
    -   Reads and validates the file header (magic number, file size, constant).
    -   Iterates through blocks, reading `block_length` and processing LZ4 elements using the `Lz4` struct.
    -   Appends literals and copies match sequences to the `output_string`.
    -   Writes the final `output_string` to the output file.

## Program Execution (`main` function)

The `main` function serves as the entry point:

-   Validates command-line arguments (expects input and output file paths).
-   Opens input and output file streams in binary mode.
-   Calls the `decode` function to perform the decompression.
-   Returns `EXIT_SUCCESS` or `EXIT_FAILURE` based on the decoding result.

## Usage

To compile and run the decoder:

```bash
g++ maindc.cpp -o lz4_decoder
```

Then, execute it with a compressed LZ4 file and a desired output file:

```bash
./lz4_decoder input.lz4 output.txt
```

Replace `input.lz4` with the path to your LZ4 compressed file and `output.txt` with the desired name for the decompressed output.
