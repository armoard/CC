#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <locale>
#include <codecvt>
#include "common.h"

class Decompressor {
public:
    explicit Decompressor(std::string fileName) : fileName(std::move(fileName)), root(new Node()) {}

    void getHeaderInfo() {
        std::ifstream fileInput(fileName, std::ios::binary);
        if (!fileInput) {
            std::cerr << "ERROR: Could not open filename" << std::endl;
            return;
        }

        fileInput >> unique;
        std::cerr << "Unique symbols count: " << unique << std::endl;

        for (int i = 0; i < unique; i++) {
            int charCode;
            std::string huffmanCode;
            fileInput >> charCode >> huffmanCode;
            huffmanTable[(wchar_t)charCode] = huffmanCode;
        }

        fileInput >> toRead;
        std::cerr << "Read toRead = " << toRead << " bits" << std::endl;

        char buffer;
        for (uint32_t bitsRead = 0; bitsRead < toRead;) {
            fileInput.get(buffer);
            if (fileInput.eof()) {
                std::cerr << "ERROR: Unexpected EOF while reading compressed bits" << std::endl;
                break;
            }
            for (int i = 7; i >= 0 && bitsRead < toRead; i--) {
                compressedBits.push_back((buffer >> i) & 1);
                bitsRead++;
            }
        }

        fileInput.close();
        std::cerr << "Finished reading header and compressed data" << std::endl;
    }

    void makeTree() {
        std::cerr << "Constructing Huffman tree" << std::endl;
        for (const auto& entry : huffmanTable) {
            wchar_t character = entry.first;
            const std::string& code = entry.second;
            Node* current = root;

            for (char bit : code) {
                if (bit == '0') {
                    if (!current->left) current->left = new Node();
                    current = current->left;
                } else {
                    if (!current->right) current->right = new Node();
                    current = current->right;
                }
            }
            current->character = character;
        }
        std::cerr << "Huffman tree constructed successfully" << std::endl;
    }

    void printTree(Node* node, std::string prefix = "") {
        if (!node) return;
        if (!node->left && !node->right) {
            std::cerr << "Huffman Code for '" << node->character << "' -> " << prefix << std::endl;
        }
        printTree(node->left, prefix + "0");
        printTree(node->right, prefix + "1");
    }

    std::wstring decode() {
        std::cerr << "Starting decoding process" << std::endl;
        std::wstring decodedText;
        Node* current = root;

        if (!root) {
            std::cerr << "ERROR: Huffman tree is null, cannot decode" << std::endl;
            return decodedText;
        }

        for (size_t i = 0; i < compressedBits.size(); i++) {
            if (!current) {
                std::cerr << "ERROR: Reached a null node at position " << i << std::endl;
                break;
            }

            if (compressedBits[i] == 1) {
                current = current->right;  
            } else {
                current = current->left;   
            }

            if (current->isLeaf()) {  
                decodedText += current->character;
                current = root;
            }
        }

        std::cerr << "Finished decoding, total characters: " << decodedText.size() << std::endl;
        return decodedText;
    }

    void outputFile(const std::wstring& decodedText) {
        std::cerr << "Writing output file" << std::endl;
        std::ofstream outputFile("decompressed.txt", std::ios::binary);
        if (!outputFile) {
            std::cerr << "ERROR: Could not open output file." << std::endl;
            return;
        }

        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        std::string utf8Text = converter.to_bytes(decodedText);
        outputFile.write(utf8Text.data(), utf8Text.size());
        outputFile.close();

        std::cerr << "Decompression complete. Output saved to decompressed.txt" << std::endl;
    }

    void decompress() {
        std::cerr << "Starting decompression process" << std::endl;
        getHeaderInfo();
        makeTree();
        printTree(root);

        if (compressedBits.size() != toRead) {
            std::cerr << "ERROR: Expected " << toRead << " bits, but got " << compressedBits.size() << std::endl;
        }

        outputFile(decode());
        std::cerr << "Decompression finished successfully" << std::endl;
    }

    ~Decompressor() {
        deleteTree(root);
        std::cerr << "Huffman tree deleted successfully" << std::endl;
    }

private:
    std::string fileName;
    std::map<wchar_t, std::string> huffmanTable;
    int unique = 0;
    uint32_t toRead = 0;
    std::vector<bool> compressedBits;
    Node* root;
};
