#ifndef LINKLIST_H
#define LINKLIST_H

typedef struct ListNode{
    char *data;
    struct ListNode *before;
    struct ListNode *next;
}ListNode;

typedef struct LinkList{
    struct ListNode *head;
    struct ListNode *tail;
}LinkList;

// init LinkList
LinkList* initLinkList();
// init ListNode
ListNode* initListNode();
// append ListNode
void append(LinkList* list, ListNode* node);
// append many ListNode
void appendMany(LinkList* list, int num, ...);
// connnet two LinkList
void extend(LinkList* list1, LinkList* list2);
// connect many LinkList 
void extendMany(LinkList* list, int num, ...);
// destory LinkList
void freeLinkList(LinkList* list);

#endif