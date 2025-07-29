# PPM Image Processor and Encoder

This C++ program is designed to load P6 (binary) PPM (Portable Pixmap) image files, split them into their individual Red, Green, and Blue color channels, and then encode these channels using a combination of PackBits run-length encoding and Base64 encoding. The final output is a JSON string containing the image dimensions and the Base64-encoded color channels.

## Functionality

The program performs the following key operations:

1.  **PPM Image Loading**: Reads a P6 (binary) PPM image file, parsing its header (magic number, width, height, max color value), and loading the pixel data into a `mat<vec3b>` structure.
2.  **RGB Channel Splitting**: Separates the loaded RGB image into three individual grayscale matrices, one for each Red, Green, and Blue channel. Each channel is represented as a `mat<uint8_t>`.
3.  **PackBits Encoding**: Applies a custom PackBits run-length encoding algorithm to each of the R, G, and B channel matrices. This compression scheme is designed to efficiently store sequences of repeated bytes or non-repeated bytes.
4.  **Base64 Encoding**: Encodes the PackBits-compressed byte vectors for each channel into a Base64 string. This makes the binary data safe for inclusion in text-based formats like JSON.
5.  **JSON Output Generation**: Constructs a JSON string that includes the original image's width and height, along with the Base64-encoded strings for the Red, Green, and Blue channels.

## Supported Features

*   **P6 PPM Format Support**: Specifically designed to load and parse binary P6 PPM image files.
*   **Custom `mat` and `vec` Classes**: Utilizes custom-defined `mat<T>` for 2D array handling and `vec<T, N>` for fixed-size vectors (e.g., `vec3b` for RGB pixels), providing a lightweight data structure.
*   **PackBits Compression**: Implements a form of PackBits run-length encoding for efficient storage of image data, particularly effective for images with contiguous blocks of the same color.
*   **Base64 Encoding**: Converts binary data into a text-based format suitable for embedding within JSON or other text files.
*   **Command-Line Interface**: The `main` function is set up to accept a filename as a command-line argument, process it, and output the resulting JSON to standard output.

## Usage

1.  **Dependencies**: The program uses standard C++ libraries (`fstream`, `string`, `vector`, `cstdint`, `array`, `iostream`, `cmath`). No external libraries are required beyond what's included in a standard C++ compiler.
2.  **Compiling**: Compile all `.cpp` files together. For example, assuming all source files are in the same directory:
    ```bash
    g++ main.cpp ppm.cpp compress.cpp process_ppm.cpp -o ppm_encoder
    ```
    *(Note: You might need to adjust the compilation command based on how your `mat.h` and `vec.h` are included, or if `json.cpp` contains more than just the `JSON` function.)*
3.  **Running the Program**:
    Execute the compiled program from the command line, providing the path to a P6 PPM image file as an argument:

    ```bash
    ./ppm_encoder <path_to_image.ppm>
    ```

    The program will then print the JSON output to the standard output.

### Example

Let's assume you have a P6 PPM image named `example.ppm`. You can process it as follows:

```bash
./ppm_encoder example.ppm > output.json
```

This command will process `example.ppm` and save the resulting JSON string into a file named `output.json`. The `output.json` file will contain the image's dimensions and the Base64-encoded R, G, and B channels.

---

**Summary**: This program provides a complete solution for processing P6 PPM image files, including loading, RGB channel splitting, PackBits compression, and Base64 encoding, outputting the results as a structured JSON string.
