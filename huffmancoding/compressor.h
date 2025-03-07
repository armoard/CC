#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <locale>
#include <codecvt>

struct Node {
    std::wstring character;
    int frequency;
    Node(std::wstring c, int f) : character(c), frequency(f) {}

    struct Compare {
        bool operator()(const Node* a, const Node* b) {
            return a->frequency > b->frequency;
        }
    };
};

class Compressor {
public:
    Compressor(std::string fileName) {
        this->fileName = fileName;
    }

    void countFreq() {
        std::ifstream file(fileName, std::ios::binary);
        if (!file) {
            std::cerr << "Error opening file: " << fileName << std::endl;
            return;
        }

        // Leer el archivo como bytes
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        // Convertir de UTF-8 a wstring
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wide_content = converter.from_bytes(content);

        // Si hay un BOM (FEFF), ignorarlo
        if (!wide_content.empty() && wide_content[0] == 0xFEFF) {
            wide_content.erase(0, 1);
        }

        // Contar la frecuencia de cada carácter
        for (wchar_t letter : wide_content) {
            freqCount[std::wstring(1, letter)]++;
        }
    }

    void heapify() {
        for (const auto& entry : freqCount) {
            minHeap.push(new Node(entry.first, entry.second));
        }
    }

    void printHeap() {
        auto tempHeap = minHeap;
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

        while (!tempHeap.empty()) {
            Node* node = tempHeap.top();
            tempHeap.pop();

            std::string charDisplay;
            wchar_t wch = node->character[0];

            if (wch == L' ') charDisplay = "[SPACE]";
            else if (wch == L'\n') charDisplay = "[NEWLINE]";
            else if (wch == L'\r') charDisplay = "[CARRIAGE RETURN]";
            else if (wch < 32 || wch == 127)
                charDisplay = "[CTRL-" + std::to_string(wch) + "]";
            else
                charDisplay = converter.to_bytes(node->character);

            std::cout << "Char: " << charDisplay
                      << " (UTF-16: " << std::hex << wch << std::dec
                      << "), Freq: " << node->frequency << std::endl;
        }
    }

    ~Compressor() {
        while (!minHeap.empty()) {
            delete minHeap.top();
            minHeap.pop();
        }
    }

private:
    std::string fileName;
    std::unordered_map<std::wstring, int> freqCount;
    std::priority_queue<Node*, std::vector<Node*>, Node::Compare> minHeap;
};
