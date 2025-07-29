C++ Image Processing Project

This repository contains a C++ program for image processing, focusing on reading, manipulating, and writing files in PGM and PPM formats. The project implements the demosaicing (color interpolation) of a grayscale image to which a Bayer filter has been applied.

Overview

The main purpose of this code is to demonstrate a complete workflow for color reconstruction from raw image data, similar to that captured by a digital camera sensor with a Bayer Color Filter Array (CFA).

Features

`mat` Data Structure: A generic matrix (`template`) data structure to represent images, capable of handling different data types per pixel.

PGM File Reading and Writing:
- `read_pgm`: Reads 16-bit PGM ("P5") image files.
- `write_pgm`: Writes 8-bit PGM ("P5") image files, converting the data from 16-bit.

Bayer Filter:
- `bayer`: Applies a Bayer filter pattern to a grayscale image to simulate the output of a raw image sensor, where each pixel captures only one color (red, green, or blue).

Color Interpolation (Demosaicing):
- A set of functions (`green`, `r_b_green`, `red_blue`) that reconstruct the missing color channels for each pixel through interpolation algorithms. This process converts the Bayer-filtered image into a full-color image.

PPM File Writing:
- `write_ppm`: Saves the resulting color image in the PPM ("P6") format.

Program Workflow

The `main` executable orchestrates the process in the following order:

1.  Read: Reads a 16-bit PGM input image from the first command-line argument.
2.  Grayscale Conversion: Converts the 16-bit image to an 8-bit one and saves it as `*_gray.pgm`.
3.  Bayer Filter: Applies the Bayer filter to the grayscale image and saves the result as `*_bayer.ppm`.
4.  Interpolation: Performs a series of interpolation steps to reconstruct the color channels:
    - Reconstructs the green channel.
    - Reconstructs the red and blue channels for the green pixels.
    - Reconstructs the remaining red and blue channels.
5.  Final Output: Saves the fully interpolated color image as `*_interp.ppm`. Intermediate results are also saved for analysis.

Usage

To compile and run the program, provide two command-line arguments:

1.  The path to the input PGM file.
2.  The base name for the output files.

```bash
g++ main.cpp -o image_processor
./image_processor path/to/input.pgm output_basename
```
