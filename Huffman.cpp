/*
Name: Jonathan Just
Date: 9/17/2020
Class: EECS 2510, Non-Linear Data Structures
Professor: Dr. Lawrence Thomas

Huffman.cpp

Parses through a text file and encodes it into a compressed .huf file.

This .huf file can then be decoded later by this class. 

For experimental purposes, the class can also create a .htree file that
can be used to encode files based on frequency counts of other files.

===== PUBLIC METHODS ======

Huffman.encodeFile(string inputFile, string outputFile)
	
	- When called, encodes inputFile into outputFile
	- outputFile takes up less disk space than inputFile
	- the output can later be decoded by this class

Huffman.decodeFile(string inputFile, string outputFile)

	- Builds a tree based off of the header of inputFile, and uses it to decode inputFile
	- The user is able to specify the file extension of outputFile
	- Note that inputFile MUST have been encoded using the same algorithm present in this class to work

Huffman.makeTreeBuilder(string inputFile, string outputFile)
	
	- Takes an input file, gets the character counts, builds a tree, and generates an .htree file based on the tree
	- The .htree file can be used by this class to rebuild a tree based on the character frequencies, and encode another file using that tree
	- This method is mostly an academic excercize, as encoding files based on byte frequencies of other files will have lesser compression

Huffman.encodeFileWithTree(string inputFile, string treeFile, string outputFile)

	- Takes an input file and a tree file to encode inputFile based off of the tree in treeFile
	- This can be used to encode files based off the frequency counts of other files
	- Note that this isn't recomended for normal use; compression should always be worse using this

*/

#include "Huffman.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include "SimpleQueue.h"

Huffman::Huffman() // Constructor
{
	for (int i = 0; i < 256; i++) // Set all leaf values
	{
		unsigned char c = i;
		leaves[i] = new HNode;
		leaves[i]->key = c;
		leaves[i]->rPtr = nullptr;
		leaves[i]->lPtr = nullptr;
		leaves[i]->weight = 0;
	}

	for (int i = 0; i < 510; i++) pairOrder[i] = 0x00; // Clear pairOrder array

	for (int i = 0; i < 256; i++) charCipher[i] = ""; // Clear charCipher array

	root = nullptr;

}

Huffman::~Huffman() // Destructor
{
	tearDown(root); // Tears down the tree starting from the root
}

void Huffman::encodeFile(string inputFile, string outputFile)
{
	/* [Public Method]
	* Encodes inputFile.
	* 
	* First, the method passes the input file to countChar to get the frequency count array of each character prepared.
	* 
	* Next, the method calls initTree to build the huffman tree, passing the input file down. Using this tree, it calls buildCipher
	* to get the encoded equivilants of each character into the cipher array before writing to the file.
	* 
	* Finally, writeCodeToFile is called, passing inputFile and outputFile. This parses through the input file again, converting each
	* byte in the file to the proper encoded bit string.
	* 
	*/
	
	// Its ugly to have this here, but needed as the output file name is proccessed in writeCodeToFile()
	if (outputFile == "") // If outputFile is not specified, set its name to the input file w/ extension .huf
	{
		size_t last = inputFile.find_last_of('.');
		outputFile = inputFile.substr(0, last);
		outputFile += ".huf";
	}
	else if (outputFile.find('.') == string::npos) // This happens when the user specifies a file name but not an extension
	{
		outputFile += ".huf";
	}

	auto start = std::chrono::steady_clock::now(); // Used to get total elapsed time

	countChar(inputFile); // Update char weights
	initTree(inputFile); // Builds tree based on char weights
	buildCipher(); // Builds the cipher array so the program can know how to encode each letter
	writeCodeToFile(inputFile, outputFile); // uses the tree and cipher array to encode inputFile

	auto end = std::chrono::steady_clock::now();

	ifstream bytesIn(inputFile, ios::binary | ios::ate); // Finally, output elapsed time and bytes in/out to console
	ifstream bytesOut(outputFile, ios::binary | ios::ate);

	double totalMs = (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()); // gets the total number of elapsed milliseconds and divides it by 1000

	double seconds = totalMs / 1000; // Divide that number by 1000 to get total elapsed seconds

	cout << fixed << setprecision(3) << seconds << " seconds. " << bytesIn.tellg() << " bytes in / " << bytesOut.tellg() << " bytes out" << endl;

	bytesIn.close();
	bytesOut.close();

}

