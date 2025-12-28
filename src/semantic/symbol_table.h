/**
 * Mini-C 编译器 - 符号表
 * 
 * 文件: symbol_table.h
 * 描述: 符号表的核心定义和接口（增强版）
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 * 
 * 功能:
 * - 基于链式哈希表的高效符号存储
 * - 支持嵌套作用域管理
 * - 集成类型系统
 * - 支持变量和函数符号
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "type_system.h"
#include <stdbool.h>

/* ==================== 符号定义 ==================== */

/**
 * 符号类型枚举
 */
typedef enum {
    SYM_VARIABLE,   // 变量符号
    SYM_FUNCTION,   // 函数符号（2.0版本）
    SYM_TYPEDEF     // 类型定义符号（2.0版本）
} SymbolKind;

/**
 * 符号结构体
 * 
 * 表示符号表中的一个符号（变量、函数等）
 */
typedef struct Symbol {
    char *name;             // 符号名称
    SymbolKind kind;        // 符号类型
    Type *type;             // 数据类型（来自type_system）
    
    int scope_level;        // 作用域层级（0=全局）
    int offset;             // 栈帧偏移量（字节）
    bool is_global;         // 是否全局变量
    bool is_initialized;    // 是否已初始化
    bool is_declared;       // 是否已从AST声明过（用于检测重定义）
    
    int line;               // 定义所在行号（用于错误报告）
    
    struct Symbol *next;    // 哈希表链表指针
} Symbol;

/**
 * 符号表结构体
 * 
 * 使用链式哈希表实现，支持嵌套作用域
 */
typedef struct SymbolTable {
    Symbol **buckets;       // 哈希桶数组
    int size;               // 哈希表大小（桶数量）
    int scope_level;        // 当前作用域层级
    int next_offset;        // 下一个变量的栈偏移量
} SymbolTable;

/* ==================== 符号表创建和销毁 ==================== */

/**
 * 创建符号表
 * 
 * @param size 哈希表大小（建议使用素数，如127）
 * @return 符号表指针
 */
SymbolTable* symbol_table_create(int size);

/**
 * 销毁符号表
 * 
 * 释放符号表及其包含的所有符号
 * 
 * @param st 符号表指针
 */
void symbol_table_destroy(SymbolTable *st);

/* ==================== 符号操作 ==================== */

/**
 * 插入符号到符号表
 * 
 * 在当前作用域层级插入符号。如果当前作用域已存在同名符号，返回NULL（重定义错误）
 * 
 * @param st 符号表指针
 * @param name 符号名称
 * @param type 符号类型
 * @param line 定义所在行号
 * @return 成功返回插入的符号指针，失败（重定义）返回NULL
 */
Symbol* symbol_insert(SymbolTable *st, const char *name, Type *type, int line);

/**
 * 查找符号（支持作用域链搜索）
 * 
 * 从当前作用域开始查找，如果找不到则向外层作用域搜索
 * 
 * @param st 符号表指针
 * @param name 符号名称
 * @return 找到的符号指针，未找到返回NULL
 */
Symbol* symbol_lookup(SymbolTable *st, const char *name);

/**
 * 在当前作用域查找符号（不搜索外层）
 * 
 * 仅在当前作用域层级查找，用于检测重定义
 * 
 * @param st 符号表指针
 * @param name 符号名称
 * @return 找到的符号指针，未找到返回NULL
 */
Symbol* symbol_lookup_current_scope(SymbolTable *st, const char *name);

/* ==================== 作用域管理 ==================== */

/**
 * 进入新作用域
 * 
 * 增加作用域层级，通常在遇到 '{' 时调用
 * 
 * @param st 符号表指针
 */
void enter_scope(SymbolTable *st);

/**
 * 退出当前作用域
 * 
 * 删除当前作用域层级的所有符号，减少作用域层级，通常在遇到 '}' 时调用
 * 
 * @param st 符号表指针
 */
void exit_scope(SymbolTable *st);

/**
 * 获取当前作用域层级
 * 
 * @param st 符号表指针
 * @return 当前作用域层级
 */
int get_scope_level(SymbolTable *st);

/* ==================== 栈偏移量管理 ==================== */

/**
 * 分配栈偏移量
 * 
 * 为新变量分配栈帧中的位置
 * 
 * @param st 符号表指针
 * @param size 变量大小（字节）
 * @return 分配的栈偏移量
 */
int allocate_offset(SymbolTable *st, int size);

/**
 * 获取当前栈帧大小
 * 
 * @param st 符号表指针
 * @return 栈帧大小（字节）
 */
int get_stack_size(SymbolTable *st);

/* ==================== 调试和打印 ==================== */

/**
 * 打印符号表内容（调试用）
 * 
 * @param st 符号表指针
 */
void symbol_table_print(SymbolTable *st);

/**
 * 打印单个符号信息
 * 
 * @param sym 符号指针
 */
void symbol_print(Symbol *sym);

/* ==================== 统计信息 ==================== */

/**
 * 获取符号表中符号总数
 * 
 * @param st 符号表指针
 * @return 符号数量
 */
int symbol_table_count(SymbolTable *st);

/**
 * 获取当前作用域的符号数量
 * 
 * @param st 符号表指针
 * @return 当前作用域符号数量
 */
int symbol_table_count_current_scope(SymbolTable *st);

/* ==================== 函数符号辅助函数（2.0版本）==================== */

/**
 * 检查符号是否为函数
 * 
 * @param sym 符号指针
 * @return 是函数返回true，否则返回false
 */
bool symbol_is_function(Symbol *sym);

/**
 * 获取函数参数数量
 * 
 * @param sym 符号指针
 * @return 参数数量，如果不是函数返回-1
 */
int symbol_get_param_count(Symbol *sym);

/**
 * 获取函数参数类型
 * 
 * @param sym 符号指针
 * @param index 参数索引（从0开始）
 * @return 参数类型，如果索引无效返回NULL
 */
Type* symbol_get_param_type(Symbol *sym, int index);

/**
 * 获取函数返回类型
 * 
 * @param sym 符号指针
 * @return 返回类型，如果不是函数返回NULL
 */
Type* symbol_get_return_type(Symbol *sym);

#endif /* SYMBOL_TABLE_H */

