/*
Name: Jonathan Just
Date: 9/20/2020
Class: EECS 2510, Non-Linear Data Structures
Professor: Dr. Lawrence Thomas

HNode.h

A simple node struct.

Has a key contianing a string, a weight, and 2 pointers.

To be used in huffman tree

*/

#include<string>
#pragma once
struct HNode
{

	unsigned char key; // Byte carried by node

	HNode* lPtr; // left and right pointers
	HNode* rPtr;

	unsigned int weight; // weight of the node

};