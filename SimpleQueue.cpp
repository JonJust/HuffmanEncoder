/*
Name: Jonathan Just
Date: 9/25/2020
Class: EECS 2510, Non-Linear Data Structures
Professor: Dr. Lawrence Thomas

SimpleQueue.cpp

A simple queue class. (FIFO)

Stores a char in a node. To add a char to the front of the queue, call add().

To return the item from the fron of the queue and remove it from the queue, call offer().

*/

#include "SimpleQueue.h"


SimpleQueue::SimpleQueue() 
{

	head = nullptr; // First item in queue
	tail = nullptr; // Last item in queue (Used to make insertion process O(1) time rather than O(n) time)
	length = 0; // Length of queue (Updated with insertion method)

}

SimpleQueue::~SimpleQueue() 
{ // Tears down queue by parsing through it and deleting each node

	if (head == nullptr) return; // If head is a nullptr then the queue is empty; return

	QNode* trailer = nullptr; // always a step behind head

	while (head->next != nullptr)
	{
		trailer = head;
		head = head->next;
		delete trailer; // trailer is behind head, so its safe to delete
	}

	delete head; // while loop leaves one final node that head points to; delete it before returning

	return;
}

void SimpleQueue::add(unsigned char key)
{
	// adds a node to the front of the queue

	length++;

	if (head == nullptr) // if head is a nullptr, queue is empty
	{
		head = new QNode; // Create new node, set it to the head, and return.
		head->key = key;
		head->next = nullptr;
		tail = head;
		return;
	}

	QNode* newNode = new QNode;
	newNode->key = key;
	newNode->next = nullptr;
	tail->next = newNode; // finally, create new node and add it to the end of the list

	tail = newNode; // update tail

	return;

}

unsigned char SimpleQueue::offer()
{
	if (head == nullptr) return 0; // returns null terminator if queue is empty

	length--;

	unsigned char key = head->key;

	if (head->next == nullptr)
	{
		delete head;
		head = nullptr;
		return key;
	}

	QNode* trailer = head;

	if (head->next != nullptr) head = head->next; // go to next item in array and set it as the new head
	
	delete trailer; // finally, delete the former head. (Or the current head if its the only item in the queue)

	return key;

}

bool SimpleQueue::isEmpty()
{
	if (head == nullptr) return true;

	return false;
}