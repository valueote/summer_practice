#include "bits.h"
#include "treenode.h"
#include "huffman.h"
#include "map.h"
#include "vector.h"
#include "priorityqueue.h"
#include "strlib.h"
#include "testing/SimpleTest.h"
using namespace std;

/**
 * Given a Queue<Bit> containing the compressed message bits and the encoding tree
 * used to encode those bits, decode the bits back to the original message text.
 *
 * You can assume that tree is a well-formed non-empty encoding tree and
 * messageBits queue contains a valid sequence of encoded bits.
 *
 * Your implementation may change the messageBits queue however you like. There
 * are no requirements about what it should look like after this function
 * returns. The encoding tree should be unchanged.
 *
 * Use a while loop to traversal the tree and move in terms of the bits, if we get a leaf, then we find
 * a character and add it to the result.
 */
string decodeText(EncodingTreeNode* tree, Queue<Bit>& messageBits) {
    /* TODO: Implement this function. */
    string text;
    EncodingTreeNode* cur = tree;

    while(!messageBits.isEmpty()){
        Bit bit = messageBits.dequeue();

        if(bit == 0){
            cur = cur->zero;
        }else{
            cur = cur->one;
        }

        if(cur->isLeaf()){
            text += cur->ch;
            cur = tree;
        }
    }
    return text;
}

/**
 * Reconstruct encoding tree from flattened form Queue<Bit> and Queue<char>.
 *
 * You can assume that the queues are well-formed and represent
 * a valid encoding tree.
 *
 * Your implementation may change the queue parameters however you like. There
 * are no requirements about what they should look like after this function
 * returns.
 *
 * Build the tree recursively,if the current num is 0, stop and return a node with character
 * otherwise create a interior node and build its left and right.
 */
EncodingTreeNode* unflattenTree(Queue<Bit>& treeShape, Queue<char>& treeLeaves) {
    /* TODO: Implement this function. */
    EncodingTreeNode* root = new EncodingTreeNode(nullptr,nullptr);

    Bit bit = treeShape.dequeue();
    if(bit == 0){
        root->ch = treeLeaves.dequeue();
        return root;
    }


    EncodingTreeNode* left = unflattenTree(treeShape,treeLeaves);
    EncodingTreeNode* right = unflattenTree(treeShape,treeLeaves);
    root->zero = left;
    root->one = right;




    return root;
}

/**
 * Decompress the given EncodedData and return the original text.
 *
 * You can assume the input data is well-formed and was created by a correct
 * implementation of compress.
 *
 * Your implementation may change the data parameter however you like. There
 * are no requirements about what it should look like after this function
 * returns.
 *
 * Call the unfalttenTree function to get a tree and call decodeText to turn it into string
 *
 */
string decompress(EncodedData& data) {
    /* TODO: Implement this function. */
    EncodingTreeNode* tree = unflattenTree(data.treeShape,data.treeLeaves);
    string text = decodeText(tree,data.messageBits);
    deallocateTree(tree);

    return text;
}

/**
 * Constructs an optimal Huffman coding tree for the given text, using
 * the algorithm described in lecture.
 *
 * Reports an error if the input text does not contain at least
 * two distinct characters.
 *
 * When assembling larger trees out of smaller ones, make sure to set the first
 * tree dequeued from the queue to be the zero subtree of the new tree and the
 * second tree as the one subtree.
 *
 * Use huffman algorithm to build an optimal tree in light of the input text
 *
 */
EncodingTreeNode* buildHuffmanTree(string text) {
    Map<char,int> frequency;
    for(char i : text){
        frequency[i]++;
    }

    PriorityQueue<EncodingTreeNode*> pq;
    for(char i : text){
        if(frequency.containsKey(i)){
            EncodingTreeNode* node = new EncodingTreeNode(i);
            pq.enqueue(node,frequency.get(i));
            frequency.remove(i);
        }
    }

    while(pq.size() != 1){
        int priority = pq.peekPriority();
        EncodingTreeNode* one = pq.dequeue();
        priority += pq.peekPriority();
        EncodingTreeNode* zero = pq.dequeue();

        EncodingTreeNode* newNode = new EncodingTreeNode(one,zero);
        pq.enqueue(newNode,priority);

    }

    return pq.dequeue();
}

