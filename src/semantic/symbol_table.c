/**
 * Mini-C 编译器 - 符号表实现
 * 
 * 文件: symbol_table.c
 * 描述: 符号表的具体实现（基于链式哈希表 + 作用域管理）
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 */

#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ==================== 哈希函数（复用现有实现）==================== */

/**
 * RSHash 哈希函数
 * 
 * @param str 字符串
 * @param len 字符串长度
 * @return 哈希值
 */
static unsigned int RSHash(const char* str, unsigned int len) {
    unsigned int b = 378551;
    unsigned int a = 63689;
    unsigned int hash = 0;
    unsigned int i = 0;
    
    for (i = 0; i < len; str++, i++) {
        hash = hash * a + (*str);
        a = a * b;
    }
    
    return hash;
}

/**
 * 计算字符串的哈希桶索引
 * 
 * @param st 符号表指针
 * @param name 符号名称
 * @return 哈希桶索引
 */
static int hash_index(SymbolTable *st, const char *name) {
    unsigned int hash = RSHash(name, strlen(name));
    return hash % st->size;
}

/* ==================== 符号表创建和销毁 ==================== */

/**
 * 创建符号表
 */
SymbolTable* symbol_table_create(int size) {
    SymbolTable *st = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!st) {
        fprintf(stderr, "错误: 符号表内存分配失败\n");
        return NULL;
    }
    
    // 初始化字段
    st->size = size;
    st->scope_level = 0;        // 初始为全局作用域
    st->next_offset = 0;        // 栈偏移从0开始
    
    // 分配哈希桶数组
    st->buckets = (Symbol**)malloc(sizeof(Symbol*) * size);
    if (!st->buckets) {
        fprintf(stderr, "错误: 哈希桶内存分配失败\n");
        free(st);
        return NULL;
    }
    
    // 初始化所有桶为NULL
    for (int i = 0; i < size; i++) {
        st->buckets[i] = NULL;
    }
    
    return st;
}

/**
 * 销毁符号表
 */
void symbol_table_destroy(SymbolTable *st) {
    if (!st) {
        return;
    }
    
    // 释放所有哈希桶中的符号
    for (int i = 0; i < st->size; i++) {
        Symbol *sym = st->buckets[i];
        while (sym) {
            Symbol *next = sym->next;
            
            // 释放符号名称
            if (sym->name) {
                free(sym->name);
            }
            
            // 注意：type不在这里释放，由type_system统一管理
            
            free(sym);
            sym = next;
        }
    }
    
    // 释放哈希桶数组
    free(st->buckets);
    
    // 释放符号表本身
    free(st);
}

/* ==================== 符号操作 ==================== */

/**
 * 插入符号到符号表
 */
Symbol* symbol_insert(SymbolTable *st, const char *name, Type *type, int line) {
    if (!st || !name || !type) {
        return NULL;
    }
    
    // 检查当前作用域是否已存在同名符号（重定义检查）
    Symbol *existing = symbol_lookup_current_scope(st, name);
    if (existing) {
        // 重定义错误
        return NULL;
    }
    
    // 创建新符号
    Symbol *sym = (Symbol*)malloc(sizeof(Symbol));
    if (!sym) {
        fprintf(stderr, "错误: 符号内存分配失败\n");
        return NULL;
    }
    
    // 复制符号名称
    sym->name = (char*)malloc(strlen(name) + 1);
    if (!sym->name) {
        fprintf(stderr, "错误: 符号名称内存分配失败\n");
        free(sym);
        return NULL;
    }
    strcpy(sym->name, name);
    
    // 设置符号属性
    sym->type = type;
    sym->scope_level = st->scope_level;
    sym->is_global = (st->scope_level == 0);
    sym->is_initialized = false;
    sym->line = line;
    
    // 根据类型设置符号种类
    if (type->kind == TYPE_FUNCTION) {
        sym->kind = SYM_FUNCTION;
        sym->offset = 0;  // 函数不需要栈偏移量
    } else {
        sym->kind = SYM_VARIABLE;
        // 为变量分配栈空间
        if (!sym->is_global) {
            sym->offset = allocate_offset(st, type_size(type));
        } else {
            sym->offset = 0;  // 全局变量不使用栈偏移
        }
    }
    
    // 插入哈希表
    int index = hash_index(st, name);
    sym->next = st->buckets[index];
    st->buckets[index] = sym;
    
    return sym;
}

