/*
Name: Jonathan Just
Date: 9/25/2020
Class: EECS 2510, Non-Linear Data Structures
Professor: Dr. Lawrence Thomas

QNode.h

A simple node struct used for a queue

Has a key contianing a string, a weight, and 3 pointers.

To be used in huffman tree

*/

#include<string>
#pragma once
struct QNode
{
	unsigned char key; // Bit to be carried by node

	QNode* next; // next item in queue
};