/**
 * Given a string and an encoding tree, encode the text using the tree
 * and return a Queue<Bit> of the encoded bit sequence.
 *
 * You can assume tree is a valid non-empty encoding tree and contains an
 * encoding for every character in the text.
 *
 * Traverse the tree and build a map that associates each character with its
 *  encoded bit sequence. Then return the corresponding bit sequence  in the light of
 *  the input text.
 *
 */

// A helper function to traverse the tree and get the bit queue of every character.
void traverseTree(EncodingTreeNode* tree, Map<char,Queue<Bit>>& m, Queue<Bit> line){
    if(tree->isLeaf()){
        m.put(tree->ch,line);
        return;
    }
    Queue<Bit> copy = line;
    line.enqueue(0);
    copy.enqueue(1);
    traverseTree(tree->zero,m,line);
    traverseTree(tree->one,m,copy);

}


Queue<Bit> encodeText(EncodingTreeNode* tree, string text) {
    /* TODO: Implement this function. */

    Map<char,Queue<Bit>> m;
    Queue<Bit> result;
    traverseTree(tree,m, result);
    for(char c : text){
        Queue<Bit> q = m.get(c);
        while(!q.isEmpty()){
            result.enqueue(q.dequeue());

        }
    }
    return result;

}

/**
 * Flatten the given tree into a Queue<Bit> and Queue<char> in the manner
 * specified in the assignment writeup.
 *
 * You can assume the input queues are empty on entry to this function.
 *
 * You can assume tree is a valid well-formed encoding tree.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
void flattenTree(EncodingTreeNode* tree, Queue<Bit>& treeShape, Queue<char>& treeLeaves) {
    /* TODO: Implement this function. */
    if(tree->isLeaf()){
        treeShape.enqueue(0);
        treeLeaves.enqueue(tree->ch);
    }else{
        treeShape.enqueue(1);
        flattenTree(tree->zero, treeShape, treeLeaves);
        flattenTree(tree->one, treeShape, treeLeaves);
    }


}

/**
 * Compress the input text using Huffman coding, producing as output
 * an EncodedData containing the encoded message and flattened
 * encoding tree used.
 *
 * Reports an error if the message text does not contain at least
 * two distinct characters.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
EncodedData compress(string messageText) {
    EncodedData result;
    Queue<Bit> shape;
    Queue<char> leaves;

    EncodingTreeNode* tree = buildHuffmanTree(messageText);
    Queue<Bit> bitSequence = encodeText(tree, messageText);
    flattenTree(tree,shape,leaves);
    result.messageBits = bitSequence;
    result.treeShape = shape;
    result.treeLeaves = leaves;

    deallocateTree(tree);
    return result;
}

/* * * * * * Testing Helper Functions Below This Point * * * * * */

EncodingTreeNode* createExampleTree() {
    /* Example encoding tree used in multiple test cases:
     *                *
     *              /   \
     *             T     *
     *                  / \
     *                 *   E
     *                / \
     *               R   S
     */
    EncodingTreeNode* nodeR = new EncodingTreeNode('R');
    EncodingTreeNode* nodeS = new EncodingTreeNode('S');
    EncodingTreeNode* nodeE = new EncodingTreeNode('E');
    EncodingTreeNode* nodeT = new EncodingTreeNode('T');

    EncodingTreeNode* nodeOne = new EncodingTreeNode(nodeR,nodeS);
    EncodingTreeNode* nodeTwo = new EncodingTreeNode(nodeOne,nodeE);
    EncodingTreeNode* nodeThree = new EncodingTreeNode(nodeT,nodeTwo);

    return nodeThree;
}