/**
 * 查找符号（支持作用域链搜索）
 */
Symbol* symbol_lookup(SymbolTable *st, const char *name) {
    if (!st || !name) {
        return NULL;
    }
    
    int index = hash_index(st, name);
    Symbol *sym = st->buckets[index];
    
    // 在链表中查找，优先返回作用域层级最高（最内层）的符号
    Symbol *best_match = NULL;
    int best_scope = -1;
    
    while (sym) {
        if (strcmp(sym->name, name) == 0) {
            // 找到同名符号
            if (sym->scope_level <= st->scope_level && 
                sym->scope_level > best_scope) {
                // 符号在当前或外层作用域，且是目前找到的最内层
                best_match = sym;
                best_scope = sym->scope_level;
            }
        }
        sym = sym->next;
    }
    
    return best_match;
}

/**
 * 在当前作用域查找符号（不搜索外层）
 */
Symbol* symbol_lookup_current_scope(SymbolTable *st, const char *name) {
    if (!st || !name) {
        return NULL;
    }
    
    int index = hash_index(st, name);
    Symbol *sym = st->buckets[index];
    
    // 仅查找当前作用域层级的符号
    while (sym) {
        if (strcmp(sym->name, name) == 0 && 
            sym->scope_level == st->scope_level) {
            return sym;
        }
        sym = sym->next;
    }
    
    return NULL;
}

/* ==================== 作用域管理 ==================== */

/**
 * 进入新作用域
 */
void enter_scope(SymbolTable *st) {
    if (!st) {
        return;
    }
    
    st->scope_level++;
}

/**
 * 退出当前作用域
 * 
 * 删除当前作用域层级的所有符号
 */
void exit_scope(SymbolTable *st) {
    if (!st || st->scope_level <= 0) {
        return;
    }
    
    int current_scope = st->scope_level;
    
    // 遍历所有哈希桶
    for (int i = 0; i < st->size; i++) {
        Symbol **ptr = &st->buckets[i];
        
        // 遍历链表，删除当前作用域的符号
        while (*ptr) {
            Symbol *sym = *ptr;
            
            if (sym->scope_level == current_scope) {
                // 删除此符号
                *ptr = sym->next;  // 从链表中移除
                
                // 释放内存
                if (sym->name) {
                    free(sym->name);
                }
                free(sym);
            } else {
                // 保留此符号，继续下一个
                ptr = &sym->next;
            }
        }
    }
    
    // 减少作用域层级
    st->scope_level--;
}

/**
 * 获取当前作用域层级
 */
int get_scope_level(SymbolTable *st) {
    return st ? st->scope_level : -1;
}

/* ==================== 栈偏移量管理 ==================== */

/**
 * 分配栈偏移量
 * 
 * 为新变量分配栈帧中的位置，考虑对齐
 */
int allocate_offset(SymbolTable *st, int size) {
    if (!st) {
        return 0;
    }
    
    // 简单的栈分配策略：顺序分配
    int offset = st->next_offset;
    
    // 对齐到4字节边界（简化实现）
    if (size % 4 != 0) {
        size = (size / 4 + 1) * 4;
    }
    
    st->next_offset += size;
    
    return offset;
}

/**
 * 获取当前栈帧大小
 */
int get_stack_size(SymbolTable *st) {
    return st ? st->next_offset : 0;
}

/* ==================== 调试和打印 ==================== */

/**
 * 打印单个符号信息
 */
