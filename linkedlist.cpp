#include "linkedlist.h"


void LinkedList::insertRear(Node *node)
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

void LinkedList::deleteNode(Node *node)
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
