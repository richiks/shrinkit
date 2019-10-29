#include "encoding.h"
#include "mymap.h"
#include "pqueue.h"
#include "filelib.h"

/** Declaration for used constants */

const int EOF_MARKER = -1;
const int INCREMENT_FREQUENCY = 1;

/** Helper function declarations */

void buildEncodingMapHelper(HuffmanNode* currentNode, string &binaryRep, Map<int, string>& encodingMap);

/** @brief buildFrequencyTable - This function builds the frequency table by reading the input stream character by character.
 *  When the end of file is reached, EOF character with frequency 1 is added to the frequency table.
 *  @param input - Reference to input stream
 */

MyMap buildFrequencyTable(istream& input) {
    MyMap freqencyTable;

    while(true) {
        int character = input.get();

        if (character == EOF_MARKER) {
            break;
        } else {
            freqencyTable.put(character, INCREMENT_FREQUENCY);
        }
    }
    freqencyTable.put(PSEUDO_EOF,INCREMENT_FREQUENCY);
    return freqencyTable;
}

/** @brief buildEncodingTree - This function builds the encoding tree using a priority queue of HuffmanNode*. All the characters keys along with their corresponding frequencies
 *  are enqueued in the priority queue in their order of priority. Then, the two nodes with the highest priority are dequeued, merged and then enqueued back.
 *  @param freqTable - reference to the map of frequency table
 */

HuffmanNode* buildEncodingTree(const MyMap& freqTable) {
    PriorityQueue<HuffmanNode*> priorQueue;
    Vector<int> keys = freqTable.keys();

    for(int character: keys) {
        HuffmanNode* newNode = new HuffmanNode;
                newNode->character = character;
                newNode->count = freqTable.get(character);
                priorQueue.enqueue(newNode, freqTable.get(character));
    }
    while (priorQueue.size() >= 2) {
        HuffmanNode *highestPriorityNode = priorQueue.dequeue();
        HuffmanNode *secondHighestPriorityNode = priorQueue.dequeue();
        HuffmanNode *combined = new HuffmanNode(NOT_A_CHAR, highestPriorityNode->count + secondHighestPriorityNode->count, highestPriorityNode, secondHighestPriorityNode);
        priorQueue.enqueue(combined, combined->count);
    }

    return priorQueue.dequeue();
}

/** @brief buildEncodingMap - This function builds and returns the encoding map using the encoding tree. This function calls buildEncodingMapHelper function internally.
 *  @param encodingTree - Pointer to the root node of encoding tree.
 */

Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;
    string binaryRep = "";
    buildEncodingMapHelper(encodingTree, binaryRep, encodingMap);
    return encodingMap;
}

/** @brief buildEncodingMapHelper - This function traverses the encoding tree recursively and adds the changes to the string binaryRep .
 *  @param currentNode - Pointer to the root node of the encoding tree
 *  @param binaryRep - Reference to the string binaryRep
 *  @param encodingMap - Reference to the encoding map
 */

void buildEncodingMapHelper(HuffmanNode* currentNode, string &binaryRep, Map<int, string> &encodingMap) {
    if (currentNode->isLeaf()) {
        encodingMap[currentNode->character] = binaryRep;
    }
    else {
        string left = binaryRep + integerToString(0);
        string right = binaryRep + integerToString(1);
        buildEncodingMapHelper(currentNode->zero, left , encodingMap);
        buildEncodingMapHelper(currentNode->one, right, encodingMap);
    }
}

/** @brief encodeData - This function reads the input stream character by character and writes 1s or 0s to the output stream
 *  @param input - Reference to input stream
 *  @param encodingMap - Reference to encoding map built by the previous function.
 *  @param output - Reference to the output stream
 */

void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
    while (true) {
        int character = input.get();

        if (character == EOF_MARKER) {
            break;
        }
        string characterCode = encodingMap[character];
        for (char c : characterCode) {
            if(c == '1') {
                output.writeBit(1);
            } else {
                output.writeBit(0);
            }
        }
    }
    string eofCode = encodingMap[PSEUDO_EOF];
    for (char c : eofCode) {
        if(c == '1') {
            output.writeBit(1);
        } else {
            output.writeBit(0);
        }
    }
}

/**
 * @brief decodeData -  This function decodes the input by traversing left or right down the encoding tree depending upon the value of bit(obtained by reading the input stream).
 * @param input - Reference to input stream
 * @param encodingTree - Pointer to the root node of encoding tree
 * @param output - Reference to the output stream
 */

void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
    HuffmanNode* currentNode = encodingTree;
    while (true) {
        if (currentNode->isLeaf()) {
            if (currentNode->character == PSEUDO_EOF) {
                break;
            }
            output.put(currentNode->character);
            currentNode = encodingTree;
        }
        int bit = input.readBit();
        if (input.fail()) {
            break;
        }
        else if (!bit) {
            currentNode = currentNode->zero;
        }
        else {
            currentNode = currentNode->one;
        }
    }
}

/**
 * @brief compress - Compresses the input
 */

void compress(istream& input, obitstream& output) {
    MyMap freqTable = buildFrequencyTable(input);
    HuffmanNode* rootNode = buildEncodingTree(freqTable);
    Map<int, string> encodingMap = buildEncodingMap(rootNode);

    rewindStream(input);
    output << freqTable;
    encodeData(input, encodingMap, output);
}

/**
 * @brief decompress - Decompresses the input
 */

void decompress(ibitstream& input, ostream& output) {
    MyMap freqTable;
    input >> freqTable;
    HuffmanNode* rootNode = buildEncodingTree(freqTable);
    decodeData(input, rootNode, output);
}

/**
 * @brief freeTree - Deletes the dynamically allocated memory by traversing through the tree
 * @param node - Pointer to the root node of encoding tree
 */

void freeTree(HuffmanNode* node) {
    if (!node->isLeaf()) {
        freeTree(node->zero);
        freeTree(node->one);
    }
    delete node;
}


