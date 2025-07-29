# C++ Image Encoding/Decoding with Rotation and Symbol Mapping

This repository contains a C++ program that implements a unique image encoding and decoding scheme. This scheme utilizes a custom symbol set and a rotational mapping logic to transform image pixel data. The program processes image headers to extract dimensions and then applies the encoding/decoding operations to the pixel data.

## Overview

The core idea of this program is to convert raw pixel data into a sequence of printable ASCII characters using a base-85 like encoding, combined with a rotational shift. This method could be used for obfuscation, compact representation for specific purposes, or as a building block for more complex image processing pipelines.

## Features

-   **Symbol Set:** A fixed set of 85 printable ASCII characters (`'0'-'9'`, `'a'-'z'`, `'A'-'Z'`, and various symbols like `.` `-` `:` `+` etc.) is used for encoding.
-   **Rotational Mapping:**
    -   `rotation` function: Applies a forward rotational shift to an index based on a `remainder`, `n` (a rotation parameter), and a `cycle` (which tracks the position in the output stream).
    -   `rotation_decode` function: Reverses the rotational shift, mapping an encoded symbol back to its original index. This ensures that the decoding process can reconstruct the original pixel data.
-   **`fit` function:** A utility function to ensure an index wraps around within the 85-character symbol set, similar to a modulo operation but handling negative values for consistent behavior.
-   **Header Processing:** Both encoding and decoding functions read and process image headers to extract width and height information.
    -   The `encode` function expects the header lines to contain width, height, and max value.
    -   The `decode` function reads width and height from a comma-separated string in the encoded file.

## Encoding Process (`encode` function)

1.  **Read Header:** Reads the input image's header to determine `width` and `height`.
2.  **Base-85 Conversion:** Reads raw 4-byte chunks of pixel data. Each 4-byte chunk is treated as a 32-bit integer `value`. This `value` is then converted into 5 base-85 components.
3.  **Rotational Encoding:** Each of the 5 base-85 components is passed through the `rotation` function along with the `n` parameter and a `n_rotation` counter (representing the cycle/position) to generate a shifted index.
4.  **Symbol Mapping:** The shifted index is then mapped to a character from the `symbol` array, which is written to the output file.
5.  **Output Format:** The encoded output starts with `width,height,` followed by the stream of encoded symbols.

## Decoding Process (`decode` function)

1.  **Read Header:** Reads the `width` and `height` from the comma-separated header in the encoded input file.
2.  **Symbol Mapping (Reverse):** A `map_symbol` is created to quickly find the numerical index of a given symbol character.
3.  **Rotational Decoding:** Reads each encoded symbol from the input, converts it to its numerical index, and then applies the `rotation_decode` function using the `n` parameter and the current position (`i`) to get the original base-85 component.
4.  **Base-85 to Pixel Data:** (Note: The provided `decode` function has commented out and potentially incomplete logic for reconstructing the original pixel values from the base-85 components.) The intention is to convert sets of 5 base-85 components back into 4-byte pixel values.
5.  **Output (Partial):** The current `decode` function attempts to write a PPM header (`P6`) but the pixel data reconstruction and writing logic is not fully implemented in the provided snippet.

## Usage

The program takes command-line arguments to specify the operation (encode or decode), the rotation parameter `n`, and the input and output filenames.

```bash
g++ z85rotdc.cpp -o image_transformer
```

### For Encoding:

```bash
./image_transformer c <n_value> <input_image_file> <output_encoded_file>
```

-   `c`: Specifies encoding mode.
-   `<n_value>`: An integer value used for the rotational shift.
-   `<input_image_file>`: The path to the input image file (e.g., a raw pixel data file that the encoder expects).
-   `<output_encoded_file>`: The path where the encoded output will be saved.

### For Decoding:

```bash
./image_transformer d <n_value> <input_encoded_file> <output_image_file>
```

-   `d`: Specifies decoding mode.
-   `<n_value>`: The same integer value used during encoding must be provided for correct decoding.
-   `<input_encoded_file>`: The path to the encoded file.
-   `<output_image_file>`: The path where the decoded image will be saved (currently attempts to write a PPM header).

**Note:** The pixel data reconstruction and writing in the `decode` function appears incomplete in the provided code snippet.
