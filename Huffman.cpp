#include "Huffman.h"
#include <bitset>
#include <stdarg.h>
#include <bits/stdc++.h>

using namespace std;

// Constructor
Huffman::Huffman() : root(nullptr) {}

// Destructor
Huffman::~Huffman() {
    deleteTree(root);
}

// deleteTree method
void Huffman::deleteTree(Node *node) {
    if (node == nullptr) return;   
    
    deleteTree(node->left);        
    deleteTree(node->right);       
    
    delete node;                   
}

// printTree method
void Huffman::printTree(Node* node)
{
    if(!node) return;

    printTree(node->left);
    printTree(node->right);

    cout << node->ch << "_";
}


// Counts the frequency of each character in the input text
unordered_map<char, int>  Huffman::countFrequencies(const string& text) {
    unordered_map<char, int> freq;

    for(char ch : text) {
        freq[ch]++;
    }

    return freq;
}

// Build a priority queue (min-heap) of leaf nodes using character frequencies
priority_queue<Node*, vector<Node*>, Compare> Huffman::buildPriorityQueue(const unordered_map<char, int>& freq) {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    
    for(auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }

    return pq;
}

// Construct the Huffman Tree from the priority queue and returns its root
Node* Huffman::buildHuffmanTreeFromQueue(priority_queue<Node*, vector<Node*>, Compare> pq) {
    while(pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();

        int sum = left->freq + right->freq;
        pq.push(new Node('\0', sum, left, right));
    }

    return pq.top();
}

// Recursively traverses the Huffman Tree to generate binary codes for each character
void Huffman::generateCodes(Node* node, string code, unordered_map<char, string>& huffmanCode) {
    if(!node) return;

    // If leaf node, assign the code
    if(!node->left && !node->right) {
        huffmanCode[node->ch] = code;
    }

    generateCodes(node->left, code + "0", huffmanCode);
    generateCodes(node->right, code + "1", huffmanCode);
}

// Serializes the Huffman tree using preorder traversal:
// '0' for internal nodes, '1' + char for leaf nodes.
void Huffman::serializeTree(const Node* node, string& out) {
    if(!node) {
        return;
    }

    if(!node->left && !node->right) {
        out += "1";
        out += node->ch;
        return;
    }

    out += "0";

    serializeTree(node->left, out);
    serializeTree(node->right, out);
}

// Encode the input text using generated Huffman codes
string Huffman::encodeText(const string& input, unordered_map<char, string>& huffmanCode) {
    string encoded = "";
    for(char ch : input) {
        encoded += huffmanCode.at(ch);
    }

    return encoded;
}

// Packs a string of '0' and '1' bits into a byte vector.
// Each 8 bits are combined into one byte, reducing memory usage.
vector<uint8_t> Huffman::packBits(const string& bitString) {
    vector<uint8_t> packed;
    uint8_t currentByte = 0;
    int bitCount = 0;

    for (char bit : bitString) {
        currentByte = (currentByte << 1) | (bit == '1' ? 1 : 0);
        bitCount++;

        if (bitCount == 8) {
            packed.push_back(currentByte);
            currentByte = 0;
            bitCount = 0;
        }
    }

    uint8_t padding = 0;
    
    // If any bits left, shift them left
    if (bitCount > 0) {
        padding = (uint8_t)(8 - bitCount);
        currentByte <<= (8 - bitCount);
        packed.push_back(currentByte);
    }

    // Last 8 bits are for padding (the left 4 bits in a byte)
    padding <<= 4;
    
    // the right 4 bits are used as a flag
    // '0000' - not compressed, '0001' - compressed 
    packed.push_back(padding | 1);

    return packed;
}

// Converts the size of the serialized Huffman tree (in bits) to a binary string
// with leading zeros removed (minimal binary representation).
string Huffman::encodeTreeSize(size_t sizeInBits) {
    bitset<16> bits(sizeInBits);

    return bits.to_string();
}

