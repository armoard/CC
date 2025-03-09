#ifndef __COMMON_H
#define __COMMON_H
struct Node {
    wchar_t character;  
    int frequency;
    Node* left;
    Node* right;

    Node() : character('\0'), left(nullptr), right(nullptr) {}
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

void deleteTree(Node* node) {
    if (node) {
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
}
#endif