# Huffman Compression in C++

This project implements **Huffman coding**, a popular lossless data compression algorithm.  
It supports both **compression** and **decompression** of plain text, using a custom binary format that makes decompression possible from just a single encoded string.

---

## Features

- Compress any text using Huffman coding.
- Decompress a string without needing extra metadata or external files.
- Automatically detects whether a string is compressed or not.
- Custom compact binary format with internal tree representation.
- Handles padding and tree reconstruction with no external dependencies.

---

## How It Works

### Compression Format

The compressed string follows this structure:

| 16 bits       | n bits               | message bits   | 8-bit control byte      |
|---------------|----------------------|----------------|-------------------------|
| Tree size (n) | Serialized Huffman Tree | Encoded text | Padding + Flag           |

**1. Tree Size (16 bits)**  
- The first 16 bits represent the size of the serialized Huffman tree, in bits (`n`).

**2. Huffman Tree (n bits)**  
- The tree is serialized into a binary string using pre-order traversal:  
  - `1` followed by the 8-bit ASCII character for leaf nodes.  
  - `0` for internal nodes.

**3. Encoded Message**  
- The input text is encoded using Huffman codes generated from the tree.

**4. Control Byte (8 bits)**  
- The last byte contains two parts:  
  - **Left 4 bits:** Number of padding bits added to make total length divisible by 8 (0–8).  
  - **Right 4 bits:** Flag that tells us whether the message is compressed.  
    - `0001` → Message is compressed (format explained above).  
    - `0000` → Message is not compressed (output is binary representation of input + '0').

---

## Why This Format?

The entire compressed message can be passed as a **string**, even if it contains binary data.

- When decompressing, the program reads the **last byte**:  
  - If right nibble is `0000`, the message is plain text, no decompression needed.  
  - If right nibble is `0001`, the program:  
    1. Reads the tree size.  
    2. Reconstructs the tree.  
    3. Removes padding.  
    4. Decodes the bitstream using the Huffman tree.

> This ensures the system is **robust**, **compact**, and requires **no external metadata**.

---

## Usage

Include `Huffman.h` and `Huffman.cpp` in your project.

### Example

```cpp
#include "Huffman.h"
#include <iostream>

int main() {
    Huffman h;
    
    std::string text = "this is a simple example";
    std::string compressed = h.process(text);
    std::string decompressed = h.deprocess(compressed);

    std::cout << "Original: " << text << "\n";
    std::cout << "Compressed: " << compressed << "\n";
    std::cout << "Decompressed: " << decompressed << "\n";

    return 0;
}
````

---

## File Structure

```
Huffman/
├── Huffman.h         // Class declaration
├── Huffman.cpp       // Class implementation
├── main.cpp          // Example usage
```

---

## License

This project is open-source and available under the MIT License.

---

## Author

Ivan Berenić

Feel free to contribute, suggest improvements, or open issues!

```
