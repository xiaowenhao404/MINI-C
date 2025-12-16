/**
 * Mini-C 编译器 - 类型系统
 * 
 * 文件: type_system.h
 * 描述: 类型系统的核心定义和接口
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 * 
 * 功能:
 * - 支持基本类型: int, float, char, void
 * - 预留扩展类型: 数组, 指针, 结构体, 函数
 * - 类型大小计算
 * - 类型比较和兼容性判断
 * - 类型提升规则
 */

#ifndef TYPE_SYSTEM_H
#define TYPE_SYSTEM_H

#include <stdbool.h>

/* ==================== 类型定义 ==================== */

/**
 * 类型种类枚举
 */
typedef enum {
    TYPE_VOID,      // void类型（无返回值）
    TYPE_INT,       // 整型 (4字节)
    TYPE_FLOAT,     // 浮点型 (4字节)
    TYPE_CHAR,      // 字符型 (1字节)
    TYPE_ARRAY,     // 数组类型（2.0版本）
    TYPE_POINTER,   // 指针类型（2.0版本）
    TYPE_STRUCT,    // 结构体类型（2.0版本）
    TYPE_FUNCTION   // 函数类型（2.0版本）
} TypeKind;

/**
 * 结构体成员定义（2.0版本使用）
 */
typedef struct StructMember {
    char *name;                 // 成员名称
    struct Type *type;          // 成员类型
    int offset;                 // 在结构体中的偏移量
    struct StructMember *next;  // 下一个成员
} StructMember;

/**
 * 类型结构体
 * 
 * 描述一个完整的类型信息，包括基本类型和派生类型
 */
typedef struct Type {
    TypeKind kind;              // 类型种类
    int size;                   // 类型大小（字节）
    
    /* 派生类型相关字段 */
    struct Type *base;          // 基类型（用于数组、指针）
    int array_len;              // 数组长度（TYPE_ARRAY）
    
    /* 结构体相关字段（2.0版本） */
    char *struct_name;          // 结构体名称
    StructMember *members;      // 结构体成员链表
    
    /* 函数类型相关字段（2.0版本） */
    struct Type *return_type;   // 函数返回类型
    struct Type **param_types;  // 参数类型数组
    int param_count;            // 参数个数
} Type;

/* ==================== 基本类型构造函数 ==================== */

/**
 * 创建void类型
 * @return void类型对象指针
 */
Type* new_void_type(void);

/**
 * 创建int类型
 * @return int类型对象指针
 */
Type* new_int_type(void);

/**
 * 创建float类型
 * @return float类型对象指针
 */
Type* new_float_type(void);

/**
 * 创建char类型
 * @return char类型对象指针
 */
Type* new_char_type(void);

/* ==================== 派生类型构造函数（2.0版本预留）==================== */

/**
 * 创建数组类型
 * @param base 数组元素的基类型
 * @param length 数组长度
 * @return 数组类型对象指针
 */
Type* new_array_type(Type *base, int length);

/**
 * 创建指针类型
 * @param base 指针指向的基类型
 * @return 指针类型对象指针
 */
Type* new_pointer_type(Type *base);

/* ==================== 类型属性查询 ==================== */

/**
 * 获取类型大小
 * @param t 类型对象指针
 * @return 类型大小（字节数）
 */
int type_size(Type *t);

/**
 * 判断类型是否为整数类型（int或char）
 * @param t 类型对象指针
 * @return 是整数类型返回true，否则返回false
 */
bool is_integer_type(Type *t);

/**
 * 判断类型是否为浮点类型
 * @param t 类型对象指针
 * @return 是浮点类型返回true，否则返回false
 */
bool is_float_type(Type *t);

/**
 * 判断类型是否为数值类型（int、float或char）
 * @param t 类型对象指针
 * @return 是数值类型返回true，否则返回false
 */
bool is_numeric_type(Type *t);

/* ==================== 类型比较 ==================== */

/**
 * 判断两个类型是否完全相同
 * @param a 第一个类型
 * @param b 第二个类型
 * @return 相同返回true，否则返回false
 */
bool type_equal(Type *a, Type *b);

/**
 * 判断两个类型是否兼容（可隐式转换）
 * 
 * 兼容规则：
 * - int 和 float 互相兼容
 * - char 和 int 兼容
 * - 相同类型总是兼容
 * 
 * @param a 第一个类型
 * @param b 第二个类型
 * @return 兼容返回true，否则返回false
 */
bool type_compatible(Type *a, Type *b);

/**
 * 判断是否可以进行显式类型转换
 * @param from 源类型
 * @param to 目标类型
 * @return 可以转换返回true，否则返回false
 */
bool can_cast(Type *from, Type *to);

/* ==================== 类型提升 ==================== */

/**
 * 获取两个类型的提升类型（用于二元运算）
 * 
 * 提升规则：
 * - int + int → int
 * - float + float → float
 * - int + float → float
 * - char + int → int
 * - char + float → float
 * 
 * @param a 第一个类型
 * @param b 第二个类型
 * @return 提升后的类型，如果无法提升返回NULL
 */
Type* promote_type(Type *a, Type *b);

/**
 * 获取类型名称字符串（用于错误报告）
 * @param t 类型对象指针
 * @return 类型名称字符串（静态字符串，不需要释放）
 */
const char* type_to_string(Type *t);

/* ==================== 类型系统初始化和清理 ==================== */

/**
 * 初始化类型系统
 * 
 * 创建并缓存常用的基本类型对象，避免重复创建
 */
void init_type_system(void);

/**
 * 清理类型系统
 * 
 * 释放类型系统使用的所有内存
 */
void cleanup_type_system(void);

/* ==================== 内存管理 ==================== */

/**
 * 创建类型对象（内部使用）
 * @param kind 类型种类
 * @param size 类型大小
 * @return 新创建的类型对象指针
 */
Type* create_type(TypeKind kind, int size);

/**
 * 复制类型对象
 * @param t 要复制的类型
 * @return 新的类型对象副本
 */
Type* type_copy(Type *t);

/**
 * 释放类型对象（谨慎使用，通常由类型系统统一管理）
 * @param t 要释放的类型对象
 */
void type_free(Type *t);

#endif /* TYPE_SYSTEM_H */

