# C++ BMP to PAM Image Converters

This repository contains C++ programs designed to convert various types of BMP (Bitmap) image files into the PAM (Portable Arbitrary Map) format. The conversion process involves reading BMP header information, handling color tables (for paletted BMPs), extracting pixel data, and then writing the image data in the PAM format.

## Overview

The BMP format has several variants depending on the bit-depth and compression. This collection of converters addresses different common BMP types, providing a flexible solution for transforming them into the PAM format, which is a simpler, extensible image format often used in image processing pipelines.

## Common Features

Across all provided converters, you will find the following shared elements:

-   `mat` Structure: A custom structure to represent the image data. It typically stores `rows_`, `cols_`, and a `std::vector` of `std::array<uint8_t, 3>` (for RGB pixel data). It includes methods for setting dimensions, accessing pixels by row/column or linear index, and adding individual pixels.
-   `read_header` Function: Reads the initial part of the BMP file to extract crucial information such as:
    -   Magic number (`0x4D42` for "BM").
    -   Image width and height.
    -   The starting offset of the pixel data within the file (`starting_point`).
    -   For paletted BMPs, the number of colors in the color table (`num_colors`).
-   `write_pam` Function: Writes the processed image data into a PAM file. The PAM header is consistent across all converters, specifying `P7` (for RGB), `WIDTH`, `HEIGHT`, `DEPTH 3`, `MAXVAL 255`, `TUPLTYPE RGB`, and `ENDHDR`. The pixel data is written row by row, from bottom to top (typical for BMPs, which store pixels inverted vertically).

## Specific Converters

This repository includes programs tailored for different BMP bit-depths:

### 1. `main.cpp` (Possibly 4-bit BMP)

This converter appears to handle BMP files that use a color table where each pixel is represented by 4 bits.

-   `read_header`: Determines `num_colors` (if 0, defaults to `pow(2, 4)`, implying a 4-bit color depth).
-   `read_color_table`: Reads a color palette of `num_colors` entries, where each entry is 4 bytes (B, G, R, reserved).
-   `read_pixels`: Reads pixel data where each byte contains two 4-bit pixel indices. It then uses the `color_table` to map these indices to RGB values and adds them to the `mat` structure. It also handles row padding.

### 2. `bmp08.cpp` (8-bit BMP)

This converter is designed for 8-bit paletted BMP files.

-   `read_header`: Determines `num_colors` (if 0, defaults to `pow(2, 8)`, implying an 8-bit color depth).
-   `read_color_table`: Similar to `main.cpp`, it reads the color palette based on `num_colors`.
-   `read_pixels`: Reads pixel data where each byte directly represents an index into the `color_table`. It then retrieves the corresponding RGB values and adds them to the `mat` structure. It also handles row padding.

### 3. `bmp24.cpp` (24-bit BMP)

This converter handles 24-bit BMP files, which do not use a color table, as pixel data is directly stored in RGB format.

-   `read_header`: Does not read `num_colors` as 24-bit BMPs do not have a color table. The header parsing is adjusted accordingly.
-   `read_pixels`: Directly reads 3 bytes (B, G, R) for each pixel from the input stream. It then reorders them to R, G, B before adding them to the `mat` structure. It also handles row padding.

## Usage

To compile and run any of these converters:

```bash
g++ <source_file>.cpp -o bmp_to_pam_converter
./bmp_to_pam_converter input.bmp output.pam
```

Replace `<source_file>.cpp` with `main.cpp`, `bmp08.cpp`, or `bmp24.cpp` depending on the type of BMP file you intend to convert. `input.bmp` should be your source BMP file, and `output.pam` will be the generated PAM file.
