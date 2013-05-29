#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include <stdio.h>


template<class T>
class LinkedList;

template<class T>
class Node
{
public:
	Node(): next(NULL), prev(NULL) {}
	Node(const T &data): next(NULL), prev(NULL), data_(data) {}
	virtual ~Node() {};

	Node *getNext() { return next;}
	Node *getPrev() { return prev;}
	T &data() {return data_;}
	const T &data() const {return data_;}

protected:
	T data_;
	Node *next;
	Node *prev;

	friend class LinkedList<T>;
};


template<class T>
class LinkedList
{
public:
   	LinkedList() : head(NULL), rear(NULL), size_(0) {}
   	virtual ~LinkedList() {}

	void insertRear(Node<T> *node);
	void deleteNode(Node<T> *node);

	Node<T> *getHead() { return head;}
	Node<T> *getRear() { return rear;}
	int size() { return size_; }
protected:
	Node<T> *head;
	Node<T> *rear;
	int size_;
};


template<class T>
void LinkedList<T>::insertRear(Node<T> *node)
{
	if (node == NULL)
		return;

	node->next = node->prev = NULL;
	if (rear == NULL) {
		head = rear = node;
	} else {
		rear->next = node;
		node->prev = rear;
		rear = node;
	}
	++size_;
}

template<class T>
void LinkedList<T>::deleteNode(Node<T> *node)
{
	if (node == NULL)
		return;

	if (node == head) {
		head = head->next;
		if (head == NULL) {
			rear = NULL;
		} else {
			head->prev = NULL;
		}
	} else if (node == rear) {
		rear = rear->prev;
		if (rear == NULL) {
			head = NULL;
		} else {
			rear->next = NULL;
		}
	} else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	--size_;
}

#endif
