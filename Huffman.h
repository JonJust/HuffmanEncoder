/*
Name: Jonathan Just
Date: 9/17/2020
Class: EECS 2510, Non-Linear Data Structures
Professor: Dr. Lawrence Thomas

Huffman.h

The header for Huffman.cpp

*/

#include "HNode.h"
#pragma once

using namespace std;

 class Huffman
{
public:

	Huffman(); // Constructor
	~Huffman(); // Destructor

	void makeTreeBuilder(string inputFile, string outputFile = "");
	void encodeFile(string inputFile, string outputFile = "");
	void decodeFile(string inputFile, string outputFile);
	void encodeFileWithTree(string inputFile, string treeFile, string outputFile = "");

private:

	void countChar(string inputFile); // Updates charCounts[] based on input file
	void initTree(string inputFile); // Builds huffman tree based on node weights
	void rebuildTree(string inputFile); // Rebuilds tree from 510-byte string
	void buildCipher(string path = "", HNode* traverse  = nullptr); // Acquires the char path codes from the tree
	void writeCodeToFile(string inputFile, string outputFile); // Called by encodeFile and encodeFileWithTree to output code to file
	void rebuildPairOrder(string inputFile); // rebuilds pairOrder[] based on the first 510 bytes of target file.
	void tearDown(HNode* traverse = nullptr);
	HNode* getMinNode(HNode* exclude = nullptr);

	HNode* leaves[256]; // Leaf for every possible character
	string charCipher[256];
	unsigned char pairOrder[510]; // Used to keep track of how the nodes are paired. (Saved as a header to file.huff)
	HNode* root;

};

