#include "linkList.h"
#include <malloc.h>
#include <stdarg.h>
#include <string.h>

LinkList* initLinkList(){
    LinkList* list = (LinkList*)malloc(sizeof(LinkList));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

ListNode* initListNode(){
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    node->data = NULL;
    node->before = NULL;
    node->next = NULL;
    return node;
}

void append(LinkList* list, ListNode* node){
    if(!list){
        list = initLinkList();
    }
    if(!list->head){
        list->head = list->tail = node;
    }else{
       list->tail->next = node;
       node->before = list->tail;
       list->tail = node; 
    }
}

void appendMany(LinkList* list, int num, ...){
    if(!list){
        list = initLinkList();
    }
    va_list valist;
    va_start(valist, num);
    ListNode* temp;
    int i;
    for(i = 0; i < num; i++){
        temp = va_arg(valist, ListNode*);
        if(!list->head){
            list->head = list->tail = temp;
        }else{
            list->tail->next = temp;
            temp->before = list->tail;
            list->tail = temp;
        }
    }
}

void extend(LinkList* list1, LinkList* list2){
    if(!list1){
        list1 = list2;
    }else if(list2 && list2->head){
        list1->tail->next = list2->head;
        list2->head->before = list1->tail;
        list1->tail = list2->tail;
    }
}

void extendMany(LinkList* list, int num, ...){
    va_list valist;
    va_start(valist, num);
    LinkList* temp;
    int i;
    for(i = 0; i < num; i++){
        temp = va_arg(valist, LinkList*);
        if(!list){
            list = temp;
        }else if(temp && temp->head){
            list->tail->next = temp->head;
            temp->head->before = list->tail;
            list->tail = temp->tail;
        }
    }
}

void freeLinkList(LinkList* list){
    if(!list){
        return;
    }
    if(!list->head){
        free(list);
        return;
    }
    ListNode* temp;
    for(temp = list->head->next; temp; list->head = temp, temp = list->head->next){
        if(list->head->data){
            free(list->head->data);    
        }
        free(list->head);
    }
    free(list->head);
    free(list);
}