# C++ Y4M Video Frame Extractor and Converter

This repository contains C++ code for processing Y4M (YUV4MPEG2) video files. It provides functionalities to extract individual frames from a Y4M stream and convert them into either color (PPM) or grayscale (PGM) image formats.

## Overview

Y4M is a simple, header-based video file format often used for uncompressed or minimally compressed video streams, particularly in the context of codecs development and testing. This program demonstrates how to parse a Y4M stream, extract its YUV components, and convert them to more common image formats for visualization or further processing.

## Core Components

### `mat.h`: Generic Matrix Class

This templated `mat` class serves as a fundamental data structure for representing 2D matrices of any data type. It's used throughout the project to store image pixel data efficiently.

### `utils.h`: Utility Functions

-   `raw_write`: A templated utility function for writing raw binary data to an output stream, useful for efficient file I/O.

### `types.h`: Custom Types

-   `vec3b`: A type alias for `std::array<uint8_t, 3>`, representing a 3-byte vector, primarily used for RGB or YCbCr pixel components.

### `ppm.h`/`ppm.cpp`: PPM Image File Handling

These files provide functions for saving and loading PPM (Portable Pixmap) image files, supporting both ASCII (P3) and binary (P6) formats. PPM is used here for saving the extracted color frames.

### `pgm.h`/`pgm.cpp`: PGM Image File Handling

Similar to the PPM handlers, these files provide functions for saving and loading PGM (Portable Graymap) image files, supporting both ASCII (P2) and binary (P5) formats. PGM is used for saving the extracted grayscale frames.

## Y4M Extraction Functions

The project includes two main functions for extracting frames from a Y4M file:

### `y4m_extract_color` (Color Frame Extraction)

This function extracts YUV (YCbCr) components from a Y4M stream and converts them into RGB color frames.

-   **Header Reading:** Parses the Y4M header to extract video dimensions (width 'W', height 'H').
-   **Frame Parsing:** Iterates through the video stream, reading Y (Luminance), Cb (Chroma Blue), and Cr (Chroma Red) components for each frame. It assumes 4:2:0 subsampling, where Cb and Cr planes are half the resolution of the Y plane.
-   **YCbCr to RGB Conversion:** Applies standard conversion formulas to transform YCbCr pixel values to RGB.
-   **Saturation:** Includes a `saturation` function to clip YCbCr values to typical video ranges (16-235 for Y, 16-240 for Cb/Cr) before conversion, which helps prevent color clamping issues.
-   **Output:** Stores each converted RGB frame as a `mat<vec3b>` in a `std::vector`.

### `y4m_extract_gray` (Grayscale Frame Extraction)

This function extracts only the Y (luminance) component from a Y4M stream, effectively creating grayscale frames.

-   **Header Reading:** Similar to `y4m_extract_color`, it parses the Y4M header.
-   **Frame Parsing:** Reads only the Y component for each frame, discarding the Cb and Cr chroma components.
-   **Output:** Stores each grayscale frame as a `mat<uint8_t>` in a `std::vector`.

## Usage Example (from commented `main` functions)

The commented `main` functions in the source files provide examples of how these functions can be used:

### Color Frame Conversion Example:

```cpp
// Example to extract first 30 color frames and save as PPM
std::vector<mat<vec3b>> frames;
y4m_extract_color(argv[1], frames); // argv[1] is the input Y4M filename
std::string s;
for (char i=0; i<30; ++i) {
    char ss = 'a'+i;
    s += ss;
    save_ppm(s+".ppm", frames[i], true); // Saves as ASCII PPM
}
```

### Grayscale Frame Conversion Example:

```cpp
// Example to extract first 16 grayscale frames and save as PGM
std::vector<mat<uint8_t>> frames;
y4m_extract_gray(argv[1], frames); // argv[1] is the input Y4M filename
std::string s;
for (char i=0; i<16; ++i) {
    char sd = 'a'+i;
    s+=sd;
    save_pgm(s+".pgm", frames[i], true); // Saves as ASCII PGM
}
std::cout << frames.size(); // Prints the number of extracted frames
```

To use this code, you would compile the relevant `.cpp` files together (e.g., `y4m_extract_color.cpp`, `ppm.cpp`, `mat.cpp`) and then run the executable with a Y4M file as input.

```bash
# Example compilation for color extraction:
g++ y4m_extract_color.cpp ppm.cpp mat.cpp -o y4m_color_extractor
./y4m_color_extractor input.y4m

# Example compilation for grayscale extraction:
g++ y4m_extract_gray.cpp pgm.cpp mat.cpp -o y4m_gray_extractor
./y4m_gray_extractor input.y4m
```