void Huffman::decodeFile(string inputFile, string outputFile)
{
	/* [Public Function]
	* Decodes inputFile, and outputs the decoded file to outputFile.
	* 
	* First, calls rebuildPairOrder to fill pairOrder[] based on inputFile's header.
	* Using pairOrder[], rebuildTree is called to rebuild the huffman tree based on
	* pairOrder[]. Then, buildCipher is called to put the cipher keys into charCipher[]
	* for optimizations. 
	*
	* With the tree and all relevant arrays filled, the function then begins to crawl through
	* inputFile, decoding each bit string into their equivilant char values by traversing down
	* the tree.
	*/

	auto start = std::chrono::steady_clock::now(); // Used to get total elapsed time

	rebuildPairOrder(inputFile);
	rebuildTree(inputFile);
	buildCipher(); // Builds the cipher array so the program can know how to encode each letter

	unsigned char c;

	ifstream input(inputFile, ios::binary);
	ofstream output(outputFile, ios::binary);

	for (int i = 0; i < 510; i++) input.read((char*)&c, sizeof(c)); // Burn through first 510 bytes in file to get to encoded file

	int maxPathLength = charCipher[0].length();
	for (int i = 1; i < 255; i++) if (charCipher[i].length() > maxPathLength) maxPathLength = charCipher[i].length(); // finds the length of the longest path in the cipher array

	int minPathLength = charCipher[0].length();
	for (int i = 1; i < 255; i++) if (charCipher[i].length() < minPathLength) minPathLength = charCipher[i].length(); // finds the length of the shortest path in the cipher array


	HNode* traverse = root;

	while (input.read((char*)&c, sizeof(c)))
	{

		for (int i = 128; i >= 1; i = i / 2) // Starts at 2^7, then goes down to 2^6, 2^5....
		{
			if (c & i) traverse = traverse->rPtr; // If the indexed bit in the read byte is 1, go left down the tree
			else traverse = traverse->lPtr; // else, go right down the tree

			if (traverse->lPtr == nullptr && traverse->rPtr == nullptr) // We hit a leaf! reset traverse and print key to file
			{
				output.write((char*)&traverse->key, sizeof(traverse->key));
				traverse = root;
			}

		}

	}

	input.close(); // Need to close files before exiting
	output.close();

	auto end = std::chrono::steady_clock::now();

	ifstream bytesIn(inputFile, ios::binary | ios::ate); // Finally, output elapsed time and bytes in/out to console
	ifstream bytesOut(outputFile, ios::binary | ios::ate);

	double totalMs = (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()); // gets the total number of elapsed milliseconds and divides it by 1000

	double seconds = totalMs / 1000; // Divide that number by 1000 to get total elapsed seconds

	cout << fixed << setprecision(3) << seconds << " seconds. " << bytesIn.tellg() << " bytes in / " << bytesOut.tellg() << " bytes out" << endl;

	bytesIn.close();
	bytesOut.close();

}

void Huffman::makeTreeBuilder(string inputFile, string outputFile)
{
	/* [Public Method]
	* Creates a tree builder file that can be used to
	* encode a target file with a tree built based on
	* another file.
	*/

	auto start = std::chrono::steady_clock::now(); // Used to get total elapsed time

	countChar(inputFile); // Update char weights
	initTree(inputFile); // Builds tree based on char weights

	if (outputFile == "") // If outputFile is not specified, set its name to the input file w/ extension .huf
	{
		size_t last = inputFile.find_last_of('.');
		outputFile = inputFile.substr(0, last);
		outputFile += ".htree";
	}
	else if (outputFile.find('.') == string::npos) // This happens when the user specifies a file name but not an extension
	{
		outputFile += ".htree";
	}

	ofstream output(outputFile, ios::binary);

	for (int i = 0; i < 510; i++) output.write((char*)&pairOrder[i], sizeof(pairOrder[i])); // Writes the 510-byte header to the output file

	output.close();

	auto end = std::chrono::steady_clock::now();

	ifstream bytesIn(inputFile, ios::binary | ios::ate); // Finally, output elapsed time and bytes in/out to console
	ifstream bytesOut(outputFile, ios::binary | ios::ate);

	double totalMs = (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()); // gets the total number of elapsed milliseconds and divides it by 1000

	double seconds = totalMs / 1000; // Divide that number by 1000 to get total elapsed seconds

	cout << fixed << setprecision(3) << seconds << " seconds. " << bytesIn.tellg() << " bytes in / " << bytesOut.tellg() << " bytes out" << endl;

	bytesIn.close();
	bytesOut.close();

}