void symbol_print(Symbol *sym) {
    if (!sym) {
        return;
    }
    
    printf("  符号: %s\n", sym->name);
    printf("    类型: %s\n", type_to_string(sym->type));
    printf("    作用域层级: %d\n", sym->scope_level);
    
    if (!sym->is_global) {
        printf("    栈偏移: %d\n", sym->offset);
    } else {
        printf("    全局变量\n");
    }
    
    printf("    定义行号: %d\n", sym->line);
    printf("    已初始化: %s\n", sym->is_initialized ? "是" : "否");
}

/**
 * 打印符号表内容
 */
void symbol_table_print(SymbolTable *st) {
    if (!st) {
        return;
    }
    
    printf("========================================\n");
    printf("符号表内容\n");
    printf("========================================\n");
    printf("当前作用域层级: %d\n", st->scope_level);
    printf("栈帧大小: %d 字节\n", st->next_offset);
    printf("哈希表大小: %d\n", st->size);
    printf("\n符号列表:\n");
    
    int count = 0;
    for (int i = 0; i < st->size; i++) {
        Symbol *sym = st->buckets[i];
        while (sym) {
            symbol_print(sym);
            printf("\n");
            count++;
            sym = sym->next;
        }
    }
    
    printf("符号总数: %d\n", count);
    printf("========================================\n");
}

/* ==================== 统计信息 ==================== */

/**
 * 获取符号表中符号总数
 */
int symbol_table_count(SymbolTable *st) {
    if (!st) {
        return 0;
    }
    
    int count = 0;
    for (int i = 0; i < st->size; i++) {
        Symbol *sym = st->buckets[i];
        while (sym) {
            count++;
            sym = sym->next;
        }
    }
    
    return count;
}

/* ==================== 函数符号辅助函数（2.0版本）==================== */

/**
 * 检查符号是否为函数
 * 
 * @param sym 符号指针
 * @return 是函数返回true，否则返回false
 */
bool symbol_is_function(Symbol *sym) {
    return sym && sym->kind == SYM_FUNCTION;
}

/**
 * 获取函数参数数量
 * 
 * @param sym 符号指针
 * @return 参数数量，如果不是函数返回-1
 */
int symbol_get_param_count(Symbol *sym) {
    if (!sym || sym->kind != SYM_FUNCTION) {
        return -1;
    }
    if (!sym->type || sym->type->kind != TYPE_FUNCTION) {
        return -1;
    }
    return sym->type->param_count;
}

/**
 * 获取函数参数类型
 * 
 * @param sym 符号指针
 * @param index 参数索引（从0开始）
 * @return 参数类型，如果索引无效返回NULL
 */
Type* symbol_get_param_type(Symbol *sym, int index) {
    if (!sym || sym->kind != SYM_FUNCTION) {
        return NULL;
    }
    if (!sym->type || sym->type->kind != TYPE_FUNCTION) {
        return NULL;
    }
    if (index < 0 || index >= sym->type->param_count) {
        return NULL;
    }
    return sym->type->param_types[index];
}

/**
 * 获取函数返回类型
 * 
 * @param sym 符号指针
 * @return 返回类型，如果不是函数返回NULL
 */
Type* symbol_get_return_type(Symbol *sym) {
    if (!sym || sym->kind != SYM_FUNCTION) {
        return NULL;
    }
    if (!sym->type || sym->type->kind != TYPE_FUNCTION) {
        return NULL;
    }
    return sym->type->return_type;
}

/**
 * 获取当前作用域的符号数量
 */
int symbol_table_count_current_scope(SymbolTable *st) {
    if (!st) {
        return 0;
    }
    
    int count = 0;
    int current_scope = st->scope_level;
    
    for (int i = 0; i < st->size; i++) {
        Symbol *sym = st->buckets[i];
        while (sym) {
            if (sym->scope_level == current_scope) {
                count++;
            }
            sym = sym->next;
        }
    }
    
    return count;
}