EncodingTreeNode* createTestTree() {

    EncodingTreeNode* nodeA = new EncodingTreeNode('A');
    EncodingTreeNode* nodeB = new EncodingTreeNode('B');
    EncodingTreeNode* nodeC = new EncodingTreeNode('C');
    EncodingTreeNode* nodeD = new EncodingTreeNode('D');
    EncodingTreeNode* nodeE = new EncodingTreeNode('E');
    EncodingTreeNode* nodeF = new EncodingTreeNode('F');


    EncodingTreeNode* nodeOne = new EncodingTreeNode(nodeA,nodeB);
    EncodingTreeNode* nodeTwo = new EncodingTreeNode(nodeOne,nodeC);
    EncodingTreeNode* nodeThree = new EncodingTreeNode(nodeD,nodeE);
    EncodingTreeNode* nodeFour = new EncodingTreeNode(nodeTwo,nodeThree);
    EncodingTreeNode* nodeFive = new EncodingTreeNode(nodeFour,nodeF);

    return nodeFive;
}


void deallocateTree(EncodingTreeNode* t) {
    if(t->isLeaf()){
        delete t;
    }else{
        deallocateTree(t->zero);
        deallocateTree(t->one);
        delete t;
    }
}

bool areEqual(EncodingTreeNode* a, EncodingTreeNode* b) {
    if(a == nullptr || b == nullptr){
        return a == b;
    }else if(a->isLeaf() && !b->isLeaf()){
        return false;
    }else if(!a->isLeaf() && b->isLeaf()){
        return false;
    }else if(a->isLeaf() && b->isLeaf()){
        return a->ch == b->ch;
    }else{
        return areEqual(a->zero,b->zero) && areEqual(a->one,b->one);
    }

}

/* * * * * * Test Cases Below This Point * * * * * */

STUDENT_TEST("Test the createExampleTree and deallocateTree"){
    EncodingTreeNode* t = createExampleTree();
    deallocateTree(t);

}

STUDENT_TEST("Test the areEqual function"){
    EncodingTreeNode* t1 = new EncodingTreeNode('A');
    EncodingTreeNode* t2 = new EncodingTreeNode('B');
    EncodingTreeNode* t3 = createExampleTree();
    EncodingTreeNode* t4 = createExampleTree();

    EXPECT(!areEqual(t1,nullptr));
    EXPECT(!areEqual(t1,t2));
    EXPECT(!areEqual(t1,t3));
    EXPECT(areEqual(t4,t3));
    EXPECT(!areEqual(t3->one,t3));

    deallocateTree(t1);
    deallocateTree(t2);
    deallocateTree(t3);
    deallocateTree(t4);
}

STUDENT_TEST("Test the decodeText function"){
    EncodingTreeNode* tree = createTestTree();

    Queue<Bit> messageBits = { 1 }; // F
    EXPECT_EQUAL(decodeText(tree, messageBits), "F");

    messageBits = { 1, 1 }; // FF
    EXPECT_EQUAL(decodeText(tree, messageBits), "FF");

    messageBits = { 1, 0, 0, 1, 0, 1, 1 }; // FCE
    EXPECT_EQUAL(decodeText(tree, messageBits), "FCE");

    messageBits = { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1 }; // CADBF
    EXPECT_EQUAL(decodeText(tree, messageBits), "CADBF");



    deallocateTree(tree);
}

STUDENT_TEST("Test the unflattenTree function with middle size tree"){
    EncodingTreeNode* reference = createTestTree();
    Queue<Bit>  treeShape  = { 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0 };
    Queue<char> treeLeaves = { 'A', 'B', 'C', 'D', 'E', 'F' };
    EncodingTreeNode* tree = unflattenTree(treeShape, treeLeaves);

    EXPECT(areEqual(tree, reference));

    deallocateTree(tree);
    deallocateTree(reference);
}

STUDENT_TEST("Test decompress with middle tree") {
    EncodedData data = {
        { 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0 }, // treeShape
        { 'A', 'B', 'C', 'D', 'E', 'F' },  // treeLeaves
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1 } // messageBits
    };

    EXPECT_EQUAL(decompress(data), "BADFE");
}

