#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <locale>
#include <codecvt>

struct Node {
    wchar_t character;  
    int frequency;      
    Node* left;         
    Node* right;        

    Node(wchar_t c, int f) : character(c), frequency(f), left(nullptr), right(nullptr) {}
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
    Compressor(std::string fileName) {
        this->fileName = fileName;
    }
    void countFreq() {
        std::ifstream file(fileName, std::ios::binary);
        if (!file) {
            std::cerr << "Error opening file: " << fileName << std::endl;
            return;
        }
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wide_content = converter.from_bytes(content);

        if (!wide_content.empty() && wide_content[0] == 0xFEFF) {
            wide_content.erase(0, 1);
        }

        for (wchar_t letter : wide_content) {
            freqCount[letter]++;
        }
    }


    void heapify() {
        for (const auto& entry : freqCount) {
            minHeap.push(new Node(entry.first, entry.second));
        }
    } //first push all leafs into heap

    void makeTree(){
        while (minHeap.size() > 1){
            Node* left = minHeap.top(); minHeap.pop();
            Node* right = minHeap.top(); minHeap.pop();

            Node* parent = new Node(left->frequency + right->frequency, left, right);
            minHeap.push(parent);
        }
        root = minHeap.top();
    } 

    ~Compressor() {
        deleteTree(root);
    }

private:
    std::string fileName;
    std::unordered_map<wchar_t, int> freqCount;
    std::priority_queue<Node*, std::vector<Node*>, Node::Compare> minHeap;
    Node* root = nullptr;
    void deleteTree(Node* node) {
        if (node) {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
        }
    }
};
