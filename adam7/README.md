# C++ Image Compression/Decompression using Wavelet-like Quadtree

This repository contains a C++ program for image compression and decompression, specifically designed for PGM (Portable Graymap) images. The core idea is to utilize a wavelet-like quadtree structure for multi-resolution image representation and processing, enabling efficient storage and reconstruction.

## Overview

The program implements a compression scheme that decomposes an image into multiple resolution levels based on a custom "Adam7-like" mask. This allows for progressive encoding, where lower resolution data is stored first, followed by higher resolution details. The decompression process reconstructs the image from these levels, providing control over the level of detail.

## Features

-   `mat` Structure: A custom structure to represent a 2D matrix of `uint8_t` (grayscale pixel values). It includes methods for accessing pixels, setting dimensions, and adding pixels.
-   `mat_adam7` Function: Generates a mask based on an 8x8 "Adam7-like" pattern. This mask is crucial for segmenting the image into different resolution levels, mimicking a wavelet decomposition. Each pixel in the mask indicates which "level" of detail that corresponding image pixel belongs to.
-   `read_pgm` Function: Reads PGM (Portable Graymap) image files. It handles standard PGM headers and extracts pixel data into the `mat` structure.
-   `write_MLT` Function (Multi-Level Tree Encoding):
    -   Encodes an image into a custom "MULTIRES" format.
    -   Writes image dimensions.
    -   Iterates through the image pixels based on the generated mask, storing pixels level by level. Pixels belonging to level 1 are stored first, then level 2, and so on, up to level 7. This creates a multi-resolution compressed stream.
-   `read_mlt` Function (Multi-Level Tree Decoding):
    -   Reads from the custom "MULTIRES" format.
    -   Reconstructs the mask based on the image dimensions.
    -   Populates the image pixels by reading data from the stream, level by level, up to a specified or available level. This allows for partial decoding to reconstruct lower resolution versions of the image.
-   `sub_mat` Function: A utility function for extracting and manipulating sub-matrices (regions) of an image.
-   `write_pgm_level_i` Function:
    -   Saves images at different resolution levels as PGM files.
    -   Based on the mask, it reconstructs an image up to a certain `level` of detail.
    -   It uses a map to determine the block sizes for each level (e.g., level 1 uses 8x8 blocks, level 7 uses 1x1 blocks).
    -   This allows visualization of the progressive reconstruction of the image.

## Main Program Workflow

The `main` function handles command-line arguments to perform either compression or decompression:

-   **Compression (`c` argument):**
    1.  Reads an input PGM image.
    2.  Generates an Adam7-like mask for the image.
    3.  Encodes the image into the custom "MULTIRES" format using `write_MLT` and saves it to an output file.
-   **Decompression (`d` argument):**
    1.  Reads from a "MULTIRES" input file using `read_mlt`, reconstructing the image up to a certain level.
    2.  Reconstructs and saves various resolution levels of the image as separate PGM files using `write_pgm_level_i`.

## Usage

To compile and run the program:

```bash
g++ main.cpp -o image_compressor
```

### For Compression:

```bash
./image_compressor c input.pgm output.mlt
```

This will read `input.pgm` and compress it into `output.mlt`.

### For Decompression:

```bash
./image_compressor d input.mlt output_basename
```

This will read `input.mlt` and decompress it, generating multiple PGM files (e.g., `output_basename_1.pgm`, `output_basename_2.pgm`, etc.), each representing a different resolution level of the original image.
