/**
 * Mini-C 编译器 - 类型系统实现
 * 
 * 文件: type_system.c
 * 描述: 类型系统的具体实现
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 */

#define _POSIX_C_SOURCE 200809L
#include "type_system.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ==================== 全局类型缓存 ==================== */

// 缓存常用的基本类型，避免重复创建
static Type *cached_void_type = NULL;
static Type *cached_int_type = NULL;
static Type *cached_float_type = NULL;
static Type *cached_char_type = NULL;

/* ==================== 内部辅助函数 ==================== */

/**
 * 创建类型对象
 * @param kind 类型种类
 * @param size 类型大小
 * @return 新创建的类型对象指针
 */
Type* create_type(TypeKind kind, int size) {
    Type *type = (Type*)malloc(sizeof(Type));
    if (!type) {
        fprintf(stderr, "错误: 内存分配失败\n");
        exit(1);
    }
    
    // 初始化所有字段
    type->kind = kind;
    type->size = size;
    type->base = NULL;
    type->array_len = 0;
    type->struct_name = NULL;
    type->members = NULL;
    type->return_type = NULL;
    type->param_types = NULL;
    type->param_count = 0;
    
    return type;
}

/* ==================== 类型系统初始化和清理 ==================== */

/**
 * 初始化类型系统
 * 创建并缓存常用的基本类型对象
 */
void init_type_system(void) {
    // 创建基本类型并缓存
    if (!cached_void_type) {
        cached_void_type = create_type(TYPE_VOID, 0);
    }
    if (!cached_int_type) {
        cached_int_type = create_type(TYPE_INT, 4);
    }
    if (!cached_float_type) {
        cached_float_type = create_type(TYPE_FLOAT, 4);
    }
    if (!cached_char_type) {
        cached_char_type = create_type(TYPE_CHAR, 1);
    }
}

/**
 * 清理类型系统
 * 释放缓存的类型对象
 */
void cleanup_type_system(void) {
    if (cached_void_type) {
        free(cached_void_type);
        cached_void_type = NULL;
    }
    if (cached_int_type) {
        free(cached_int_type);
        cached_int_type = NULL;
    }
    if (cached_float_type) {
        free(cached_float_type);
        cached_float_type = NULL;
    }
    if (cached_char_type) {
        free(cached_char_type);
        cached_char_type = NULL;
    }
}

/* ==================== 基本类型构造函数 ==================== */

/**
 * 创建void类型
 */
Type* new_void_type(void) {
    if (!cached_void_type) {
        init_type_system();
    }
    return cached_void_type;
}

/**
 * 创建int类型
 */
Type* new_int_type(void) {
    if (!cached_int_type) {
        init_type_system();
    }
    return cached_int_type;
}

/**
 * 创建float类型
 */
Type* new_float_type(void) {
    if (!cached_float_type) {
        init_type_system();
    }
    return cached_float_type;
}

/**
 * 创建char类型
 */
Type* new_char_type(void) {
    if (!cached_char_type) {
        init_type_system();
    }
    return cached_char_type;
}

/* ==================== 派生类型构造函数（2.0版本预留）==================== */

/**
 * 创建数组类型
 * @param base 数组元素的基类型
 * @param length 数组长度
 * @return 数组类型对象指针
 */
Type* new_array_type(Type *base, int length) {
    if (!base || length <= 0) {
        fprintf(stderr, "错误: 无效的数组类型参数\n");
        return NULL;
    }
    
    Type *array_type = create_type(TYPE_ARRAY, base->size * length);
    array_type->base = base;
    array_type->array_len = length;
    
    return array_type;
}

/**
 * 创建指针类型
 * @param base 指针指向的基类型
 * @return 指针类型对象指针
 */
Type* new_pointer_type(Type *base) {
    if (!base) {
        fprintf(stderr, "错误: 无效的指针类型参数\n");
        return NULL;
    }
    
    // 指针在x86-64架构下是8字节
    Type *pointer_type = create_type(TYPE_POINTER, 8);
    pointer_type->base = base;
    
    return pointer_type;
}

/**
 * 创建函数类型
 * @param return_type 返回类型
 * @param param_types 参数类型数组
 * @param param_count 参数个数
 * @return 函数类型对象指针
 */
Type* new_function_type(Type *return_type, Type **param_types, int param_count) {
    if (!return_type) {
        fprintf(stderr, "错误: 无效的函数返回类型\n");
        return NULL;
    }
    
    // 函数指针在x86-64架构下是8字节
    Type *func_type = create_type(TYPE_FUNCTION, 8);
    func_type->return_type = return_type;
    func_type->param_count = param_count;
    
    // 复制参数类型数组
    if (param_count > 0 && param_types) {
        func_type->param_types = (Type**)malloc(sizeof(Type*) * param_count);
        if (!func_type->param_types) {
            fprintf(stderr, "错误: 内存分配失败\n");
            free(func_type);
            return NULL;
        }
        memcpy(func_type->param_types, param_types, sizeof(Type*) * param_count);
    } else {
        func_type->param_types = NULL;
    }
    
    return func_type;
}

