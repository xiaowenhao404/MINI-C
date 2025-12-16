#include "hashMap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "stack.h"

extern int scope, type, preType;
void yyerror(const char *s);

// Hash Function
unsigned int RSHash(const char* str, unsigned int len){
    unsigned int b = 378551;
    unsigned int a = 63689;
    unsigned int hash = 0;
    unsigned int i = 0;
    for(i = 0; i < len; str++, i++){
        hash = hash * a + (*str);
        a = a * b;
    }
    return hash;
}

HashMap* createHashMap(int size){
    HashMap* hashMap = (HashMap*)malloc(sizeof(HashMap));
    hashMap->size = size;
    hashMap->hash_table = (HashNode**)malloc(sizeof(HashNode*) * size);
    int i;
    // init hashMap
    for(i = 0; i < size; i++){
        hashMap->hash_table[i] = NULL;
    }
    return hashMap;
}

// variable to data
Data* toData(int type, char* str, struct Declator* declator, int scope){
    Data* data = (Data*)malloc(sizeof(Data));
    int len = strlen(str);
    data->id_name = (char*)malloc(len);
    memcpy(data->id_name, str, len);
    data->type = type;
    data->scope = scope;
    data->declator = declator;
    return data;
}


void freeHashNode(HashNode* hashNode){
    Declator* d, *temp;
    if(hashNode->data->declator){
        d = hashNode->data->declator;
        while(d->next){
            temp = d->next;
            free(d);
            d = temp;
        }
        free(d);
    }
    free(hashNode->data);
    free(hashNode);
}

// 分配内存
void getSize(HashNode* hashNode){
    int size = sizeof(int);
    int num = 1;
    Declator* d;
    for(d = hashNode->data->declator; d; d = d->next){
        if(d->type == ARRAY){
            // num *= d->length;
        }else if(d->type == POINTER){
            size = sizeof(long);
        }
    }
    hashNode->data->size = size * num;
}
HashNode* createHashNode(Data* data){
    HashNode* hashNode = (HashNode*)malloc(sizeof(HashNode));
    hashNode->data = data;
    getSize(hashNode);
    hashNode->next = NULL;
    return hashNode;
}

// add data to hashMap
HashNode* put(HashMap* hashMap, Data* data){
    const char* name = data->id_name;
    int pos = RSHash(name, strlen(name)) % hashMap->size;
    HashNode* ptr = hashMap->hash_table[pos];
    if(!ptr){
        hashMap->hash_table[pos] = createHashNode(data);
        return hashMap->hash_table[pos];
    }
    if(ptr->data->scope < data->scope){
        HashNode* hashNode = createHashNode(data);
        hashNode->next = ptr;
        hashMap->hash_table[pos] = hashNode;
        return ptr;
    }
    while(ptr->next && ptr->next->data->scope >= data->scope){
        if(strcmp(ptr->data->id_name, name) == 0 && ptr->data->scope == data->scope){
            return NULL; // already exit;
        }
        ptr = ptr->next;
    }
    if(strcmp(ptr->data->id_name, name) == 0 && ptr->data->scope == data->scope){
        return NULL; // already exit;
    }
    HashNode* hashNode = createHashNode(data);
    hashNode->next = ptr->next;
    ptr->next = hashNode;
    return hashNode;
}

void putTree(HashMap* hashMap, struct Tree *tree){
    Stack* s = createStack();
    push(s, tree, 0);
    SNode* n;
    int i;
    while((n = pop(s))){
        if(n->tree->num == 0){
            if(strcmp(n->tree->name, "ID")){
                continue;
            }
            if(!put(hashMap, toData(type, n->tree->content, n->tree->declator, scope))){
                char* error = (char*)malloc(50);
                sprintf(error, "\"%s\" is  already defined", n->tree->content);
                yyerror(error);
                free(error);
            }
        }else{
            for(i = 0; i < n->tree->num; i++){
                push(s, n->tree->leaves[i], 0);
            }
        }
    }
}

// find data in hashMap, return NULL when error
HashNode* get(HashMap* hashMap, Data* data){
    const char* name = data->id_name;
    int pos = RSHash(name, strlen(name)) % hashMap->size;
    HashNode* ptr = hashMap->hash_table[pos];
    while(ptr){
        if(!ptr->data){
            break;
        }
        if(strcmp(ptr->data->id_name, name) == 0 && ptr->data->scope <= data->scope){
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

void printData(HashMap* hashMap){
    HashNode* ptr;
    Declator* d;
    int i;
    for(i = 0; i < hashMap->size; i++){
        ptr = hashMap->hash_table[i];
        while(ptr && ptr->data){
            printf("%s %d ", ptr->data->id_name, ptr->data->type);
            for(d = ptr->data->declator; d; d = d->next){
                // printf("%d %d ", d->type, d->length);
                printf("%d ", d->type);
            }
            printf("%p", ptr->data->adress);
            printf("\n");
            ptr = ptr->next;
        }
    }
}

void destoryPartOfHashMap(HashMap* hashMap, int scope){
    // printData(hashMap);
    int i;
    HashNode* ptr;
    for(i = 0; i < hashMap->size; i++){
        ptr = hashMap->hash_table[i];
        if(!ptr || ptr->data->scope < scope){
            continue;
        }
        if(ptr->data->scope == scope){   // delete from head
            while(ptr && ptr->data->scope == scope){
                hashMap->hash_table[i] = hashMap->hash_table[i]->next;
                freeHashNode(ptr);
                ptr = hashMap->hash_table[i];
            }
            return;
        }
        for(; ptr->next; ptr = ptr->next){
            if(ptr->next->data->scope <= scope){
                break;
            }
        }
        if(!ptr->next){ //end
            return;
        }
        HashNode* before = ptr;
        ptr = ptr->next;
        while(ptr && ptr->data->scope == scope){
            before->next = ptr->next;
            free(ptr);
            ptr = before->next;
        }
    }
}

void destoryHashMap(HashMap* hashMap){
    int i;
    HashNode* ptr;
    for(i = 0; i < hashMap->size; i++){
        while((ptr = hashMap->hash_table[i])){
            hashMap->hash_table[i] = hashMap->hash_table[i]->next;
            freeHashNode(ptr);
        }
    }
    free(hashMap->hash_table);
    free(hashMap);
}

void printHashMap(HashMap* hashMap){
    HashNode* ptr;
    int i;
    for(i = 0; i < hashMap->size; i++){
        ptr = hashMap->hash_table[i];
        while(ptr && ptr->data){
            printf("%-10s\t", ptr->data->id_name);
            ptr = ptr->next;
        }
        printf("\n");
    }
}
