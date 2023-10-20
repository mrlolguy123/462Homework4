// USES TEXT FILES stopwords.txt AND robotech.txt
// stopwords.txt    https://psu.instructure.com/courses/2261958/files/154599906
// robotech.txt     http://www.textfiles.com/stories/robotech

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <map>

class Node {
public:
    int freq;
    char ch;
    Node* left;
    Node* right;

    Node(int freq, char ch) : freq(freq), ch(ch), left(nullptr), right(nullptr) {}

    friend std::ostream &operator<<(std::ostream &os, const Node &node) {
        os << "freq: " << node.freq << " ch: " << node.ch << " left: " << node.left << " right: " << node.right;
        return os;
    }
};

int freq[26] = {0};
std::vector<std::string> stopWords;

std::map<char, std::string> huffman;
std::vector<Node*> tree;

void stopWordsLoad()
{
    std::string inputLine;
    std::ifstream file("stopwords.txt");
    while(std::getline(file, inputLine))
        stopWords.push_back(inputLine);
}

bool isStop(std::string& word)
{
    std::vector<std::string>& stop = stopWords;
    for(const std::string& iterator : stop)
    {
        if (iterator == word)
            return false;
    }
    return true;
}

void freqCheck(const std::string& url)
{
    std::vector<std::string>& stop = stopWords;
    std::string line, word;
    std::ifstream file(url);
    while(std::getline(file, line))
    {
        line.erase(std::remove_if(line.begin(), line.end(), ::ispunct), line.end());
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        std::istringstream iss(line);
        while(iss >> word)
            if(!isStop(word))
                for(char c : word)
                    if (c >= 'a' && c <= 'z')
                        freq[c - 'a']++;

    }
}

void huffmanGen()
{
    for(int x = 0; x < 26; x++)
        if(freq[x])
            tree.push_back(new Node(freq[x], static_cast<char>(x+'a')));

    while(tree.size() > 1) {
        std::sort(tree.begin(), tree.end(), [](Node *a, Node *b) { return a->freq > b->freq; });

        Node *left = tree.back();
        tree.pop_back();

        Node *right = tree.back();
        tree.pop_back();

        Node *newNode = new Node(left->freq + right->freq, '-');
        newNode->left = left;
        newNode->right = right;
        tree.push_back(newNode);
    }
}

void binaryCodes(Node* node, const std::string& code = "")
{
    if(node == nullptr)
        return;

    if (node->left == nullptr && node->right == nullptr)
        huffman[node->ch] = code;

    binaryCodes(node->left, code + "0");
    binaryCodes(node->right, code + "1");
}

void printHuffman(Node* node, const std::string pre = "", bool left = false)
{
    if(node == nullptr)
        return;

    std::string nodeChild = (left) ? "L: " : "R: ";

    std::string binary = (node->ch == '-') ? "" : huffman[node->ch];

    if (node->ch == '-')
        std::cout << pre << "*-- " << nodeChild << node->freq << std::endl;
    else
        std::cout << pre << "*-- " << nodeChild << node->ch << " Frequency: " << node->freq <<" Binary: " << binary << std::endl;

    printHuffman(node->left, pre + (left ? "|\t" : "\t"), true);
    printHuffman(node->right, pre + (left ? "|\t" : "\t"), false);
}

std::string encode(const std::string& message) {
    std::string encoded;

    for (char ch : message)
        if (huffman.count(ch))
            encoded += huffman[ch];
    return encoded;
}

std::string decode(Node* node, const std::string& message) {
    std::string decoded;

    for(int i = 0; i < message.size();)
    {
        Node* currentNode = node;
        while(currentNode->left != nullptr || currentNode->right != nullptr)
            currentNode = (message[i++] == '0') ? currentNode->left : currentNode->right;
        decoded += currentNode->ch;
    }

    return decoded;
}

int main() {
    stopWordsLoad();
    freqCheck("robotech.txt");

    for(int x = 0; x < 26; x++)
        std::cout << static_cast<char>(x+'a') <<  ": " << freq[x] << std::endl;
    std::cout << std::endl;

    huffmanGen();
    Node* node = tree.back();
    binaryCodes(node);
    printHuffman(node);

    std::cout << "type something in lowercase:";
    std::string input;
    std::cin >> input;


    std::cout << "encoded: " << encode(input) << std::endl;
    std::cout << "decoded: " << decode(node, encode(input)) << std::endl;
}
