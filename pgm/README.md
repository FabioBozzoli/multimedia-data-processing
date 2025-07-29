# C++ PGM Image Processor

This repository contains a C++ program for reading, processing, and writing grayscale images in the Portable Graymap (PGM) format. It includes a generic matrix class for image data manipulation and specific functions for PGM file I/O and basic image transformations.

## Overview

The PGM format is a simple image file format that represents grayscale images. This project provides fundamental utilities for working with these images, demonstrating how to handle both plain (ASCII) and binary PGM variants.

## `mat.h`: Generic Matrix Class

This header defines a templated `mat` class, which is a versatile data structure designed to represent 2D matrices of any data type. It's used here as the underlying structure for image data.

### Features

-   Template-based: Can store elements of any data type (`T`).
-   Dimensions: Stores `rows_` and `cols_`.
-   Data Storage: Uses a `std::vector<T>` for contiguous memory allocation, allowing for efficient access.
-   Access Operators:
    -   `operator()(int r, int c)`: Provides bounds-checked access to elements using row and column indices.
    -   `operator[](int pos)`: Provides bounds-checked linear access to elements.
-   Resizing: `resize(int r, int c)` allows dynamic resizing of the matrix.
-   Raw Data Access: `rawdata()` provides a pointer to the underlying raw data, useful for efficient I/O operations.
-   Iterators: `begin()` and `end()` methods provide standard C++ iterators for easy traversal.

## `pgm.h`/`pgm.cpp`: PGM File Handling

These files provide functions for reading and writing PGM images, supporting both "plain" (P2) and "binary" (P5) modes.

### Features

-   `pgm_mode` Enum: Defines the supported PGM modes (`plain` for P2, `binary` for P5).
-   `write` Function:
    -   Writes a `mat<uint8_t>` (grayscale image) to a specified filename.
    -   Handles P2 (ASCII) by writing pixel values as integers separated by spaces.
    -   Handles P5 (binary) by writing raw pixel data directly.
    -   Includes a comment header and image dimensions.
-   `read` Function:
    -   Reads a PGM image from a specified filename.
    -   Parses the magic number (P2 or P5), comments, dimensions, and maximum pixel value.
    -   Loads pixel data into a `mat<uint8_t>` based on the PGM mode.
    -   Returns an empty `mat` if the file cannot be opened.

## `main.cpp`: Example Usage and Image Transformation

This file demonstrates how to use the `mat` and PGM utility functions, including an example image generation and a simple image transformation.

### Features

-   `make_example` Function: Creates a sample grayscale image where each row has a constant pixel value equal to its row index.
-   `flip` Function: Implements a vertical flip transformation for a grayscale image, swapping pixels from the top half with those from the bottom half.
-   Main Function:
    -   Reads a PGM image named "frog\_bin.pgm".
    -   Applies the `flip` transformation to the image.
    -   Writes the transformed image to a new file named "reverse.pgm" in binary PGM mode.

## Usage

To compile and run this program:

```bash
g++ main.cpp pgm.cpp -o pgm_processor
./pgm_processor
```

Make sure you have a `frog_bin.pgm` file in the same directory as your executable for the example in `main.cpp` to work correctly. The program will output a `reverse.pgm` file.