STUDENT_TEST("Text encodeText with middle encoding tree") {
    EncodingTreeNode* reference = createTestTree(); // see diagram above

    Queue<Bit> messageBits = encodeText(reference, "F"); // F
    EXPECT_EQUAL(decodeText(reference,messageBits), "F");


    messageBits = { 0, 0, 0, 0, 0, 1, 1}; // AE
    EXPECT_EQUAL(decodeText(reference,messageBits), "AE");

    messageBits = { 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1 ,1 }; // BADFE
    EXPECT_EQUAL(decodeText(reference,messageBits),"BADFE");

    deallocateTree(reference);
}

STUDENT_TEST("FlattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree();

    Queue<Bit>  treeShape;
    Queue<char> treeLeaves;
    flattenTree(reference, treeShape, treeLeaves);

    EncodingTreeNode* result = unflattenTree(treeShape,treeLeaves);
    EXPECT(areEqual(reference,result));

    deallocateTree(reference);
    deallocateTree(result);
}

STUDENT_TEST("compress, small example input") {
    EncodedData data = compress("STREETTEST");
    EncodedData dataA = compress("ILOVEFJY");
    EncodedData dataB = compress("ICOMEFROMNKU");

    EXPECT_EQUAL(decompress(data),"STREETTEST");
    EXPECT_EQUAL(decompress(dataA),"STREETTEST");
    EXPECT_EQUAL(decompress(dataB),"STREETTEST");
}
/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("decodeText, small example encoding tree") {
    EncodingTreeNode* tree = createExampleTree(); // see diagram above
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(decodeText(tree, messageBits), "E");

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(decodeText(tree, messageBits), "SET");

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1}; // STREETS
    EXPECT_EQUAL(decodeText(tree, messageBits), "STREETS");

    deallocateTree(tree);
}

PROVIDED_TEST("unflattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  treeShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeLeaves = { 'T', 'R', 'S', 'E' };
    EncodingTreeNode* tree = unflattenTree(treeShape, treeLeaves);

    EXPECT(areEqual(tree, reference));

    deallocateTree(tree);
    deallocateTree(reference);
}

PROVIDED_TEST("decompress, small example input") {
    EncodedData data = {
        { 1, 0, 1, 1, 0, 0, 0 }, // treeShape
        { 'T', 'R', 'S', 'E' },  // treeLeaves
        { 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1 } // messageBits
    };

    EXPECT_EQUAL(decompress(data), "TRESS");
}

PROVIDED_TEST("buildHuffmanTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    EncodingTreeNode* tree = buildHuffmanTree("STREETTEST");
    EXPECT(areEqual(tree, reference));

    deallocateTree(reference);
    deallocateTree(tree);
}

PROVIDED_TEST("encodeText, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(encodeText(reference, "E"), messageBits);

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(encodeText(reference, "SET"), messageBits);

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1 }; // STREETS
    EXPECT_EQUAL(encodeText(reference, "STREETS"), messageBits);

    deallocateTree(reference);
}

PROVIDED_TEST("flattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  expectedShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> expectedLeaves = { 'T', 'R', 'S', 'E' };

    Queue<Bit>  treeShape;
    Queue<char> treeLeaves;
    flattenTree(reference, treeShape, treeLeaves);

    EXPECT_EQUAL(treeShape,  expectedShape);
    EXPECT_EQUAL(treeLeaves, expectedLeaves);

    deallocateTree(reference);
}

PROVIDED_TEST("compress, small example input") {
    EncodedData data = compress("STREETTEST");
    Queue<Bit>  treeShape   = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeChars   = { 'T', 'R', 'S', 'E' };
    Queue<Bit>  messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0 };

    EXPECT_EQUAL(data.treeShape, treeShape);
    EXPECT_EQUAL(data.treeLeaves, treeChars);
    EXPECT_EQUAL(data.messageBits, messageBits);
}

PROVIDED_TEST("Test end-to-end compress -> decompress") {
    Vector<string> inputs = {
        "HAPPY HIP HOP",
        "Nana Nana Nana Nana Nana Nana Nana Nana Batman"
        "Research is formalized curiosity. It is poking and prying with a purpose. – Zora Neale Hurston",
    };

    for (string input: inputs) {
        EncodedData data = compress(input);
        string output = decompress(data);

        EXPECT_EQUAL(input, output);
    }
}
