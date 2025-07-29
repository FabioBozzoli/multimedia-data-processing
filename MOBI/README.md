# C++ PDB File Parser and Decompressor

This repository contains a C++ program designed to read, parse, and decompress data from Palm Database (PDB) files, specifically focusing on Palm DOC format. This tool allows for the extraction of metadata and the decompression of record data, outputting the human-readable content.

## Overview

PDB files were commonly used on Palm OS devices to store various types of data, including e-books (Palm DOC), databases, and other application-specific information. This program provides a way to inspect the structure of these files and retrieve their compressed contents.

## Features

-   `bitreader` Class: A utility class for reading bits from an input stream, essential for parsing bit-level data structures within PDB files.
-   PDB Header Parsing:
    -   Extracts key metadata from the PDB header, including the database name, creation date, type, creator, and the total number of records.
-   Record Entry Processing:
    -   Reads through the record entries section of the PDB file, collecting vital information such as the offset of each record within the file and its unique ID.
-   Palm DOC Data Extraction:
    -   Identifies specific details related to Palm DOC compression, including the compression type, total text length, record count, record size, and encryption type.
-   Data Decompression:
    -   Implements a decompression algorithm to reconstruct the original data from the compressed records. This process handles byte-level and short-length encoded data, effectively reversing the compression applied within the PDB's Palm DOC sections.
-   Output Handling:
    -   Prints parsed metadata to the console and writes the decompressed content to an output file.

## Program Workflow

The `main` function orchestrates the parsing and decompression process:

1.  **File Validation:** Checks for correct command-line arguments (input PDB file and output text file) and ensures files can be opened.
2.  **BOM Writing:** Writes a Byte Order Mark (BOM) to the output file.
3.  **Header Parsing:** Calls `PDB_header` to read and display the main PDB header information.
4.  **Record Entry Information:** Calls `info_entries` to process and display details for each record entry, storing their offsets.
5.  **Palm DOC Details:** Seeks to the first record's offset and calls `palm_DOC` to extract and display Palm DOC-specific compression and record details.
6.  **Data Decompression:** Calls `decode_records` to read from the input stream, use the stored offsets and length information to decompress the record data, and write the decompressed content to the specified output file.

## Usage

To compile and run the program, provide the path to the PDB input file and the desired output file name as command-line arguments:

```bash
g++ main.cpp -o pdb_parser
./pdb_parser input.pdb output.txt
```
