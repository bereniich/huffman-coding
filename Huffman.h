#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <initializer_list>

// A Huffman tree node
struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;

    // Constructors
    Node(char ch, int freq) : ch(ch), freq(freq), left(nullptr), right(nullptr) {}
    Node(char ch, int freq, Node* left, Node* right) : ch(ch), freq(freq), left(left), right(right) {}
};

// Comparison object to be used to order the heap
struct Compare {
    bool operator() (Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

class Huffman {
private:
    Node* root;  
    
    std::unordered_map<char, int> countFrequencies(const std::string& text); 
    std::priority_queue<Node*, std::vector<Node*>, Compare> buildPriorityQueue(const std::unordered_map<char, int>& freq);
    Node* buildHuffmanTreeFromQueue(std::priority_queue<Node*, std::vector<Node*>, Compare> pq);
    void generateCodes(Node* node, std::string code, std::unordered_map<char, std::string>& huffmanCode);
    std::string encodeText(const std::string& input, std::unordered_map<char, std::string>& huffmanCode);
    void serializeTree(const Node* node, std::string& out);
    std::string transformToBinaryASCII(const std::string& input);
    size_t calculateSerializedTreeLength(const std::string& treeBits);
    std::vector<std::uint8_t> packBits(const std::string& bitString);
    std::string encodeTreeSize(size_t sizeInBits);
    std::string assembleBitstream(const std::initializer_list<std::string>& parts);

    std::string unpackBits(const std::string& packed);
    std::vector<std::string> splitBitstream(const std::string& bitstream);
    Node* deserializeTree(const std::string& data);
    Node* deserializeHelper(const std::string& data, size_t& index);
    std::string removePadding(const std::string& input);
    std::string transformToChar(const std::string& input);
    std::string decode(const std::string& bitString, Node* root);
    
public:
    Huffman();
    ~Huffman();

    std::string process(const std::string& input); // Compress
    std::string deprocess(const std::string& input); // Decompress

    void deleteTree(Node* node);
    void printTree(Node* node);
};

#endif
