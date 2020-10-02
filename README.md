# HuffmanEncoder
A Huffman compression program I built for school

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