void Huffman::encodeFileWithTree(string inputFile, string treeFile, string outputFile)
{
	/* [Public Method]
	* First, initlizes the pair order and tree tree based on treeFile
	* Then, calls buildCipher to get the encoded key paths based on the tree
	* Finally, calls writeCodeToFile() to encode and compress input file into outputFile
	*/

	auto start = std::chrono::steady_clock::now(); // Used to get total elapsed time

	rebuildPairOrder(treeFile); // rebuilds pair order array
	rebuildTree(treeFile); // Builds tree based on char weights
	buildCipher(); // Builds the cipher array so the program can know how to encode each letter
	writeCodeToFile(inputFile, outputFile); // uses the tree and cipher array to encode inputFile

	// Its ugly to have this here, but needed as the output file name is proccessed in writeCodeToFile()
	if (outputFile == "") // If outputFile is not specified, set its name to the input file w/ extension .huf
	{
		size_t last = inputFile.find_last_of('.');
		outputFile = inputFile.substr(0, last);
		outputFile += ".huf";
	}
	else if (outputFile.find('.') == string::npos) // This happens when the user specifies a file name but not an extension
	{
		outputFile += ".huf";
	}

	auto end = std::chrono::steady_clock::now();

	ifstream bytesIn(inputFile, ios::binary | ios::ate); // Finally, output elapsed time and bytes in/out to console
	ifstream bytesOut(outputFile, ios::binary | ios::ate);

	double totalMs = (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()); // gets the total number of elapsed milliseconds and divides it by 1000

	double seconds = totalMs / 1000; // Divide that number by 1000 to get total elapsed seconds

	cout << fixed << setprecision(3) << seconds << " seconds. " << bytesIn.tellg() << " bytes in / " << bytesOut.tellg() << " bytes out" << endl;

	bytesIn.close();
	bytesOut.close();

}

void Huffman::countChar(string inputFile)
{
/* [Private Method]
*  Creates an instance of ifstream to target file.
* 
*  Crawls through the file byte-by-byte and updates the node weights for every instance a byte
*  appears in the file.
* 
*/

	ifstream input(inputFile,ios::binary);

	if (!input.is_open())
	{
		cout << "Unable to open file: " << inputFile << endl;
		exit(0);
		return;
	}

	unsigned char c;

	while (input.read((char*)&c, sizeof(c))) leaves[c]->weight++; // crawls through file and gets the count of each byte

	input.close();
}

HNode* Huffman::getMinNode(HNode* exclude)
{
	/* [Private Method]
	Returns the node with the minimum weight in the array 

	If exclude is passed, this method will return the node with the smallest weight in the array, unless it is equal to exclude.

	*/

	HNode* target = leaves[0];

	if (leaves[0] == exclude)  // If we have passed leaves[0] as the node to exclude, find next nonnullptr node in leaves[] to start loop with
	{
		for (int i = 0; i < 256; i++) if (leaves[i] != nullptr) target = leaves[i];
	}

	for (int i = 0; i < 256; i++) // crawl through leaf array
	{
		if ((leaves[i] != nullptr) && (leaves[i]->weight < target->weight) && (exclude != leaves[i])) target = leaves[i]; // Update target if the next element in array is less than or equal to it

	}

	return target;

}

void Huffman::initTree(string inputFile)
{
	/* [Private Method]
	 IMPORTANT: BEFORE CALLING MAKE SURE TO UPDATE THE CHAR COUNT ARRAY USING countChar()
	
	 Builds the tree that will be used to encode file by pairing nodes together. Lower weighted nodes will be paired first, and
	 then heigher weighted nodes will get paired.
	
	 Since there is at most 256 items in the leaves array, there will be exactly 255 pairs made. Each pairing will be saved to a string
	 that will later be used as a 510-byte tree-builder header.

	 Once built, the global root pointer will be pointed to the proper node: leaves[0].
	
	*/

	int pairIndex = 0;

	for (int i = 0; i < 255; i++) // Build tree
	{
		HNode* min = getMinNode(); // gets least weighted node in the whole array (Biased towards top of array)
		pairOrder[pairIndex] = min->key;
		pairIndex++;

		HNode* secondMin = getMinNode(min); // gets second least weighted node in array
		pairOrder[pairIndex] = secondMin->key;
		pairIndex++;

		HNode* parent = new HNode; // create new parent node
		parent->weight = min->weight + secondMin->weight; // parent node's weight will be equal to the sum of it's children


		if (min->key < secondMin->key)
		{
			parent->lPtr = min; // lower-subscripted nodes go the the left... 
			parent->rPtr = secondMin; // ... And higher subscripted nodes go to the right
			parent->key = min->key; // The parent will inheret its right-childs key. (This is used to keep track of where it will end up in the array)

			leaves[min->key] = parent; // Set minimum node's pointer in array to null
			leaves[secondMin->key] = nullptr; // Set larger node's pointer in array to point to its parent
		}
		else
		{
			parent->lPtr = secondMin; // lower-subscripted nodes go the the left... 
			parent->rPtr = min; // ... And higher subscripted nodes go to the right
			parent->key = secondMin->key; // The parent will inheret its right-childs key. (This is used to keep track of where it will end up in the array)

			leaves[secondMin->key] = parent; // Set minimum node's pointer in array to null
			leaves[min->key] = nullptr; // Set larger node's pointer in array to point to its parent
		}

	}

	root = leaves[0]; // Set the root node as the final node in the array

}

