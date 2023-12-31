#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include <user/User.h>

#define TRUE    1
#define FALSE   0

typedef User * Data;

typedef struct _node
{
	Data data;
	struct _node * next;
	struct _node * prev;
} Node;

typedef struct _dbLinkedList
{
	Node * head;
	Node * tail;
	Node * cur;
	int numOfData;
} DBLinkedList;

typedef DBLinkedList List;

void ListInit(List * plist);
void LInsert(List * plist, Data data);

int LFirst(List * plist, Data * pdata);
int LNext(List * plist, Data * pdata);

Data LRemove(List * plist);

int LCount(List * plist);

#endif /* __LINKED_LIST_H__ */