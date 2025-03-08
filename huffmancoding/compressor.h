#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <bitset>
#include <locale>
#include <codecvt>

struct Node {
    wchar_t character;  
    int frequency;
    Node* left;
    Node* right;

    Node(wchar_t ch, int f) : character(ch), frequency(f), left(nullptr), right(nullptr) {}
    Node(int f, Node* l, Node* r) : character(L'\0'), frequency(f), left(l), right(r) {}

    struct Compare {
        bool operator()(const Node* a, const Node* b) {
            return a->frequency > b->frequency;
        }
    };

    bool isLeaf() {
        return left == nullptr && right == nullptr;
    }
};

class Compressor {
public:
    explicit Compressor(std::string fileName) : fileName(std::move(fileName)) {}

    void countFreq() {
        std::wifstream file(fileName, std::ios::binary);
        if (!file) {
            std::cerr << "Error opening file: " << fileName << std::endl;
            return;
        }

        file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>));

        wchar_t ch;
        while (file.get(ch)) {
            freqCount[ch]++;
        }

        file.close();
    }

    void buildTree() {
        std::priority_queue<Node*, std::vector<Node*>, Node::Compare> minHeap;

        for (const auto& entry : freqCount) {
            minHeap.push(new Node(entry.first, entry.second));
        }

        while (minHeap.size() > 1) {
            Node* left = minHeap.top(); minHeap.pop();
            Node* right = minHeap.top(); minHeap.pop();

            Node* parent = new Node(left->frequency + right->frequency, left, right);
            minHeap.push(parent);
        }
        root = minHeap.top();
    }

    void generateCodes(Node* node, const std::string& code) {
        if (!node) return;
        if (node->isLeaf()) {
            huffmanCodes[node->character] = code;
        }
        generateCodes(node->left, code + "0");
        generateCodes(node->right, code + "1");
    }

    void writeHuffmanTable(std::ofstream& outFile) {
        outFile << huffmanCodes.size() << "\n"; //unique chars
        for (const auto& entry : huffmanCodes) {
            outFile << static_cast<int>(entry.first) << " " << entry.second << "\n";  
        }
    }

    void encodeFile(const std::string& outputFileName) {
        std::wifstream inputFile(fileName, std::ios::binary);
        std::ofstream outputFile(outputFileName, std::ios::binary);
    
        if (!inputFile || !outputFile) {
            return;
        }
    
        inputFile.imbue(std::locale(inputFile.getloc(), new std::codecvt_utf8<wchar_t>));
        

        writeHuffmanTable(outputFile);
    
        std::string bitString;
        wchar_t ch;
    
        while (inputFile.get(ch)) {
            bitString += huffmanCodes[ch];  
        }
        
        // write in file the exact number of bits before compressing
        uint32_t bitCount = bitString.size();
        outputFile.write(reinterpret_cast<const char*>(&bitCount), sizeof(bitCount));
    
        
        while (bitString.size() % 8 != 0) {
            bitString += "0";   
        }
    
        char buffer = 0;    
        int bitsWritten = 0;

        // start making bytes from bits 
        for (char bit : bitString) {
            buffer <<= 1;
            if (bit == '1') buffer |= 1;
            bitsWritten++;
    
            if (bitsWritten == 8) {
                outputFile.put(buffer);  
                buffer = 0;  
                bitsWritten = 0;
            }
        }
        
        // fill out if there is left out bits
        if (bitsWritten > 0) {
            buffer <<= (8 - bitsWritten);  
            outputFile.put(buffer);
        }
    
        inputFile.close();
        outputFile.close();
    }

    void compress(const std::string& outputFileName) {
        countFreq();
        buildTree();
        generateCodes(root, "");
        encodeFile(outputFileName);
    }

    ~Compressor() {
        deleteTree(root);
    }

private:
    std::string fileName;
    std::map<wchar_t, int> freqCount;
    std::map<wchar_t, std::string> huffmanCodes;
    Node* root = nullptr;

    void deleteTree(Node* node) {
        if (node) {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
        }
    }
};