// Calculates the number of bits required to serialize the Huffman tree.
// '0' and '1' count as 1 bit, while leaf characters are treated as full bytes (8 bits).
size_t Huffman::calculateSerializedTreeLength(const string& treeBits) {
    size_t treeBitsSize = 0;
    for(int i = 0; i < treeBits.size(); i++) {
        if(treeBits[i] == '0' || treeBits[i] == '1')
            treeBitsSize++;
        else {
            treeBitsSize += 8;
        }
    }
    
    return treeBitsSize;
}

// Concatenates multiple bitstrings into a single full bitstream
string Huffman::assembleBitstream(const initializer_list<string>& parts) {
    string result;
    for (const string& part : parts) {
        result += part;
    }

    return result;
}

// Method used to convert a serialized Huffman tree into a string consisting of only 0s and 1s
string Huffman::transformToBinaryASCII(const string& input) {
    string result;
    char previous = '\0';

    for (char current : input) {
        if ((current == '0' || current == '1') && previous != '1') {
            result += current; // Keep bit
        } else {
            // Tansform to ASCII binary representation
            for (int i = 7; i >= 0; i--) {
                result += ((current >> i) & 1) ? '1' : '0';
            }
        }

        if(previous == '1' && current == '1') previous = '\0';
        else previous = current;
    }
    
    return result;
}

// Compression 
string Huffman::process(const string& input) {
    if (input.empty()) {
        return "";
    }

    auto freq = countFrequencies(input);
    auto pq = buildPriorityQueue(freq);
    Node* root = buildHuffmanTreeFromQueue(pq);

    unordered_map<char, string> huffmanCode;
    generateCodes(root, "", huffmanCode);

    // Serialize (encode) tree
    string treeBits; 
    serializeTree(root, treeBits);

    string treeBitsASCII = transformToBinaryASCII(treeBits);

    // Encode inpput
    string dataBits = encodeText(input, huffmanCode);

    // Encode tree size
    size_t treeSize = calculateSerializedTreeLength(treeBitsASCII);
    if(treeSize > 65535) { // if more than 16 bits
        cerr << "\nError: Tree is way too big." << endl;
        exit(EXIT_FAILURE);
    }

    string sizeBits = encodeTreeSize(treeSize);

    // Conect all encoded strings
    string fullBits = assembleBitstream ({sizeBits, treeBitsASCII, dataBits});
    vector<uint8_t> packed = packBits(fullBits);
    
    deleteTree(root);

    // Convert to string
    string compressed = string(packed.begin(), packed.end());
    if(compressed.size() >= input.size()) {
        // 0 bit flag - not compressed
        return input + "0"; 
    } else {
        // the last bit in this string is 1 indicating the input was compressed
        return compressed;
    }
}

// Converts a string of packed bytes into a string of '0' and '1' characters representing the bits.
string Huffman::unpackBits(const string& packed) {
    string bitString;

    for(uint8_t byte : packed) {
        bitset<8> bits(byte);
        bitString += bits.to_string();
    }

    return bitString;
}

// If text was not compressed, this method would return {0, input binary ASCII coded}
// Otherwise {1, tree size, serialized tree with charaters representet as binary ASCII, compressed text}
vector<string> Huffman::splitBitstream(const string& bitstream) {
    vector<string> strings;

    // The very last bit of bitstream
    // is a flag indicating if the data is compressed ('1') or not ('0')  
    if(bitstream.back() == '1') {
        strings.push_back("1");

        // first 16 bits are size of binary tree
        string treeSizeBits = bitstream.substr(0, 16);
        strings.push_back(treeSizeBits); 
        // Convert the first 16 bits to integer treeSize
        uint16_t treeSize = static_cast<uint16_t>(bitset<16>(treeSizeBits).to_ulong());
        // Next 'treeSize' bits represent the serialized Huffman tree
        string serializedTree = bitstream.substr(16, treeSize);
        strings.push_back(serializedTree); 

        // Remaining bits are the encoded (compressed) data
        size_t dataStart = 16 + treeSize;
        size_t dataLength = bitstream.size() - dataStart;  // -1 for the last flag bit
        string data = bitstream.substr(dataStart, dataLength);
        strings.push_back(data); 

    } else {
        strings.push_back("0");

        // All bits expect for the last one are the not-compressed data
        strings.push_back(bitstream.substr(0, bitstream.size() - 1)); // -1 for the last flag bit
    }

    return strings;
}

