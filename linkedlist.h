#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include <stdio.h>
class LinkedList;
class Node
{
public:
	Node(): next(NULL), prev(NULL) {}
	virtual ~Node() {};

	Node *getNext() { return next;}
	Node *getPrev() { return prev;}

protected:
	Node *next;
	Node *prev;

	friend class LinkedList;
};

class LinkedList
{
public:
   	LinkedList() : head(NULL), rear(NULL), size_(0) {}
   	virtual ~LinkedList() {}

	void insertRear(Node *node);
	void deleteNode(Node *node);

	Node *getHead() { return head;}
	Node *getRear() { return rear;}
	int size() { return size_; }

protected:
	Node *head;
	Node *rear;
	int size_;
};


#endif