/**
 * 创建结构体类型
 * @param name 结构体名称
 * @param members 结构体成员链表
 * @return 结构体类型对象指针
 */
Type* new_struct_type(const char *name, StructMember *members) {
    if (!name) {
        fprintf(stderr, "错误: 结构体名称不能为空\n");
        return NULL;
    }
    
    Type *struct_type = create_type(TYPE_STRUCT, 0);  // 大小稍后计算
    struct_type->struct_name = strdup(name);
    struct_type->members = members;
    
    // 计算结构体大小（考虑对齐）
    int total_size = 0;
    int max_alignment = 1;
    
    StructMember *current = members;
    while (current) {
        // 计算成员的对齐要求（x86-64 通常按类型大小对齐）
        int alignment = current->type->size;
        if (alignment > 8) alignment = 8;  // 最大对齐到8字节
        
        // 更新最大对齐
        if (alignment > max_alignment) {
            max_alignment = alignment;
        }
        
        // 对齐当前偏移量
        total_size = (total_size + alignment - 1) / alignment * alignment;
        
        // 设置成员偏移量
        current->offset = total_size;
        
        // 增加大小
        total_size += current->type->size;
        
        current = current->next;
    }
    
    // 结构体总大小需要对齐到最大对齐要求
    struct_type->size = (total_size + max_alignment - 1) / max_alignment * max_alignment;
    
    return struct_type;
}

/**
 * 查找结构体成员
 * @param struct_type 结构体类型
 * @param member_name 成员名称
 * @return 成员指针，如果不存在返回 NULL
 */