// Method to rebuild a Huffman tree
Node* Huffman::deserializeTree(const string& serializedTree) {
    size_t index = 0;
    return deserializeHelper(serializedTree, index);
}

// Recursive method that helps rebuild the Huffman tree
Node* Huffman::deserializeHelper(const string& serializedTree, size_t& index) {
    if (index >= serializedTree.size()) {
        return nullptr;
    }

    char type = serializedTree[index++];

    if (type == '1') {
        // We reached the leaf node - the next 8 bits are a binary representation of a character
        // in a serialized tree
        if (index + 8 > serializedTree.size()) {
            cerr << "Error: Not enough bits for char at index " << index << endl;
            return nullptr;
        }

        string charBits = serializedTree.substr(index, 8);
        bitset<8> bits(charBits);
        char ch = static_cast<char>(bits.to_ulong());
        
        index += 8;

        return new Node(ch, 0);
    } 
    else if (type == '0') {
        // Inner node
        Node* left = deserializeHelper(serializedTree, index);
        Node* right = deserializeHelper(serializedTree, index);

        return new Node('\0', 0, left, right);
    }

    cerr << "Error: Unexpected type character '" << type << "' at index " << index << endl;
    return nullptr;
}


// Based on reconstructed Huffman tree, decode the comporessed message
string Huffman::decode(const string& bitString, Node* root) {
    Node *current = root;
    string result = "";

    for(char bit : bitString) {
        // Inner nodes
        if(bit == '0') {
            current = current->left;
        } else if(bit == '1') {
            current = current->right;
        }
        
        // Check if a leaf reached
        if(!current->left && !current->right) {
            result += current->ch;
            current = root; // Go back to the root
        }
    }

    return result;
}

// Method used to decode text that wasn't compressed
// Input consists only of 0s and 1s because all of it's charaters are
// represented as ASCII binary codes
string Huffman::transformToChar(const string& input) {
    string result = "";
    uint8_t currentByte = 0;
    size_t counter = 0;

    for (char bit : input) {
        currentByte = (currentByte << 1) | (bit == '1' ? 1 : 0);
        counter++;

        if(counter == 8) {
            result += static_cast<char>(currentByte);
            counter = 0;
            currentByte = 0;
        } 
    }
    
    return result;
}

// Method that returns data without padding made in compression
string Huffman::removePadding(const string& input) {
    if (input.size() < 8) return "";

    string lastByte = input.substr(input.size() - 8, 8);

    int padding = 0;
    for (int i = 0; i < 4; ++i) {
        padding = (padding << 1) | (lastByte[i] == '1' ? 1 : 0);
    }

    string data = input.substr(0, input.size() - 8);

    if (padding > 0 && padding <= 7) {
        data = data.substr(0, data.size() - padding);
    }

    return data;
}

// Decompression
string Huffman::deprocess(const string& input) {
    string unpacked = unpackBits(input);
    vector<string> strings = splitBitstream(unpacked);

    if(strings.at(0) == "0") {
        return transformToChar(strings.at(1));  // Not-compressed data
    } 

    // Decompress data
    string serializedTree = strings.at(2);
    string data = removePadding(strings.at(3));
    Node *root = deserializeTree(serializedTree);
    
    string decompressed = decode(data, root);
   
    deleteTree(root);
    return decompressed;
}
