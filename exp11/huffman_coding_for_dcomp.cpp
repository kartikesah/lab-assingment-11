#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <vector>

// Node representation for Huffman Tree
struct Node {
    char ch;
    int freq;
    Node *left;
    Node *right;

    Node(char character, int frequency) {
        ch = character;
        freq = frequency;
        left = nullptr;
        right = nullptr;
    }

    Node(char character, int frequency, Node* l, Node* r) {
        ch = character;
        freq = frequency;
        left = l;
        right = r;
    }
};

// Comparator for the min-priority queue
struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

// Traverse Huffman Tree to generate prefix codes
void generateCodes(Node* root, const std::string& code, std::unordered_map<char, std::string>& huffmanCode) {
    if (!root) return;

    // Found a leaf node
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = code;
    }

    generateCodes(root->left, code + "0", huffmanCode);
    generateCodes(root->right, code + "1", huffmanCode);
}

// Decode the encoded bitstring using the Huffman Tree
std::string decode(Node* root, const std::string& encodedStr) {
    std::string decodedStr = "";
    Node* curr = root;

    // Edge case: single distinct character in tree
    if (!root->left && !root->right) {
        return std::string(encodedStr.length(), root->ch);
    }

    for (char bit : encodedStr) {
        if (bit == '0') {
            curr = curr->left;
        } else {
            curr = curr->right;
        }

        // Reached leaf node
        if (!curr->left && !curr->right) {
            decodedStr += curr->ch;
            curr = root;
        }
    }

    return decodedStr;
}

// Free allocated memory for the tree
void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

void buildHuffmanTree(const std::string& text) {
    if (text.empty()) {
        std::cout << "Input text is empty.\n";
        return;
    }

    // Step 1: Count frequency of each character
    std::unordered_map<char, int> freqMap;
    for (char ch : text) {
        freqMap[ch]++;
    }

    // Step 2: Push all leaf nodes into a min-priority queue
    std::priority_queue<Node*, std::vector<Node*>, Compare> minHeap;
    for (auto pair : freqMap) {
        minHeap.push(new Node(pair.first, pair.second));
    }

    // Special case: input text contains only one unique character
    if (minHeap.size() == 1) {
        Node* singleNode = minHeap.top();
        std::cout << "Huffman Codes:\n";
        std::cout << "'" << singleNode->ch << "' : 0\n\n";
        
        std::string encodedStr(singleNode->freq, '0');
        std::cout << "Original String: " << text << "\n";
        std::cout << "Encoded String:  " << encodedStr << "\n";
        std::cout << "Decoded String:  " << decode(singleNode, encodedStr) << "\n";
        delete singleNode;
        return;
    }

    // Step 3: Combine two lowest frequency nodes until one root remains
    while (minHeap.size() > 1) {
        Node* left = minHeap.top(); minHeap.pop();
        Node* right = minHeap.top(); minHeap.pop();

        int sumFreq = left->freq + right->freq;
        Node* internalNode = new Node('\0', sumFreq, left, right);
        minHeap.push(internalNode);
    }

    Node* root = minHeap.top();

    // Step 4: Traverse tree and assign codes (Left = '0', Right = '1')
    std::unordered_map<char, std::string> huffmanCode;
    generateCodes(root, "", huffmanCode);

    std::cout << "--- Huffman Codes ---\n";
    for (auto pair : huffmanCode) {
        std::cout << "'" << pair.first << "' : " << pair.second << "\n";
    }

    // Step 5: Encode the input string
    std::string encodedStr = "";
    for (char ch : text) {
        encodedStr += huffmanCode[ch];
    }

    std::cout << "\nOriginal Text: " << text << "\n";
    std::cout << "Encoded Bits:  " << encodedStr << "\n";

    // Step 6: Decode the encoded string back to original
    std::string decodedStr = decode(root, encodedStr);
    std::cout << "Decoded Text:  " << decodedStr << "\n";

    // Calculate space savings (assuming 8 bits per char in uncompressed format)
    int originalBits = text.length() * 8;
    int compressedBits = encodedStr.length();
    std::cout << "\nOriginal Size:   " << originalBits << " bits\n";
    std::cout << "Compressed Size: " << compressedBits << " bits\n";
    std::cout << "Space Reduction: " << (100.0 * (originalBits - compressedBits) / originalBits) << "%\n";

    // Clean up memory
    freeTree(root);
}

int main() {
    std::string text = "huffman coding compression algorithm";
    buildHuffmanTree(text);
    return 0;
}