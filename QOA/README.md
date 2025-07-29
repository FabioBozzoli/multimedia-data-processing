# C++ QOA to WAV Audio Converter

This repository contains a C++ program designed to read audio data from QOA (Quite OK Audio) files, process it, and then write the output to WAV (Waveform Audio File Format) files. The processing involves a basic LMS (Least Mean Squares) adaptive filter during the QOA decoding.

## Overview

The QOA format is a relatively new, simple audio compression format. This program demonstrates the process of decoding QOA audio streams, applying a filtering step, and then converting the decoded audio into the widely supported WAV format.

## Features

-   `lmsState` Structure: Defines a structure to hold the history and weights for the LMS adaptive filter used during QOA decoding.
-   `clip` Function: A utility function to clip `int32_t` values to the range of `int16_t`, preventing overflow when converting audio samples.
-   `read_qoa` Function:
    -   Reads QOA file headers, including magic number, number of samples, channels, sample rate, and frame size.
    -   Initializes LMS states for each audio channel based on data from the QOA header.
    -   Decodes QOA audio slices, processing scale factors and dequantizing residuals.
    -   Applies the LMS adaptive filter:
        -   Calculates a prediction `p` based on historical samples and current weights.
        -   Computes the new sample `s` by adding the prediction to the dequantized residual.
        -   Updates the LMS weights and history based on the prediction error, implementing a simple adaptive filtering mechanism.
    -   Collects decoded `int16_t` audio samples into a `std::vector`.
-   `write_wav` Function:
    -   Constructs a standard WAV file header (RIFF, fmt, data chunks).
    -   Fills in WAV header details such as audio format (PCM), number of channels (hardcoded to 2), sample rate (hardcoded to 44100 Hz), byte rate, block align, and bits per sample (hardcoded to 16).
    -   Writes the raw `int16_t` audio samples into the WAV data chunk.
    -   Updates the file size in the RIFF header after all data has been written.

## Program Workflow

The `main` function orchestrates the audio conversion process:

1.  **Argument Handling:** Checks if exactly two command-line arguments are provided (input QOA file and output WAV file).
2.  **File Opening:** Opens the input QOA file for reading and the output WAV file for writing, both in binary mode.
3.  **QOA Decoding:** Calls `read_qoa` to decode the audio data from the input QOA file and store the resulting PCM samples in a `std::vector<int16_t>`.
4.  **WAV Encoding:** Calls `write_wav` to take the decoded samples and write them into the output WAV file.
5.  **Exit Status:** Returns `EXIT_SUCCESS` upon successful completion or `EXIT_FAILURE` if any errors occur (e.g., incorrect arguments, file open failures).

## Usage

To compile and run this program, you will typically use a C++ compiler like g++:

```bash
g++ main.cpp -o qoa_to_wav
./qoa_to_wav input.qoa output.wav
```

Replace `input.qoa` with the path to your QOA audio file and `output.wav` with the desired name for the resulting WAV file.
