## PCX Image Loader

This program provides a simple C++ implementation for reading and decoding monochrome PCX (Picture Exchange) image files. It consists of a set of header and source files that define a minimal matrix class and a function to load PCX images into this matrix structure.

### Features

- **PCX Decoding**: Supports decoding of basic (1-bit per pixel) PCX images, reading headers and decompressing pixel data accordingly.
- **Generic Matrix Container**: Includes a lightweight `mat<T>` class template for handling 2D arrays of arbitrary types, used here for image storage.
- **Simple API**: Offers the `load_pcx` function, which takes a filename and loads the image data into a matrix of unsigned char.

### Usage

1. **Dependencies**: Only standard C++ libraries are used; no external dependencies are required.
2. **Compiling**: Compile all `.cpp` files together. For example:
   ```bash
   g++ main.cpp pcx.cpp -o pcx_loader
   ```
3. **Loading an Image**:
   ```cpp
   #include "pcx.h"
   #include "mat.h"

   mat<uint8_t> img;
   bool success = load_pcx("example.pcx", img);
   if (success) {
       // Image data is now loaded into img
   }
   ```

### File Overview

- **mat.h**: Defines a simple, resizable 2D matrix template for any data type.
- **pcx.h / pcx.cpp**: Declares and implements the `load_pcx` function responsible for parsing and decompressing the PCX file data.
- **main.cpp**: Simple example showing how to load a PCX image.

### How it Works

- The `load_pcx` function reads the PCX file header, checks file integrity, and extracts image dimensions.
- It then parses and decompresses the RLE-packed image data, converting it into a binary (black and white) image represented as a matrix of unsigned chars (0 for black, 255 for white).
- The matrix is resizable and abstracts away the details of data storage and indexing, making it easy to use for further processing or visualization.

### Limitations

- Only supports very basic, monochrome (1-bit) PCX files. Does not handle color, multi-plane, or more complex PCX variants.
- Error handling is minimal; invalid files may fail to load without detailed diagnostic output.

### Example

```cpp
#include "pcx.h"
#include "mat.h"

int main() {
    mat<uint8_t> img;
    if (load_pcx("bunny.pcx", img)) {
        // Process the loaded image here
    } else {
        std::cerr << "Failed to load image" << std::endl;
    }
    return 0;
}
```

---

**Summary**:  
This project provides a minimal C++ loader for reading simple, monochrome PCX files into a 2D array structure, suitable for basic image manipulation or study of the PCX file format.
