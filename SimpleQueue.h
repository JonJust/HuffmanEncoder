#pragma once
#include "QNode.h"

class SimpleQueue
{
public:

	SimpleQueue(); // constructor/destructor
	~SimpleQueue();

	void add(unsigned char key); // adds item to queue
	unsigned char offer(); // returns item from start of the queue and removes it
	bool isEmpty(); // returns true is head is nullptr
	int length; // number of elements in queue

private:

	QNode* head; // points to first item in queue
	QNode* tail; // points to last item in queue (speeds up process of adding new nodes w/ large datasets)

};

