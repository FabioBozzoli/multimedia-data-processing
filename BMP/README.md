# C++ Image Encoding/Decoding with Rotational Base85 Conversion

This repository contains a C++ program that implements a custom image encoding and decoding scheme. This scheme combines Base85 conversion with a rotational mapping to transform image pixel data, primarily for Portable Pixmap (PPM) images.

## Overview

The program takes a PPM image, converts its raw pixel data into a Base85 string, and then applies a rotational shift to this string. The inverse process is performed for decoding, reconstructing the original image. This method can be used for obfuscation, compact representation, or as a building block for more complex image processing pipelines.

## Features

-   **Base85 Conversion:**
    -   `from_decimal_to_z85`: Converts a 32-bit unsigned integer (representing 4 bytes of pixel data) into a 5-character Base85 string. It uses a custom symbol set of 85 printable ASCII characters.
    -   `from_z85_to_decimal`: Converts a 5-character Base85 string back into a 32-bit unsigned integer.
-   **Rotational Mapping:**
    -   `key_value`: A utility function to get the integer index of a character within the custom 85-character symbol set.
    -   `rotation`: Applies a rotational shift to a given text string. This function can perform both forward (encoding) and backward (decoding) rotations. The shift amount depends on a rotation parameter `N` and the character's position within the text.
-   **PPM File Handling:** The program reads and writes PPM (Portable Pixmap) files in their binary (`P6`) format. It specifically expects the `P6` magic number for input and outputs a `P6` header.

## Encoding Process (`encode` function)

1.  **Read Header:** Reads the input PPM file's header to determine its width and height.
2.  **Raw Data to Base85:** Reads 4 bytes of pixel data at a time from the input file, treats them as a 32-bit integer, and converts this integer to a 5-character Base85 string using `from_decimal_to_z85`. These Base85 strings are concatenated into a single string.
3.  **Apply Rotation:** The concatenated Base85 string undergoes a rotational transformation using the `rotation` function (in forward direction) with a specified rotation parameter `N`.
4.  **Write Output:** The encoded and rotated string, prefixed with the image width and height (e.g., `width,height,`), is written to the output file.

## Decoding Process (`decode` function)

1.  **Read Header:** Reads the width and height from the comma-separated header of the encoded input file.
2.  **Apply Inverse Rotation:** Reads the entire rotated Base85 string from the input file. This string is then subjected to the inverse rotational transformation using the `rotation` function (in backward direction) with the same rotation parameter `N` used during encoding.
3.  **Base85 to Raw Data:** The derotated Base85 string is processed in 5-character chunks. Each chunk is converted back into a 32-bit unsigned integer using `from_z85_to_decimal`.
4.  **Write PPM:** The 32-bit integer is then broken down into 4 individual bytes, which are written as raw pixel data to the output PPM file, prefixed with a `P6` header and image dimensions.

## Program Execution (`main` function)

The `main` function handles command-line arguments to select between encoding and decoding operations:

-   It expects exactly four command-line arguments:
    1.  An operation flag (`c` for encode, `d` for decode).
    2.  The rotation parameter `N` (an integer).
    3.  The path to the input file.
    4.  The path to the output file.
-   It parses the `N` parameter as an integer.
-   It calls either the `encode` or `decode` function based on the operation flag.

## Usage

To compile and run the program:

```bash
g++ z85rot.cpp -o image_converter
```

### For Encoding:

```bash
./image_converter c <N_value> <input_ppm_file> <output_encoded_file>
```

-   `<N_value>`: An integer value for the rotational shift (e.g., `10`).
-   `<input_ppm_file>`: Path to your input PPM (P6) image file.
-   `<output_encoded_file>`: Desired path for the encoded output file.

### For Decoding:

```bash
./image_converter d <N_value> <input_encoded_file> <output_decoded_ppm_file>
```

-   `<N_value>`: Must be the *same* integer value used during encoding.
-   `<input_encoded_file>`: Path to the encoded file created by the `encode` operation.
-   `<output_decoded_ppm_file>`: Desired path for the reconstructed PPM (P6) image file.
