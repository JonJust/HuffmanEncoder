/*
Name: Jonathan Just
Date: 9/22/2020
Class: EECS 2510, Non-Linear Data Structures
Professor: Dr. Lawrence Thomas

Source.cpp

Console application allowing the user to huffman encode a target file

============MODES===========

Show help:
	
	Sytnax:
	HUFF -h
	HUFF -?
	HUFF -help

	Uses: Displays usage options to the user and exits

Encode Directly from Input File

	Syntax:
	HUFF -e file1 [file2]

	Uses: Encode file1 and place its output to file2. If the user omits file2, then simply encode file1 directly and append .huf to it
	
Decode file:
	
	Syntax: HUFF -d file1 file2

	Uses: Decodes a file1 and places its contents into file2.

Create a tree-building file:

	Syntax: HUFF -t file1 [file2]

	Uses: Reads file1 and builds a 510-byte tree building file. If file2 is omitted, then then create a file with file1's name using the .htree extension.

Encoding with a specified tree-builder:

	Syntax: HUFF -et file1 file2 [file3]

	Uses: Reads input from file1 and encodes it based on the 510-byte tree-builder file file2. The output will be file3. If omitted, create a new file with file1's name but with the .huf extension.

==========================================

*/

#include <iostream>
#include<string>

#include "HNode.h"
#include "Huffman.h"

using namespace std;

void helpMode();

int main(int argc, char* argv[]) 
{

	if ((string)argv[1] == "-h" || (string)argv[1] == "-?" || (string)argv[1] == "-help") helpMode(); // Enter help mode. (Don't need to create any trees to do this)

	Huffman* htree = new Huffman();

	string files[3]; // Array used to keep track of files

	int fileIndex = 0; // used to keep track of which string in argv[] begins

	files[0] = argv[2]; // Prime the loop

	for (int i = 3; i < argc; i++) // Connects file names with spaces in them together
	{
		if (files[fileIndex].find('.') == string::npos) files[fileIndex] = files[fileIndex] + " " + (string)argv[i]; // if files[fileIndex] still doesn't have an extension, keep building
		else
		{
			fileIndex++; // else, start building next file
			if (fileIndex > 3) break;
			files[fileIndex] = argv[i];
		}
	}

	if ((string)argv[1] == "-e") htree->encodeFile(files[0], files[1]); // encodes files[0]

	else if ((string)argv[1] == "-t") htree->makeTreeBuilder(files[0], files[1]); // makes a tree builder file for files[1]

	else if ((string)argv[1] == "-d" && files[1] != "") htree->decodeFile(files[0], files[1]); // decodes files[0] if an output file is specified

	else if ((string)argv[1] == "-et") htree->encodeFileWithTree(files[0], files[1], files[2]); // encodes files[0] file with tree file files[1]

	delete htree;
	exit(0);

}

void helpMode()
{
	cout << "ARGUMENTS:" << endl;
	cout << "HELP MODE: -h, -?, -help" << endl;
	cout << "ENCODE FILE: -e file1 [file2]" << endl;
	cout << "DECODE FILE: -d file1 [file2]" << endl;
	cout << "CREATE TREE-BUILDING FILE: -t file1 [file2]" << endl;
	cout << "ENCODE WITH A SPECIFIED TREE-BUILDER: -et file1 file2 [file3]" << endl;
	return;
}