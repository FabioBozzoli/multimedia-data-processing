# C++ Binary Data Parser with CRC Verification

This repository contains a C++ program for reading and parsing binary data files, with a strong emphasis on data integrity verification using CRC (Cyclic Redundancy Check) checksums. The program is designed to read file headers, validate them with CRC, and then parse structured data messages, including definition and data messages.

## Overview

The program appears to be tailored for processing a specific binary file format, likely related to fitness or sensor data (given the `time_created` and `avg_speed` output examples). It demonstrates robust binary file I/O, custom CRC calculation, and dynamic interpretation of message structures defined within the file itself.

## Features

-   `FitCRC_Get16` Function: Implements a 16-bit CRC calculation based on a predefined lookup table (`crc_table`). This function updates a CRC value incrementally, processing data byte by byte. This specific CRC algorithm is often found in various protocols and file formats.
-   `definition_message` Struct: Represents a message definition within the binary file. It stores:
    -   `global_message_number_`: A global identifier for the message type.
    -   `num_field_`: The number of fields in this message definition.
    -   `fields_`: A vector of vectors, where each inner vector describes a field (e.g., field number, size, type).
    -   `local_message_type_`: A local identifier for the message type.
-   CRC Verification Functions:
    -   `check_crc_beg`: Verifies the CRC of the initial part of the file (first 12 bytes).
    -   `check_crc_end`: Verifies the CRC of the entire file, from a specific offset (after the header) to the end, including a final 2-byte CRC value at the end of the file.
-   `read_header` Function:
    -   Reads and validates the file header.
    -   Performs an initial CRC check using `check_crc_beg`.
    -   Extracts key header information, including `header_size` and `data_size`.
    -   Includes internal CRC calculations for parts of the header.
-   `read_data` Function:
    -   Parses the main data section of the file based on the `data_size` obtained from the header.
    -   **Dynamic Message Parsing:** Distinguishes between "definition messages" and "data messages" based on a byte flag.
        -   **Definition Messages:** When a definition message is encountered, it reads the `global_message_number_`, `num_field_`, `local_message_type_`, and then iterates to read the definition of each field (number, size, type). These definitions are stored in a `std::vector<definition_message>`.
        -   **Data Messages:** When a data message is encountered, it uses the `local_message_type_` to find the corresponding definition message stored previously. It then iterates through the fields defined in that message, reading the appropriate number of bytes for each field.
    -   **Data Interpretation:** Includes examples of interpreting specific fields, such as `time_created` (global message 0, field 4) and `avg_speed` (global message 19, field 13), printing their values to the console.

## Program Workflow

The `main` function orchestrates the file processing:

1.  **Argument Validation:** Checks for the correct number of command-line arguments (expects one: the path to the binary file).
2.  **File Opening:** Opens the specified binary file for reading.
3.  **Header Reading & Validation:** Calls `read_header` to process the file's initial header, including its CRC verification. If the header CRC fails, the program exits.
4.  **Data Reading & Parsing:** Calls `read_data` to parse the main data section of the file, interpreting definition and data messages.
5.  **End-of-File CRC Check:** After parsing the data, `check_crc_end` is called to verify the overall file integrity.
6.  **Status Output:** Prints "File CRC ok" if the final CRC check passes, otherwise exits with an error.

## Usage

To compile and run the program:

```bash
g++ main.cpp -o binary_parser
```

Then, execute it with the path to your binary file:

```bash
./binary_parser your_file.bin
```

The program will output parsed data (e.g., `time_created`, `avg_speed`) to the console and report on the final CRC check.