StructMember* struct_find_member(Type *struct_type, const char *member_name) {
    if (!struct_type || struct_type->kind != TYPE_STRUCT || !member_name) {
        return NULL;
    }
    
    StructMember *current = struct_type->members;
    while (current) {
        if (current->name && strcmp(current->name, member_name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

/* ==================== 类型属性查询 ==================== */

/**
 * 获取类型大小
 */
int type_size(Type *t) {
    if (!t) {
        return 0;
    }
    return t->size;
}

/**
 * 判断是否为整数类型
 */
bool is_integer_type(Type *t) {
    if (!t) {
        return false;
    }
    return t->kind == TYPE_INT || t->kind == TYPE_CHAR;
}

/**
 * 判断是否为浮点类型
 */
bool is_float_type(Type *t) {
    if (!t) {
        return false;
    }
    return t->kind == TYPE_FLOAT;
}

/**
 * 判断是否为数值类型
 */
bool is_numeric_type(Type *t) {
    if (!t) {
        return false;
    }
    return t->kind == TYPE_INT || 
           t->kind == TYPE_FLOAT || 
           t->kind == TYPE_CHAR;
}

/* ==================== 类型比较 ==================== */

/**
 * 判断两个类型是否完全相同
 */
bool type_equal(Type *a, Type *b) {
    // 处理空指针情况
    if (!a || !b) {
        return a == b;
    }
    
    // 类型种类必须相同
    if (a->kind != b->kind) {
        return false;
    }
    
    // 对于基本类型，种类相同即相同
    if (a->kind == TYPE_VOID || 
        a->kind == TYPE_INT || 
        a->kind == TYPE_FLOAT || 
        a->kind == TYPE_CHAR) {
        return true;
    }
    
    // 对于数组类型，需要比较基类型和长度
    if (a->kind == TYPE_ARRAY) {
        return a->array_len == b->array_len && 
               type_equal(a->base, b->base);
    }
    
    // 对于指针类型，需要比较基类型
    if (a->kind == TYPE_POINTER) {
        return type_equal(a->base, b->base);
    }
    
    // 对于函数类型，需要比较返回类型和所有参数类型
    if (a->kind == TYPE_FUNCTION) {
        // 返回类型必须相同
        if (!type_equal(a->return_type, b->return_type)) {
            return false;
        }
        // 参数数量必须相同
        if (a->param_count != b->param_count) {
            return false;
        }
        // 每个参数类型必须相同
        for (int i = 0; i < a->param_count; i++) {
            if (!type_equal(a->param_types[i], b->param_types[i])) {
                return false;
            }
        }
        return true;
    }
    
    // 其他类型（结构体）暂不实现
    return false;
}

/**
 * 判断两个类型是否兼容（可隐式转换）
 * 
 * 兼容规则：
 * - int 和 float 互相兼容
 * - char 和 int 兼容
 * - char 和 float 兼容
 * - 相同类型总是兼容
 */
bool type_compatible(Type *a, Type *b) {
    // 空指针检查
    if (!a || !b) {
        return false;
    }
    
    // 相同类型总是兼容
    if (type_equal(a, b)) {
        return true;
    }
    
    // int 和 float 互相兼容
    if ((a->kind == TYPE_INT && b->kind == TYPE_FLOAT) ||
        (a->kind == TYPE_FLOAT && b->kind == TYPE_INT)) {
        return true;
    }
    
    // char 和 int 兼容
    if ((a->kind == TYPE_CHAR && b->kind == TYPE_INT) ||
        (a->kind == TYPE_INT && b->kind == TYPE_CHAR)) {
        return true;
    }
    
    // char 和 float 兼容
    if ((a->kind == TYPE_CHAR && b->kind == TYPE_FLOAT) ||
        (a->kind == TYPE_FLOAT && b->kind == TYPE_CHAR)) {
        return true;
    }
    
    return false;
}

/**
 * 判断是否可以进行显式类型转换
 */
bool can_cast(Type *from, Type *to) {
    // 空指针检查
    if (!from || !to) {
        return false;
    }
    
    // 数值类型之间可以互相转换
    if (is_numeric_type(from) && is_numeric_type(to)) {
        return true;
    }
    
    // 指针和整数之间可以转换（2.0版本）
    if (from->kind == TYPE_POINTER && to->kind == TYPE_INT) {
        return true;
    }
    if (from->kind == TYPE_INT && to->kind == TYPE_POINTER) {
        return true;
    }
    
    return false;
}

/* ==================== 类型提升 ==================== */

/**
 * 获取两个类型的提升类型
 * 
 * 提升规则：
 * - int + int → int
 * - float + float → float
 * - int + float → float
 * - char + int → int
 * - char + float → float
 * - char + char → int
 */
Type* promote_type(Type *a, Type *b) {
    // 空指针检查
    if (!a || !b) {
        return NULL;
    }
    
    // 如果有float，提升为float
    if (a->kind == TYPE_FLOAT || b->kind == TYPE_FLOAT) {
        return new_float_type();
    }
    
    // 如果有int（或两个char），提升为int
    if (a->kind == TYPE_INT || b->kind == TYPE_INT ||
        (a->kind == TYPE_CHAR && b->kind == TYPE_CHAR)) {
        return new_int_type();
    }
    
    // 其他情况返回NULL（无法提升）
    return NULL;
}

/* ==================== 类型字符串表示 ==================== */

/**
 * 获取类型名称字符串
 */
const char* type_to_string(Type *t) {
    if (!t) {
        return "unknown";
    }
    
    switch (t->kind) {
        case TYPE_VOID:
            return "void";
        case TYPE_INT:
            return "int";
        case TYPE_FLOAT:
            return "float";
        case TYPE_CHAR:
            return "char";
        case TYPE_ARRAY:
            return "array";
        case TYPE_POINTER:
            return "pointer";
        case TYPE_STRUCT:
            return "struct";
        case TYPE_FUNCTION: {
            // 动态生成函数签名字符串
            static char func_sig[256];
            snprintf(func_sig, sizeof(func_sig), "%s(", 
                     type_to_string(t->return_type));
            
            for (int i = 0; i < t->param_count; i++) {
                if (i > 0) strcat(func_sig, ", ");
                strcat(func_sig, type_to_string(t->param_types[i]));
            }
            strcat(func_sig, ")");
            
            return func_sig;
        }
        default:
            return "unknown";
    }
}

/* ==================== 内存管理 ==================== */

/**
 * 复制类型对象
 */
Type* type_copy(Type *t) {
    if (!t) {
        return NULL;
    }
    
    // 对于基本类型，直接返回缓存的类型
    switch (t->kind) {
        case TYPE_VOID:
            return new_void_type();
        case TYPE_INT:
            return new_int_type();
        case TYPE_FLOAT:
            return new_float_type();
        case TYPE_CHAR:
            return new_char_type();
        default:
            break;
    }
    
    // 对于派生类型，创建新的副本
    Type *copy = (Type*)malloc(sizeof(Type));
    if (!copy) {
        fprintf(stderr, "错误: 内存分配失败\n");
        return NULL;
    }
    
    memcpy(copy, t, sizeof(Type));
    
    // 递归复制基类型
    if (t->base) {
        copy->base = type_copy(t->base);
    }
    
    return copy;
}

/**
 * 释放类型对象
 * 
 * 注意：基本类型不应该被释放，因为它们是缓存的
 */
void type_free(Type *t) {
    if (!t) {
        return;
    }
    
    // 不要释放缓存的基本类型
    if (t == cached_void_type || 
        t == cached_int_type || 
        t == cached_float_type || 
        t == cached_char_type) {
        return;
    }
    
    // 递归释放基类型（如果不是缓存类型）
    if (t->base) {
        type_free(t->base);
    }
    
    // 释放结构体名称
    if (t->struct_name) {
        free(t->struct_name);
    }
    
    // 释放结构体成员（2.0版本）
    StructMember *member = t->members;
    while (member) {
        StructMember *next = member->next;
        if (member->name) {
            free(member->name);
        }
        free(member);
        member = next;
    }
    
    // 释放函数参数类型数组（2.0版本）
    if (t->param_types) {
        free(t->param_types);
    }
    
    // 释放类型对象本身
    free(t);
}

