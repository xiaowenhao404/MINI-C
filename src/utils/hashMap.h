#ifndef HASHMAP_H
#define HASHMAP_H

#define ARRAY 1
#define POINTER 2

typedef struct Declator{    // 修饰符(array or pointer)
    int type;
    struct Tree* length;
    struct Declator* next;
}Declator;

typedef struct Data{
    char* id_name;  // 变量名 
    int type;       // 变量类型
    void* adress;    // 存储地址
    int size;
    int scope;      // 作用域
    struct Declator* declator;
}Data;

typedef struct HashNode{
    Data* data;
    struct HashNode* next;
}HashNode;

typedef struct HashMap{
    int size;
    HashNode** hash_table;
}HashMap;

// Hash Function
unsigned int RSHash(const char* str, unsigned int len);

HashMap* createHashMap(int size);

// variable to data
Data* toData(int type, char* str, struct Declator* declator, int scope);


void freeHashNode(HashNode* hashNode);

// 分配内存
void getSize(HashNode* hashNode);

HashNode* createHashNode(Data* data);

// add data to hashMap
HashNode* put(HashMap* hashMap, Data* data);

void putTree(HashMap* hashMap, struct Tree *tree);

// find data in hashMap, return NULL when error
HashNode* get(HashMap* hashMap, Data* data);

void printData(HashMap* hashMap);

void destoryPartOfHashMap(HashMap* hashMap, int scope);

void destoryHashMap(HashMap* hashMap);

void printHashMap(HashMap* hashMap);

#endif