void Huffman::rebuildTree(string inputFile)
{
	/* [Private Method]
	 IMPORTANT: MAKE SURE TO CALL rebuildPairOrder() BEFORE CALLING
	
	 Reuilds the tree that will be used to decode file by pairing nodes together.

	 Once built, the global root pointer will be pointed to the proper node.

	 Functionally similar to initTree, however this method gets the order to pair nodes from the pairOrder array.
	*/

	int pairIndex = 0;

	for (int i = 0; i < 255; i++) // Build tree
	{
		HNode* min = leaves[pairOrder[pairIndex]]; 
		pairIndex++;

		HNode* secondMin = 	leaves[pairOrder[pairIndex]];
		pairIndex++;

		HNode* parent = new HNode; // create new parent node

		if (min->key < secondMin->key)
		{
			parent->lPtr = min; // lower-weighted nodes go the the left... (biased towards end of array)
			parent->rPtr = secondMin; // ... And higher weighted nodes go to the right
			parent->key = min->key; // The parent will inheret its right-childs key. (This is used to keep track of where it will end up in the array)

			leaves[min->key] = parent; // Set minimum node's pointer in array to null
			leaves[secondMin->key] = nullptr; // Set larger node's pointer in array to point to its parent
		}
		else
		{
			parent->lPtr = secondMin; // lower-weighted nodes go the the left... (biased towards end of array)
			parent->rPtr = min; // ... And higher weighted nodes go to the right
			parent->key = secondMin->key; // The parent will inheret its right-childs key. (This is used to keep track of where it will end up in the array)

			leaves[secondMin->key] = parent; // Set minimum node's pointer in array to null
			leaves[min->key] = nullptr; // Set larger node's pointer in array to point to its parent
		}

	}

	root = leaves[0]; // Set root of tree to last standing node

}

void Huffman::rebuildPairOrder(string inputFile)
{
	/* [Private Method]
	* 
	* Crawls through the first 510
	* bytes from a target file and
	* puts it in pairOrder[].
	*/

	ifstream input(inputFile, ios::binary);
	unsigned char c; // will be used to hold byte at indexed location in for loop

	if (!input.is_open())
	{
		cout << "Unable to open file: " << inputFile << endl;
		exit(0);
		return;
	}

	for (int i = 0; i < 510; i++)
	{
		input.read((char*)&c,sizeof(c));
		pairOrder[i] = c; // Writes the 510-byte header to the output file

	}

	input.close();
}

void Huffman::buildCipher(string path, HNode* traverse)
{
/* [Private Method]
 Recursivley traverses the tree, passing down a string to keep track of the path for each node.
 Once the method hits a leaf, it stores the path it took into charCipher[traverse->key].
 More common characters will have shorter paths, while rarer characters will have longer paths.
*/
	if (traverse == nullptr) traverse = root; // If traverse is a nullptr, then this method is being called directly

	if (traverse->lPtr != nullptr)
	{
		string newPath = path + "0";
		buildCipher(newPath, traverse->lPtr);
	}

	if (traverse->rPtr != nullptr)
	{
		string newPath = path + "1";
		buildCipher(newPath, traverse->rPtr);
	}

	// When both children are nullptrs, node is a leaf

	if (traverse->rPtr == nullptr && traverse->lPtr == nullptr) charCipher[traverse->key] = path;

}

void Huffman::writeCodeToFile(string inputFile, string outputFile)
{
	/* [Private Method]
	*  
	* == IMPRORTANT, MAKE SURE THAT countChar(), initTree(), & buildCipher() BEEN CALLED
	* BEFORE CALLING THIS METHOD TO INITIALIZE ALL RELEVANT ARRAYS AND THE TREE ==============
	* 
	* Using the cipher key generated by buildCipher, this method parses through inputFile and generates
	* a bit string to be placed into outputFile. A queue is used to keep track of the order of bits to 
	* be added into outputFile. outputFile will be written to byte-by-byte. If the bit string is not divisible
	* by 8, up to 7 padding 0s will be present at the end of the file. Because the decoder works by traversing
	* down the tree until it finds a node, these bits will be discarded and not effect the decoding.
	*/

	if (outputFile == "") // If outputFile is not specified, set its name to the input file w/ extension .huf
	{
		size_t last = inputFile.find_last_of('.');
		outputFile = inputFile.substr(0, last);
		outputFile += ".huf";
	}
	else if (outputFile.find('.') == string::npos) // This happens when the user specifies a file name but not an extension
	{
		outputFile += ".huf";
	}

	ifstream input(inputFile, ios::binary);
	ofstream output(outputFile, ios::binary);

	for (int i = 0; i < 510; i++) output.write((char*)&pairOrder[i], sizeof(pairOrder[i])); // Writes the 510-byte header to the output file

	if (!input.is_open())
	{
		cout << "Unable to open file: " << inputFile << endl; // File does not exist, so alert the user and exit
		exit(0);
		return;
	}

	unsigned char c;
	string code = "";
	SimpleQueue* queue = new SimpleQueue();

	while (input.read((char*)&c, sizeof(c)))
	{

		code = charCipher[c];

		for (int i = 0; i < code.length(); ++i) // Adds the code string to the queue bit-by-bit
		{

			if (code.at(i) == '0') queue->add('0');
			else queue->add('1');

		}

		// The for loop is here to ensure that items are taken out of the queue faster than they are put in.
		// This has a minimal impact on performance and makes the code far less RAM-intensive.
		for (int i = 0; i < 2; i++) if (queue->length > 8) // When there is at least 8 elements in queue, start building file (needs to be done to conserve memory)
		{

			unsigned char in = 0; // Build the byte to be written to text file

			if (queue->offer() == '1') in = in | 128;
			if (queue->offer() == '1') in = in | 64;
			if (queue->offer() == '1') in = in | 32;
			if (queue->offer() == '1') in = in | 16;
			if (queue->offer() == '1') in = in | 8;
			if (queue->offer() == '1') in = in | 4;
			if (queue->offer() == '1') in = in | 2;
			if (queue->offer() == '1') in = in | 1;		

			output.write((char*)&in, sizeof(in)); // Finally, write the byte to the queue

		}

	}

	input.close();

	while (queue) // The file should be mostly done at this point, but this will drain the rest of the queue
	{
		unsigned char in = 0; // Build the byte to be written to text file

		if (queue->offer() == '1') in = in | 128;
		if (queue->offer() == '1') in = in | 64;
		if (queue->offer() == '1') in = in | 32;
		if (queue->offer() == '1') in = in | 16;
		if (queue->offer() == '1') in = in | 8;
		if (queue->offer() == '1') in = in | 4;
		if (queue->offer() == '1') in = in | 2;
		if (queue->offer() == '1') in = in | 1;

		output.write((char*)&in, sizeof(in)); // Append byte to file

		if (queue->isEmpty())
		{
			delete queue; // Breaks the loop and cleans garbage
			queue = nullptr;
		}

	}

	output.close();

	ifstream bytesIn(inputFile, ios::binary | ios::ate); // Finally, output elapsed time and bytes in/out to console
	ifstream bytesOut(outputFile, ios::binary | ios::ate);

	cout << bytesIn.tellg() << " bytes in / " << bytesOut.tellg() << " bytes out" << endl;

	bytesIn.close();
	bytesOut.close();

}

void Huffman::tearDown(HNode* traverse)
{
	/* [Private method]
	*  used exclusivley by the destructor
	*  Recursivley traverses through the tree and deletes each node
	*/

	if (traverse->lPtr != nullptr) tearDown(traverse->lPtr);
	if (traverse->rPtr != nullptr) tearDown(traverse->rPtr);

	delete traverse; // When both pointers are null, node is a leaf, so delete it.

}